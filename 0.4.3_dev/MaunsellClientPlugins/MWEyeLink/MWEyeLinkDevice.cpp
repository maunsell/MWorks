/*
 *  MWEyeLinkDevice.cpp
 *  MWEyeLink Plugin
 *
 *  Created by Jonathan Hendry on 3/9/2010.
 *  Copyright 2010 hhmi. All rights reserved.
 *
 */

/* Notes to self MH 1000503
 This is how we do setup and cleanup
	* Constructor [called at plugin load time]
		Sets instant variables
	* core calls attachPhysicalDevice()
		-> variableSetup()
	* startup()  [called by core; once, I think]
	* startDeviceIO()  [called by core; every trial]
	* stopDeviceIO()   [called by core; every trial]
	* shutdown() [called by core; once, I think]
	* Destructor
		-> detachPhysicalDevice
 
 What we do:
	Constructor [sets up instance variables]
 
 */

#import <eyelink_core/eyelink.h>
#import <eyelink_core/core_expt.h>

#include "MWEyeLinkDevice.h"
#include "boost/bind.hpp"
#include <MonkeyWorksCore/Component.h>

#define kBufferLength	2048
#define kDIDeadtimeUS	5000			
	
using namespace mw;




// Constructor for MWEyelinkDevice.  MW will later call attachToDevice, which is where we finish our initialization

MWEyeLinkDevice::MWEyeLinkDevice(const boost::shared_ptr <Scheduler> &a_scheduler,
								 const boost::shared_ptr <Variable> _pupilX,
								 const boost::shared_ptr <Variable> _pupilY,
								 const boost::shared_ptr <Variable> _pupilArea,
								 const MonkeyWorksTime update_time)
{
	if (VERBOSE_IO_DEVICE) {
		mprintf("MWEyeLinkDevice: constructor");
	}
	scheduler = a_scheduler;
	pupilX=_pupilX;
	pupilY=_pupilY;
	pupilArea=_pupilArea;
	
	update_period = update_time;
	deviceIOrunning = false;
}

MWEyeLinkDevice::~MWEyeLinkDevice(){ 

	if (VERBOSE_IO_DEVICE) {
		mprintf("MWEyeLinkDevice: destructor\n");
	}
	/*
	if (pulseScheduleNode != NULL) {
		boost::mutex::scoped_lock locker(pulseScheduleNodeLock); 
        pulseScheduleNode->cancel();
		pulseScheduleNode->kill();
    }
	*/
    detachPhysicalDevice();
}

// initialize the device (pre- channel creation/initialization

bool MWEyeLinkDevice::startup(){
	int error;
	bool deviceEnabled;
	bool devicePresent;
	
	if (VERBOSE_IO_DEVICE) {
		mprintf("MWEyeLinkDevice: startup");
	}
	
	if(error=open_eyelink_connection(0)){
		deviceEnabled=NO;
		devicePresent=NO;
	}
	else {
		deviceEnabled=YES;
		devicePresent=YES;
		stop_recording();
	}
	if (deviceEnabled) {
		error = start_recording(0,0,1,1);
		if(error != 0) {
			deviceEnabled=NO;
			devicePresent=NO;
		}
		else {
			eye_used = eyelink_eye_available();
			if (eye_used == BINOCULAR) {
				eye_used = LEFT_EYE;
			}		
			stop_recording();
		}
	}
	
	return true;
}

// External function for scheduling

void *update_eyeposition(const shared_ptr<MWEyeLinkDevice> &gp){
	gp->updateEyePosition();                 
	return(NULL);
}

bool MWEyeLinkDevice::updateEyePosition() {	
	ISAMPLE eLinkSample;
	long sampleEyeVal;
	int error;
	
	if (error = eyelink_get_sample(&eLinkSample)){
		sampleEyeVal = (long)((eLinkSample.gx[eye_used]*.02)-90);
		pupilX->setValue(Data(sampleEyeVal));
		sampleEyeVal = (long)(75-(eLinkSample.gy[eye_used]*.02));
		pupilY->setValue(Data(sampleEyeVal));
		sampleEyeVal = (long)(1+eLinkSample.pa[eye_used]*.02);
		pupilArea->setValue(Data(sampleEyeVal));
	}
	//mprintf("sampleEyeVal=%d",sampleEyeVal);
	return true;
}

bool MWEyeLinkDevice::shutdown(){
	// no special action required
	if (VERBOSE_IO_DEVICE) {
		mprintf("EyeLinkDevice: shutdown");
	}
	return true;
}

// attachPhysicalDevice attempts a connection with hardware.  If it reports failure, MW should 
// give up on using this device and move onto the alternative device

bool MWEyeLinkDevice::attachPhysicalDevice(){                                      // attach next avaialble device to this object
	
	bool success;
	
	attached_device = new IOPhysicalDeviceReference(0, "eyelink");
	
	if (VERBOSE_IO_DEVICE) {
		mprintf("MWEyeLinkDevice: attachPhysicalDevice; start DAQmxBaseResetDevice");
	}
	boost::mutex::scoped_lock lock(MWEyeLinkDriverLock);
	
	
	success = 1;//Check for presence of EyeLink
	if (!success) {
		mprintf("EyeLinkDevice: attachPhysicalDevice; Failed to find eyelink");
	}

	
	if (VERBOSE_IO_DEVICE) {
		if (success == true) {
			mprintf("MWEyeLinkDevice: attachPhysicalDevice; Found EyeLink");
		}
	}
	return success;
}

void MWEyeLinkDevice::detachPhysicalDevice() {
	if (VERBOSE_IO_DEVICE) {
		mprintf("MWEyeLinkDevice: detachPhysicalDevice");
	}
    assert(connected == true); // "Was not connected on entry to detachPhysicalDevice");
	
    boost::mutex::scoped_lock lock(MWEyeLinkDriverLock);  printf("lock detachP\n");
    if (eyelink_is_connected()) {
		set_offline_mode();					// place EyeLink tracker in off-line (idle) mode
		eyecmd_printf("close_data_file");    // close data file
		eyelink_close(1);					 // disconnect from tracker
	}
	close_eyelink_system();					// shut down system (MUST do before exiting)
	

}

// Start the scheduled IO on the Nidaq.  Note that the asynchronous DO is running continuously from the time the
// device is initialized.

bool MWEyeLinkDevice::startDeviceIO(){
	
	if (VERBOSE_IO_DEVICE) {
		mprintf("MWEyeLinkDevice: startDeviceIO");
	}
	if (deviceIOrunning) {
		mwarning(M_IODEVICE_MESSAGE_DOMAIN,
				 "MWEyeLinkDevice startDeviceIO:  startDeviceIO request was made without first stopping IO.  Attempting to stop and restart now.");
		if (!this->stopDeviceIO()) {					// try to stop
			merror(M_IODEVICE_MESSAGE_DOMAIN, "MWEyeLinkDevice IO could not be stopped.  Stop and restart has failed.");
			return false;
		}
	}
	schedule_nodes_lock.lock();
	
	start_recording(0,0,1,0);
	
	setActive(true);
	deviceIOrunning = true;
	
	shared_ptr<MWEyeLinkDevice> this_one = shared_from_this();
	pollScheduleNode = scheduler->scheduleUS(std::string(FILELINE ": ") + tag,
														  (MonkeyWorksTime)0, 
														  update_period, 
														  M_REPEAT_INDEFINITELY, 
														  boost::bind(update_eyeposition, 
																	  this_one),
														  M_DEFAULT_IODEVICE_PRIORITY,
														  M_DEFAULT_IODEVICE_WARN_SLOP_US,
														  M_DEFAULT_IODEVICE_FAIL_SLOP_US,
														  M_MISSED_EXECUTION_DROP);
	schedule_nodes_lock.unlock();
	return true;
}

// Stop the EyeLink collecting data.  This is typically called at the end of each trial.

bool MWEyeLinkDevice::stopDeviceIO(){

	if (VERBOSE_IO_DEVICE) {
		mprintf("MWEyeLinkDevice: stopDeviceIO");
	}
	if (!deviceIOrunning) {
		merror(M_IODEVICE_MESSAGE_DOMAIN, "stopDeviceIO: already stopped on entry");
		return false;
	}
	
	// stop all the scheduled channel checking (i.e. stop calls to "updateChannel")
	// this will stop both flushing of the FIFO and posting of data in the software buffers
	stop_recording();
	if (pollScheduleNode != NULL) {
		boost::mutex::scoped_lock(pollScheduleNodeLock);
        pollScheduleNode->cancel();
    }
	setActive(false);
	
	deviceIOrunning = false;
	return true;
}


boost::shared_ptr<mw::Component> MWEyeLinkDeviceFactory::createObject(std::map<std::string, std::string> parameters,
																   mw::mwComponentRegistry *reg) {
	const char *PUPIL_X = "pupil_x";
	const char *PUPIL_Y = "pupil_y";
	const char *PUPIL_AREA = "pupil_area";
	const char *UPDATE_PERIOD = "data_interval";
	
	REQUIRE_ATTRIBUTES(parameters, UPDATE_PERIOD);
	
	MonkeyWorksTime update_period = reg->getNumber(parameters.find(UPDATE_PERIOD)->second);	

	
	boost::shared_ptr<mw::Variable> pX = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Data(M_INTEGER, 0)));	
	if(parameters.find(PUPIL_X) != parameters.end()) {
		pX = reg->getVariable(parameters.find(PUPIL_X)->second);	
		checkAttribute(pX, 
					   parameters.find("reference_id")->second, 
					   PUPIL_X, 
					   parameters.find(PUPIL_X)->second);
	}
	
	boost::shared_ptr<mw::Variable> pY = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Data(M_INTEGER, 0)));	
	if(parameters.find(PUPIL_Y) != parameters.end()) {
		pY = reg->getVariable(parameters.find(PUPIL_Y)->second);	
		checkAttribute(pY, 
					   parameters.find("reference_id")->second, 
					   PUPIL_Y, 
					   parameters.find(PUPIL_Y)->second);
	}
	
	boost::shared_ptr<mw::Variable> pArea = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Data(M_INTEGER, 0)));	
	if(parameters.find(PUPIL_AREA) != parameters.end()) {
		pArea = reg->getVariable(parameters.find(PUPIL_AREA)->second);	
		checkAttribute(pArea, 
					   parameters.find("reference_id")->second, 
					   PUPIL_AREA, 
					   parameters.find(PUPIL_AREA)->second);
	}	
	
	
	boost::shared_ptr <mw::Scheduler> scheduler = mw::Scheduler::instance(true);
	
	boost::shared_ptr <mw::Component> new_elink = boost::shared_ptr<mw::Component>(new MWEyeLinkDevice(scheduler,
																								pX,
																								pY,
																								pArea,
																								update_period));
																								  

	
	return new_elink;
}	


