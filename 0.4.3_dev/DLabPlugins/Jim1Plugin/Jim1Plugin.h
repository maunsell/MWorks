/*
 *  Jim1Plugins.h
 *  Jim1Plugins
 *
 *  Created by bkennedy on 8/14/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#ifndef Jim1PLUGIN_H
#define Jim1PLUGIN_H

#include "MonkeyWorksCore/Plugin.h"

// this is the class definition of the plugin.  The core knows to look for a function
// called "getPlugin()" that will return this particular plugin, and an object of type Plugin
// has a function called registerComponent. 

extern "C"{
	mw::Plugin *getPlugin();
}

namespace mw {
	class Jim1Plugin : public Plugin {
		
		virtual void registerComponents(shared_ptr<mwComponentRegistry> registry);	
	};
}


#endif
