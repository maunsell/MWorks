/*
 *  FakeMonkey.cpp
 *  MonkeyWorksCore
 *
 *  Created by David Cox on 6/13/05.
 *  Copyright 2005 MIT. All rights reserved.
 *
 */

#include "FakeMonkey.h"
#include "FakeMonkeyJuiceNotification.h"

// a schedulable saccade function
void *fake_monkey_saccade(void *args){
	mFakeMonkey *monkey = (mFakeMonkey *)args;
	
	monkey->startSaccading();
	return 0;
}


void *fake_monkey_fixate(void *args){
	mFakeMonkey *monkey = (mFakeMonkey *)args;
	
	monkey->startFixating();
	return 0;
}

void *fake_monkey_spike(void *args){
	mFakeMonkey *monkey = (mFakeMonkey *)args;
	
	monkey->spike();
	return 0;
}



mFakeMonkey::mFakeMonkey(const boost::shared_ptr<mScheduler> &a_scheduler,
						 const boost::shared_ptr <mVariable> spiking_rate_) {
	spiking_rate = spiking_rate_;
	scheduler = a_scheduler;
	
	status = M_FAKE_MONKEY_FIXATING;
	
	
	// Initialize the rng
	rng.seed((int)scheduler->getClock()->getSystemTimeNS());
	
	saccade_duration_us = FAKE_MONKEY_MEAN_SACCADE_DURATION_US; // KLUDGE for now
	fixation_duration_us = FAKE_MONKEY_MEAN_FIXATION_DURATION_US; // KLUDGE for now
	
	
	
	saccade_start_h = 0;
	saccade_start_v = 0;
	saccade_target_h = 0;
	saccade_target_v = 0;
	
	
	// Define device capabilities
	mIOCapability *cap;
	
	cap = new mIOCapability(M_FAKE_MONKEY_H_GAZE, "H_GAZE", M_INPUT_DATA, 
							M_ANALOG_DATA, M_SOFTWARE_TIMED_SYNCHRONOUS_IO, 
							50, -180.0, 180.0, 32);							 
	registerCapability(cap); 
	
	cap = new mIOCapability(M_FAKE_MONKEY_V_GAZE, "V_GAZE", M_INPUT_DATA, 
							M_ANALOG_DATA, M_SOFTWARE_TIMED_SYNCHRONOUS_IO, 
							50, -180.0, 180.0, 32);							 
	registerCapability(cap);
	
	cap = new mIOCapability(M_FAKE_MONKEY_JUICE, "JUICE", M_OUTPUT_DATA, 
							M_DIGITAL_DATA, M_ASYNCHRONOUS_IO, 
							(double)0, 0, 1, 1);							 
	registerCapability(cap);
	
	
	cap = new mIOCapability(M_FAKE_MONKEY_SPIKE, "SPIKE", M_INPUT_DATA, 
							M_DIGITAL_DATA, M_ASYNCHRONOUS_IO, 
							(double)0, 0, 1, 1);							 
	registerCapability(cap);	
}

//mFakeMonkey::mFakeMonkey(const mFakeMonkey& copy);
//mFakeMonkey::~mFakeMonkey();

// Not clear what to do with these right now
//virtual mExpandableList<mIODeviceReference> *getAvailableDevices(); 
bool mFakeMonkey::attachPhysicalDevice(){                                      // attach next avaialble device to this object
	
	// bypass all of this bullshit for now--
	attached_device = new mIOPhysicalDeviceReference(0, "monkey1");
	
	return true;
}

bool mFakeMonkey::initializeChannels(){
	for(int i = 0; i < channels->getNElements(); i++){
		shared_ptr<mIOChannel> chan = channels->getElement(i);
		mIOCapability *cap = chan->getCapability();
		if(cap->getIdentifier() == M_FAKE_MONKEY_JUICE){
			shared_ptr<mVariableNotification> notif = shared_ptr<mVariableNotification>(new mFakeMonkeyJuiceNotification());			
			chan->getVariable()->addNotification(notif);
		}
		
		if(cap->getIdentifier() == M_FAKE_MONKEY_SPIKE){
			spike_var = chan->getVariable();
		}
	}
	return true;
}


bool mFakeMonkey::startDeviceIO(){
	// schedule updates
	channels_lock.lock();
	int nelements = channels->getNElements();
	schedule_nodes_lock.lock();
	
	
	for(int i = 0; i < nelements; i++){
		
		shared_ptr<mIOChannel> the_channel = channels->getElement(i);
		
		
		if((MonkeyWorksTime)(the_channel->getRequest())->getRequestedSynchronyType() == M_ASYNCHRONOUS_IO){
			//shared_ptr<mScheduleTask> p;  // TODO: attempt to instantiate base class was here
			//schedule_nodes->addReference(p); // place holder...
			continue; // no action required...
		}
		
		
		mUpdateIOChannelArgs args;
		args.device = this;
		args.channel_index = i;
		
		
		
		shared_ptr<mScheduleTask> node = scheduler->scheduleUS(std::string(FILELINE ": ") + the_channel->getName(),
															   (MonkeyWorksTime)0, 
															   (MonkeyWorksTime)(the_channel->getRequest())->getRequestedUpdateIntervalUsec(), 
															   M_REPEAT_INDEFINITELY, 
															   &update_io_channel, 
															   (void *)&args, 
															   (int)sizeof(mUpdateIOChannelArgs),
															   M_DEFAULT_IODEVICE_PRIORITY,
															   M_DEFAULT_IODEVICE_WARN_SLOP_US,
															   M_DEFAULT_IODEVICE_FAIL_SLOP_US);
		
		schedule_nodes.push_back(node);       
	}
	
	schedule_nodes_lock.unlock();
	channels_lock.unlock();
	

	boost::mutex::scoped_lock lock(monkey_lock);
	// schedule the first saccade
	movement_node = scheduler->scheduleUS(FILELINE,
										 0, 
										 0,
										 1,
										 &fake_monkey_saccade, 
										 (void *)this,
										 M_DEFAULT_IODEVICE_PRIORITY, 
										 M_DEFAULT_IODEVICE_WARN_SLOP_US,
										 M_DEFAULT_IODEVICE_FAIL_SLOP_US);
	// schedule the first spike
	spike_node = scheduler->scheduleUS(FILELINE,
									   0, 
									   0,
									   1,
									   &fake_monkey_spike, 
									   (void *)this,
									   M_DEFAULT_IODEVICE_PRIORITY, 
									   M_DEFAULT_IODEVICE_WARN_SLOP_US,
									   M_DEFAULT_IODEVICE_FAIL_SLOP_US);
	return true;
}



void mFakeMonkey::spike(){
	boost::mutex::scoped_lock lock(monkey_lock);
	boost::exponential_distribution<double> dist = boost::exponential_distribution<double>(spiking_rate->getValue().getFloat() / 1000000); 	
	variate_generator<boost::mt19937&, boost::exponential_distribution<double> > sampler = variate_generator<boost::mt19937&, boost::exponential_distribution<double> >(rng,dist);
		
	if(spike_node != NULL){
		spike_node->cancel();
	}
	
	if(spike_var != 0) {
		*spike_var = 1;
	}
	
	float delay = sampler();
	spike_node = scheduler->scheduleUS(FILELINE,
									   delay,
									   0,
									   1, 
									   &fake_monkey_spike, 
									   (void *)this, 
									   M_DEFAULT_IODEVICE_PRIORITY,
									   M_DEFAULT_IODEVICE_WARN_SLOP_US,
									   M_DEFAULT_IODEVICE_FAIL_SLOP_US,
									   M_MISSED_EXECUTION_CATCH_UP);
}



void mFakeMonkey::startSaccading(){
	boost::mutex::scoped_lock lock(monkey_lock);
	
	//mprintf("Saccading...");
	
	// reset where the next saccade will start from (i.e. here)
	saccade_start_h = saccade_target_h;
	saccade_start_v = saccade_target_v;
	
	if(command_pending){
		saccade_target_h = saccade_next_target_h;
		saccade_target_v = saccade_next_target_v;
		command_pending = false;
		//mprintf("saccade h: %g, saccade v: %g (pending command)",saccade_target_h, saccade_target_v);
	} else {
		// can make fancier later...
		saccade_target_h = 80 * ((double)rand() / (double)RAND_MAX) - 40;
		saccade_target_v = 80 * ((double)rand() / (double)RAND_MAX) - 40; 
		//mprintf("saccade h: %g, saccade v: %g",saccade_target_h, saccade_target_v);
	}
	saccade_start_time_us = scheduler->getClock()->getCurrentTimeUS();
	
	status = M_FAKE_MONKEY_SACCADING;
	
	
	if(movement_node != NULL){
		movement_node->cancel();
	}
	
	movement_node = scheduler->scheduleUS(FILELINE,
										saccade_duration_us,
										0,
										1, 
										&fake_monkey_fixate, 
										(void *)this, 
										M_DEFAULT_IODEVICE_PRIORITY,
										M_DEFAULT_IODEVICE_WARN_SLOP_US,
										M_DEFAULT_IODEVICE_FAIL_SLOP_US,
										M_MISSED_EXECUTION_CATCH_UP);
}


void mFakeMonkey::startFixating(){
	boost::mutex::scoped_lock lock(monkey_lock);
	
	//mprintf("Fixating...");
	
	
	status = M_FAKE_MONKEY_FIXATING;

	if(movement_node != NULL){
		movement_node->cancel();
	}
	
	movement_node = scheduler->scheduleUS(FILELINE,
										 fixation_duration_us,
										 0,
										 1, 
										 &fake_monkey_saccade, 
										 (void *)this,
										 M_DEFAULT_IODEVICE_PRIORITY,
										 M_DEFAULT_IODEVICE_WARN_SLOP_US,
										 M_DEFAULT_IODEVICE_FAIL_SLOP_US,
										 M_MISSED_EXECUTION_CATCH_UP);
	
	fixation_duration_us = FAKE_MONKEY_MEAN_FIXATION_DURATION_US;
}



bool mFakeMonkey::stopDeviceIO(){
	boost::mutex::scoped_lock lock(monkey_lock);
	
	status = M_FAKE_MONKEY_FIXATING;
	if(movement_node != NULL){
		movement_node->cancel();
	}
	if(spike_node != NULL){
		spike_node->cancel();
	}	
	
	//mprintf("Stopping fake monkey channel updates");
	stopAllScheduleNodes();
	
	return true;
}


bool mFakeMonkey::updateChannel(int index){
	
	channels_lock.lock();
	
	shared_ptr<mIOChannel> chan = channels->getElement(index);
	mIOCapability* cap = chan->getCapability();
	
	boost::mutex::scoped_lock lock(monkey_lock);
	double percent_done = (double)(scheduler->getClock()->getCurrentTimeUS() - saccade_start_time_us) / (double)saccade_duration_us;
	percent_done = MAX(MIN(1.0, percent_done),0);
	
	double val = 0;
	
	switch (cap->getIdentifier()){
		case M_FAKE_MONKEY_H_GAZE:
		{
			if(status == M_FAKE_MONKEY_SACCADING){
				val = (1.0-percent_done) * saccade_start_h + percent_done * saccade_target_h + 0.05*(double)((double)rand()/(double)RAND_MAX);				
			} else {
				// fixating, hang out at the target
				val = saccade_target_h + 0.05*(double)((double)rand()/(double)RAND_MAX);
			}
			
		}			
			break;
		case M_FAKE_MONKEY_V_GAZE:
			
		{
			if(status == M_FAKE_MONKEY_SACCADING){
				val = (1.0 - percent_done) * saccade_start_v + percent_done * saccade_target_v + 0.05*(double)((double)rand()/(double)RAND_MAX);				
			} else { 
				// fixating
				val = saccade_target_v + 0.05*(double)((double)rand()/(double)RAND_MAX);				
			}
		}			
			
			break;
		default:
			break;
			
	}
	
	val = MAX(MIN(val, 90), -90);
	(chan->getVariable())->setValue(val);
	channels_lock.unlock();
	return true;
}		

bool mFakeMonkey::shutdown(){
	
	return true;
}

void mFakeMonkey::saccadeTo(const double x, 
							const double y, 
							const MonkeyWorksTime time_to_fixate_at_end_of_saccade) {	
	{
		boost::mutex::scoped_lock lock(monkey_lock);
		command_pending = true;
		saccade_next_target_h = x;
		saccade_next_target_v = y;
		fixation_duration_us = time_to_fixate_at_end_of_saccade;
	}
	startSaccading();
}

void mFakeMonkey::fixate(const MonkeyWorksTime duration){
	{
		boost::mutex::scoped_lock lock(monkey_lock);
		fixation_duration_us = duration;
	}
	startFixating();
}



