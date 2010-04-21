/*
 *  CoxlabCorePlugins.cpp
 *  CoxlabCorePlugins
 *
 *  Created by David Cox on 4/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "CoxlabCorePlugins.h"
#include "NE500.h"
#include "Phidgets.h"
#include "CobraEyeTrackerConduit.h"

mPlugin *getPlugin(){
    return new CoxlabCorePlugin();
}


void CoxlabCorePlugin::registerComponents(shared_ptr<mComponentRegistry> registry) {
	
	registry->registerFactory(std::string("iodevice/ne500"),
							  (mComponentFactory *)(new mNE500DeviceFactory()));
							  
	registry->registerFactory(std::string("iochannel/ne500"),
							  (mComponentFactory *)(new mNE500DeviceChannelFactory()));
							  
							  
	registry->registerFactory(std::string("iodevice/phidget"),
								  (mComponentFactory *)(new mPhidgetDeviceFactory()));
								  
	registry->registerFactory(std::string("iochannel/phidget"),
								  (mComponentFactory *)(new mPhidgetDeviceChannelFactory()));

	registry->registerFactory(std::string("iodevice/cobra_tracker"),
								  (mComponentFactory *)(new mCobraDeviceFactory()));

	registry->registerFactory(std::string("iochannel/cobra_tracker"),
								  (mComponentFactory *)(new mCobraDeviceChannelFactory()));
	
}
