/*
 *  CircleStimulusPlugins.cpp
 *  CircleStimulusPlugins
 *
 *  Created by bkennedy on 8/14/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#include "CircleStimulusPlugin.h"
#include "CircleStimulusFactory.h"
#include "MonkeyWorksCore/ComponentFactory.h"
using namespace mw;

Plugin *getPlugin(){
    return new mCircleStimulusPlugin();
}


void mCircleStimulusPlugin::registerComponents(shared_ptr<mwComponentRegistry> registry) {
	registry->registerFactory(std::string("stimulus/circle"),
							  (ComponentFactory *)(new mCircleStimulusFactory()));
}
