/*
 *  FakeMonkeyPlugin.h
 *  FakeMonkeyPlugin
 *
 *  Created by bkennedy on 8/14/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#ifndef FAKE_MONKEY_PLUGIN_H
#define FAKE_MONKEY_PLUGIN_H

#include <MonkeyWorksCore/Plugin.h>

extern "C"{
    mPlugin *getPlugin();
}

class mFakeMonkeyPlugin : public mPlugin {
    
	virtual void registerComponents(shared_ptr<mComponentRegistry> registry);	
};

#endif
