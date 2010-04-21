/*
 *  FakeMonkeyPlugin.cpp
 *  FakeMonkeyPlugin
 *
 *  Created by bkennedy on 8/14/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#include "FakeMonkeyPlugin.h"
#include "FakeMonkeyFactory.h"
#include "FakeMonkeySaccadeToLocationFactory.h"
#include "FakeMonkeySaccadeAndFixateFactory.h"
#include "FakeMonkeyFixateFactory.h"

mPlugin *getPlugin(){
    return new mFakeMonkeyPlugin();
}


void mFakeMonkeyPlugin::registerComponents(shared_ptr<mComponentRegistry> registry) {
	registry->registerFactory(std::string("iodevice/fake_monkey"),
							  (mComponentFactory *)(new mFakeMonkeyFactory()));

	registry->registerFactory(std::string("action/fake_monkey_saccade_to_location"),
							  (mComponentFactory *)(new mFakeMonkeySaccadeToLocationFactory()));
	
	registry->registerFactory(std::string("action/fake_monkey_saccade_and_fixate"),
							  (mComponentFactory *)(new mFakeMonkeySaccadeAndFixateFactory()));
	
	registry->registerFactory(std::string("action/fake_monkey_fixate"),
							  (mComponentFactory *)(new mFakeMonkeyFixateFactory()));
}
