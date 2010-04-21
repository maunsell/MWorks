/*
 *  PlayMovie.h
 *  MovieStimulusPlugins
 *
 *  Created by bkennedy on 8/17/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#ifndef PLAY_MOVIE_ACTION_H_
#define PLAY_MOVIE_ACTION_H_

#include "MonkeyWorksCore/TrialBuildingBlocks.h"
#include "MovieStimulus.h"
using namespace mw;


class mPlayMovie : public Action {	
protected:
	shared_ptr<mMovieStimulus> movie;
	
public:
	
	mPlayMovie(shared_ptr<mMovieStimulus> the_movie);
	virtual bool execute();
};

class mPlayMovieFactory : public ComponentFactory {
	virtual boost::shared_ptr<mw::Component> createObject(std::map<std::string, std::string> parameters,
													   mwComponentRegistry *reg);
};


#endif 
// PLAY_MOVIE_ACTION_H_

