/*
 *  FakeMonkey.h
 *  MonkeyWorksCore
 *
 *  Created by David Cox on 6/13/05.
 *  Copyright 2005 MIT. All rights reserved.
 *
 */
#ifndef	FAKE_MONKEY_H_
#define	FAKE_MONKEY_H_

#include "MonkeyWorksCore/IODevice.h"
#include "MonkeyWorksCore/ComponentFactory.h"
#include <boost/random.hpp>

enum mFakeMonkeyStatus{M_FAKE_MONKEY_FIXATING = 0, M_FAKE_MONKEY_SACCADING};
enum mFakeMonkeyCapabilities{ M_FAKE_MONKEY_H_GAZE, M_FAKE_MONKEY_V_GAZE,
							  M_FAKE_MONKEY_JUICE, M_FAKE_MONKEY_SPIKE };

#define FAKE_MONKEY_MEAN_FIXATION_DURATION_US 200*1000
#define FAKE_MONKEY_MEAN_SACCADE_DURATION_US 50*1000
#define FAKE_MONKEY_MEAN_SPIKING_RATE 30.0

class mFakeMonkey : public mIODevice {

protected:

	boost::mutex monkey_lock;

	int status;
	
	double saccade_start_h, saccade_start_v;
	double saccade_target_h, saccade_target_v;
	MonkeyWorksTime saccade_start_time_us;
	bool command_pending;
	double saccade_next_target_h, saccade_next_target_v;
	shared_ptr<mScheduleTask> movement_node;
	
	MonkeyWorksTime saccade_duration_us;
	MonkeyWorksTime fixation_duration_us;
	
	boost::shared_ptr <mVariable> spiking_rate;
	shared_ptr<mVariable> spike_var;
	shared_ptr<mScheduleTask> spike_node;
	boost::mt19937 rng;
	
	shared_ptr<mScheduler> scheduler;
	
public:

	mFakeMonkey(const shared_ptr<mScheduler> &a_scheduler, 
				const shared_ptr <mVariable> spiking_rate_ = shared_ptr <mVariable>(new mConstantVariable(FAKE_MONKEY_MEAN_SPIKING_RATE)));
	//mFakeMonkey(const mFakeMonkey& copy);
	//~mFakeMonkey();
	
	virtual bool attachPhysicalDevice();
	virtual bool initializeChannels();
	virtual bool startDeviceIO();
	virtual bool stopDeviceIO();
	virtual bool updateChannel(int index);
	virtual bool shutdown();
	
	void startSaccading();
	void startFixating();
	void spike();
	
	void saccadeTo(const double x, const double y, const MonkeyWorksTime time_to_fixate_at_end_of_saccade);
	void fixate(const MonkeyWorksTime duration);
};



#endif

