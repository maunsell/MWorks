/*
 *  MWEyeLinkPlugin.h
 *
 *
 *  Created by Jonathan Hendry on 01/01/10.
 *  Copyright 2010 hhmi. All rights reserved.
 *
 */

#include <MonkeyWorksCore/Plugin.h>
#include <boost/shared_ptr.hpp>
    

extern "C"{
    mw::Plugin *getPlugin();
}



class MWEyeLinkPlugin : public mw::Plugin {
    
	virtual void registerComponents(boost::shared_ptr<mw::mwComponentRegistry> registry);	
};

