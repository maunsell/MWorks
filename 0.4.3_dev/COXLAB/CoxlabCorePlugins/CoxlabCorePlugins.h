/*
 *  CoxlabCorePlugins.h
 *  CoxlabCorePlugins
 *
 *  Created by David Cox on 4/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <MonkeyWorksCore/Plugin.h>

extern "C"{
    mPlugin *getPlugin();
}



class CoxlabCorePlugin : public mPlugin {
    
	virtual void registerComponents(shared_ptr<mComponentRegistry> registry);	
};
