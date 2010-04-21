/*
 *  FakeMonkeySaccadeToLocation.h
 *  FakeMonkey
 *
 *  Created by bkennedy on 10/8/08.
 *  Copyright 2008 mit. All rights reserved.
 *
 */

#ifndef FAKE_MONKEY_SACCADE_TO_LOCATION_H
#define FAKE_MONKEY_SACCADE_TO_LOCATION_H

#include "MonkeyWorksCore/TrialBuildingBlocks.h"
#include "FakeMonkey.h"

class mFakeMonkeySaccadeToLocation : public mAction {
protected:
	shared_ptr<mFakeMonkey> monkey;
	shared_ptr<mVariable> h_loc; 
	shared_ptr<mVariable> v_loc;
public:
	mFakeMonkeySaccadeToLocation(const shared_ptr<mFakeMonkey> &_monkey, 
								 const shared_ptr<mVariable> &_h_loc,
								 const shared_ptr<mVariable> &_v_loc);
	
	virtual ~mFakeMonkeySaccadeToLocation();
	virtual bool execute();
};

#endif
