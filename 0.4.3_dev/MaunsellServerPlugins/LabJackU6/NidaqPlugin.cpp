/*
 *  NidaqPlugin.cpp
 *
 *  Created by Jonathan Hendry on 01/01/10.
 *  Copyright 2010 hhmi. All rights reserved.
 *
 */

#include "NidaqPlugin.h"
#include "NidaqDevice.h"

using namespace mw;

Plugin *getPlugin(){
    return new NidaqPlugin();
}


void NidaqPlugin::registerComponents(shared_ptr<mwComponentRegistry> registry) {
	
	registry->registerFactory(std::string("iodevice/nidaq"),
							  (ComponentFactory *)(new NidaqDeviceFactory()));
							                              
}
