/*
 *  Jim1Factory.h
 *  MonkeyWorksCore
 *
 *  Created by bkennedy on 8/26/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */


#ifndef Jim1_FACTORY_H
#define Jim1_FACTORY_H

#include "MonkeyWorksCore/ComponentFactory.h"

// the factory returns a shared_ptr to a Component.  Every experiment object created in MonkeyWorks descends from 
// Component.  
namespace mw {
	class Jim1PluginFactory : public ComponentFactory {
		virtual boost::shared_ptr<mw::Component> createObject(std::map<std::string, std::string> parameters,
															  mwComponentRegistry *reg);
	};
}

#endif
