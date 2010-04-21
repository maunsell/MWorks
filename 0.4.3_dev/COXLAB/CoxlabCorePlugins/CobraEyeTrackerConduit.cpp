/*
 *  CobraEyeTrackerConduit.cpp
 *  CoxlabCorePlugins
 *
 *  Created by David Cox on 9/24/08.
 *  Copyright 2008 The Rowland Institute at Harvard. All rights reserved.
 *
 */

#include "CobraEyeTrackerConduit.h"
	

	
void mCobraDevice::addChild(std::map<std::string, std::string> parameters,
										mComponentRegistry *reg,
										shared_ptr<mComponent> _child){

	shared_ptr<mCobraChannel> channel = dynamic_pointer_cast<mCobraChannel, mComponent>(_child);
	if(channel == NULL){
		throw mSimpleException("Attempt to access an invalid Cobra EyeTracker channel object");
	}
    
    conduit->registerCallback(channel->getCapability(), bind(&mCobraChannel::update, channel, _1)); 
}


shared_ptr<mComponent> mCobraDeviceFactory::createObject(std::map<std::string, std::string> parameters,
												 mComponentRegistry *reg) {
												 
	REQUIRE_ATTRIBUTES(parameters, "device_name");
	
	string resource_name = parameters["device_name"];
    shared_ptr <mComponent> newDevice(new mCobraDevice(resource_name));

	
    return newDevice;
}

