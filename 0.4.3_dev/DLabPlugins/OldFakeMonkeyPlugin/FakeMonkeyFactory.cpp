/*
 *  FakeMonkeyFactory.cpp
 *  FakeMonkeyPlugin
 *
 *  Created by labuser on 8/18/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#include "FakeMonkeyFactory.h"
#include "FakeMonkey.h"

shared_ptr<mComponent> mFakeMonkeyFactory::createObject(std::map<std::string, std::string> parameters,
												 mComponentRegistry *reg) {
	shared_ptr <mScheduler> scheduler = reg->getObject<mScheduler>("DefaultScheduler");

	if(scheduler == 0) {
		throw mSimpleException("Trying to create a fake monkey with no scheduler");
	}
	
	shared_ptr <mComponent> new_fake_monkey = shared_ptr<mComponent>(new mFakeMonkey(scheduler));
	return new_fake_monkey;
}
