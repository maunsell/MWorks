/*
 *  ITC18_IODeviceFactory.h
 *  ITC18Plugin
 *
 *  Created by labuser on 8/18/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#include "MonkeyWorksCore/ComponentFactory.h"
using namespace mw;

class mITC18_IODeviceFactory : public ComponentFactory {
	virtual shared_ptr<mw::Component> createObject(std::map<std::string, std::string> parameters,
												   mwComponentRegistry *reg);
};


//itc18_triggered_analog_snippet
class ITC18_TriggeredAnalogSnippetChannelRequestFactory : public ComponentFactory {
	shared_ptr<mw::Component> createObject(std::map<std::string, std::string> parameters,
										   mwComponentRegistry *reg);
};


