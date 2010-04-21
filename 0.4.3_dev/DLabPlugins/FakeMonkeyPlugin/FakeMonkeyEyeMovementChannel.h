/*
 *  FakeMonkeyMovementChannel.h
 *  FakeMonkey
 *
 *  Created by bkennedy on 10/24/08.
 *  Copyright 2008 mit. All rights reserved.
 *
 */

#ifndef FAKE_MONKEY_EYE_MOVEMENT_CHANNEL_H
#define FAKE_MONKEY_EYE_MOVEMENT_CHANNEL_H

#include "FakeMonkeyStatus.h"
#include "MonkeyWorksCore/Clock.h"
#include "MonkeyWorksCore/GenericVariable.h"
using namespace mw;

class mFakeMonkeyEyeMovementChannel : public mw::Component {
protected:
	boost::shared_ptr <Variable> eye_h;
	boost::shared_ptr <Variable> eye_v;

	
	MonkeyWorksTime update_period;
	int samples_per_update;
	shared_ptr <Clock> clock;
	shared_ptr <boost::mutex> monkey_lock;
	shared_ptr <mFakeMonkeyStatus> status;
	shared_ptr <MonkeyWorksTime> saccade_duration;
	shared_ptr <MonkeyWorksTime> saccade_start_time;
	shared_ptr <float> saccade_start_h;
	shared_ptr <float> saccade_target_h;
	shared_ptr <float> saccade_start_v;
	shared_ptr <float> saccade_target_v;
public:
	mFakeMonkeyEyeMovementChannel(const shared_ptr<Variable> &eye_h_variable,
								  const shared_ptr<Variable> &eye_v_variable,
								  const MonkeyWorksTime update_period_,
								  const int samples_per_update_);
	
	MonkeyWorksTime getUpdatePeriod() const;
	
	virtual void setChannelParams(const shared_ptr <Clock> &a_clock,
								  const shared_ptr <boost::mutex> monkey_lock_,
								  const shared_ptr <mFakeMonkeyStatus> &status,
								  const shared_ptr <MonkeyWorksTime> &saccade_start_time,
								  const shared_ptr <MonkeyWorksTime> &saccade_duration,
								  const shared_ptr <float> &saccade_start_h,
								  const shared_ptr <float> &saccade_start_v,
								  const shared_ptr <float> &saccade_target_h,
								  const shared_ptr <float> &saccade_target_v);
	
	void update();
};

#endif
