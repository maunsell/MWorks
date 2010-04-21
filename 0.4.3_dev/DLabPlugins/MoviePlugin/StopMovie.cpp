/*
 *  StopMovie.cpp
 *  MovieStimulusPlugins
 *
 *  Created by bkennedy on 8/17/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#include "StopMovie.h"

mStopMovie::mStopMovie(shared_ptr<mMovieStimulus> the_movie) : 
Action() 
{
	setName("StopAndRewindMovie");
	movie=the_movie;
}

bool mStopMovie::execute() {	
	movie->stopAndRewind();
    return true;
}


shared_ptr<mw::Component> mStopMovieFactory::createObject(std::map<std::string, std::string> parameters,
													   mwComponentRegistry *reg) {
	
	const char *MOVIE = "movie";
	
	REQUIRE_ATTRIBUTES(parameters, MOVIE);
	
	shared_ptr <mMovieStimulus> the_movie = reg->getObject<mMovieStimulus>(parameters.find(MOVIE)->second);
	
	if(the_movie == 0) {
		throw MissingReferenceException(parameters.find("reference_id")->second, MOVIE, parameters.find(MOVIE)->second);		
	}
	
	shared_ptr <mStopMovie> new_stop_movie_action = shared_ptr<mStopMovie>(new mStopMovie(the_movie));
	return new_stop_movie_action;		
}
