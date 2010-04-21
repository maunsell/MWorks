/*
 *  MovieStimulusPlugins.cpp
 *  MovieStimulusPlugins
 *
 *  Created by bkennedy on 8/14/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#include "MovieStimulusPlugin.h"
//#include "MonkeyWorksCore/ComponentFactory.h"
#include "MovieStimulus.h"
#include "PlayMovie.h"
#include "StopMovie.h"
using namespace mw;

Plugin *getPlugin(){
    return new MovieStimulusPlugin();
}


void MovieStimulusPlugin::registerComponents(shared_ptr<mwComponentRegistry> registry) {
	registry->registerFactory(std::string("stimulus/movie"),
							  (ComponentFactory *)(new mMovieStimulusFactory()));
	
	registry->registerFactory(std::string("action/play_movie"),
							  (ComponentFactory *)(new mPlayMovieFactory()));	

	registry->registerFactory(std::string("action/stop_movie"),
							  (ComponentFactory *)(new mStopMovieFactory()));	
}
