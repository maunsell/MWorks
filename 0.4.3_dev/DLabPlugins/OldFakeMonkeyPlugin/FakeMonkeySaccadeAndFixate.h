/*
 *  FakeMonkeySaccadeAndFixate.h
 *  FakeMonkey
 *
 *  Created by labuser on 10/8/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#ifndef FAKE_MONKEY_SACCADE_AND_FIXATE_H
#define FAKE_MONKEY_SACCADE_AND_FIXATE_H

#include "MonkeyWorksCore/TrialBuildingBlocks.h"
#include "FakeMonkey.h"

class mFakeMonkeySaccadeAndFixate : public mAction {
protected:
	shared_ptr<mFakeMonkey> monkey;
	shared_ptr<mVariable> h_loc; 
	shared_ptr<mVariable> v_loc;
	shared_ptr<mVariable> duration;
public:
	mFakeMonkeySaccadeAndFixate(const shared_ptr<mFakeMonkey> &_monkey, 
								const shared_ptr<mVariable> &_h_loc,
								const shared_ptr<mVariable> &_v_loc,
								const shared_ptr<mVariable> &_duration);
	
	virtual ~mFakeMonkeySaccadeAndFixate();
	virtual bool execute();
};

#endif

