/*
 *  DLabTemplateFactory.h
 *  MonkeyWorksCore
 *
 *  Created by bkennedy on 8/26/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */


#ifndef DLAB_TEMPLATE__FACTORY_H
#define DLAB_TEMPLATE__FACTORY_H

#include "MonkeyWorksCore/ComponentFactory.h"

// the factory returns a shared_ptr to a Component.  Every experiment object created in MonkeyWorks descends from 
// Component.  
namespace mw {
	class DLabTemplatePluginFactory : public ComponentFactory {
		virtual boost::shared_ptr<mw::Component> createObject(std::map<std::string, std::string> parameters,
															  mwComponentRegistry *reg);
	};
}

#endif
