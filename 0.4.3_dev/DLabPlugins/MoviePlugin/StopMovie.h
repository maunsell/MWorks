/*
 *  StopMovie.h
 *  MovieStimulusPlugins
 *
 *  Created by bkennedy on 8/17/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#ifndef STOP_MOVIE_ACTION_H_
#define STOP_MOVIE_ACTION_H_

#include "MonkeyWorksCore/TrialBuildingBlocks.h"
#include "MovieStimulus.h"
using namespace mw;

class mStopMovie : public Action {	
protected:
	shared_ptr<mMovieStimulus> movie;
public:
	mStopMovie(shared_ptr<mMovieStimulus> the_movie);
	virtual bool execute();
};

class mStopMovieFactory : public ComponentFactory {
	virtual boost::shared_ptr<mw::Component> createObject(std::map<std::string, std::string> parameters,
													   mwComponentRegistry *reg);
};


#endif 
// STOP_MOVIE_ACTION_H_

