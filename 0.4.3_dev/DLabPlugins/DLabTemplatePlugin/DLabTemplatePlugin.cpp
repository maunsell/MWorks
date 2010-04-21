/*
 *  DLabTemplatePlugins.cpp
 *  DLabTemplatePlugins
 *
 *  Created by bkennedy on 8/14/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#include "DLabTemplatePlugin.h"
#include "DLabTemplateFactory.h"
#include "MonkeyWorksCore/ComponentFactory.h"

// this function returns a Plugin of this particular type
mw::Plugin *getPlugin(){
    return new mw::DLabTemplatePlugin();
}

// this function registers a Factory (DLabTemplatePluginFactory) with an indentification string.
// The identification is the name of the XML element, and followed by it's attribute "type".  The attribute "type"
// is not required
// For example, a variable is defined as:
// <variable tag="variable_name" default_value=".......
// To parse this the factory registration string is "variable"

// A selection variable is defined as:
// <variable type="selection" tag="random_value" selection=".......
// To parse this, the factory regsitration string is "variable/selection"

// Change "ENTER NAME AND TYPE HERE" to the proper registration string
// JJD notes (Jan 8, 2009):  The "ENTER NAME AND TYPE HERE" field is a string that will basically 
//   tell the parser what to look for in the XML file.  In other words, this string has no effect on anything
//   except how to get a parser to find this component.  The details of the string makeup are in Ben's
//   notes above.  For example, the circle stimulus plugin has the string:  "stimulus/circle".
//   This means that (to properly find this component) the xml should look like this <stimulus type="circle" ... >
//   (in the above case, the "stimulus" is the base component type.  This has implications for the order of parsing, etc.)
//  A list of "base" component categories is here: 
//          ...  <<Ben to add>>

// note that if your plugin has arguments, that is dealt with in the plugin factory (see DLabTemplateFactory.cpp)

void mw::DLabTemplatePlugin::registerComponents(boost::shared_ptr<mw::mwComponentRegistry> registry) {
	registry->registerFactory(std::string("ENTER NAME AND TYPE HERE"),
							  new mw::DLabTemplatePluginFactory());
}
