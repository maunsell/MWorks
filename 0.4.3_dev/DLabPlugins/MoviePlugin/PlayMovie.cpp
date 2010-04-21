/*
 *  PlayMovie.cpp
 *  MovieStimulusPlugins
 *
 *  Created by bkennedy on 8/17/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#include "PlayMovie.h"

mPlayMovie::mPlayMovie(shared_ptr<mMovieStimulus> the_movie) : 
Action() 
{
	setName("PlayMovie");
	movie=the_movie;
}

bool mPlayMovie::execute() {	
	movie->play();
    return true;
}

shared_ptr<mw::Component> mPlayMovieFactory::createObject(std::map<std::string, std::string> parameters,
													   mwComponentRegistry *reg) {
	
	const char *MOVIE = "movie";
	
	REQUIRE_ATTRIBUTES(parameters, MOVIE);
	
	shared_ptr <mMovieStimulus> the_movie = reg->getObject<mMovieStimulus>(parameters.find(MOVIE)->second);
	
	if(the_movie == 0) {
		throw MissingReferenceException(parameters.find("reference_id")->second, MOVIE, parameters.find(MOVIE)->second);		
	}
	
	shared_ptr <mPlayMovie> new_play_movie_action = shared_ptr<mPlayMovie>(new mPlayMovie(the_movie));
	return new_play_movie_action;		
}


