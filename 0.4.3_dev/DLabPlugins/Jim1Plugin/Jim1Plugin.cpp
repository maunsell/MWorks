/*
 *  Jim1Plugins.cpp
 *  Jim1Plugins
 *
 *  Created by bkennedy on 8/14/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#include "Jim1Plugin.h"
#include "Jim1Factory.h"
#include "MonkeyWorksCore/ComponentFactory.h"

// this function returns a Plugin of this particular type
mw::Plugin *getPlugin(){
    return new mw::Jim1Plugin();
}

// this function registers a Factory (Jim1PluginFactory) with an indentification string.
// The identification is the name of the XML element, and followed by it's attribute "type".  The attribute "type"
// is not required
// For example, a variable is defined as:
// <variable tag="variable_name" default_value=".......
// To parse this the factory registration string is "variable"

// A selection variable is defined as:
// <variable type="selection" tag="random_value" selection=".......
// To parse this, the factory regsitration string is "variable/selection"

// Change "ENTER NAME AND TYPE HERE" to the proper registration string
void mw::Jim1Plugin::registerComponents(boost::shared_ptr<mw::mwComponentRegistry> registry) {
	registry->registerFactory(std::string("stimulus/jim1"),
							  new mw::Jim1PluginFactory());
}
