/*
 *  ITC18Plugins.cpp
 *  ITC18Plugins
 *
 *  Created by bkennedy on 8/14/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#include "ITC18Plugin.h"
#include "ITC18_IODeviceFactory.h"
using namespace mw;

Plugin *getPlugin(){
    return new ITC18Plugin();
}


void ITC18Plugin::registerComponents(shared_ptr<mwComponentRegistry> registry) {
	registry->registerFactory(std::string("iodevice/itc18"),
							  (ComponentFactory *)(new mITC18_IODeviceFactory()));
	registry->registerFactory(std::string("iochannel/itc18_triggered_analog_snippet"),
							  (ComponentFactory *)(new ITC18_TriggeredAnalogSnippetChannelRequestFactory()));
}
