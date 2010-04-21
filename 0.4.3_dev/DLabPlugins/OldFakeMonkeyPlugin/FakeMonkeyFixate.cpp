/*
 *  FakeMonkeyFixate.cpp
 *  FakeMonkey
 *
 *  Created by bkennedy on 10/8/08.
 *  Copyright 2008 mit. All rights reserved.
 *
 */

#include "FakeMonkeyFixate.h"
#include "MonkeyWorksCore/GenericVariable.h"
#include "MonkeyWorksCore/ComponentRegistry_new.h"
#include "FakeMonkey.h"

mFakeMonkeyFixate::mFakeMonkeyFixate(const shared_ptr<mFakeMonkey> &_monkey, 
									 const shared_ptr<mVariable> &_duration){
	monkey = _monkey;
	duration = _duration;
	setName("FakeMonkeyFixate");
}

mFakeMonkeyFixate::~mFakeMonkeyFixate(){}

bool mFakeMonkeyFixate::execute(){	
	if(monkey == 0){
		mwarning(M_IODEVICE_MESSAGE_DOMAIN,
				 "Attempt to direct a NULL monkey");
	}
	
	monkey->fixate(duration->getValue().getInteger());
    
    return true;
}



