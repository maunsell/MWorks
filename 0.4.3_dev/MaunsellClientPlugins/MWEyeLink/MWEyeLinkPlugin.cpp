/*
 *  NidaqPlugin.cpp
 *
 *  Created by Jonathan Hendry on 01/01/10.
 *  Copyright 2010 hhmi. All rights reserved.
 *
 */

#include "MWEyeLinkPlugin.h"
#include "MWEyeLinkDevice.h"

using namespace mw;

Plugin *getPlugin(){
    return new MWEyeLinkPlugin();
}


void MWEyeLinkPlugin::registerComponents(shared_ptr<mwComponentRegistry> registry) {
	
	registry->registerFactory(std::string("iodevice/eyelink"),
							  (ComponentFactory *)(new MWEyeLinkDeviceFactory()));
							                              
}
