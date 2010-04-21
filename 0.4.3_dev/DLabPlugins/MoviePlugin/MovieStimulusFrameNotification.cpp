/*
 *  MovieStimulusFrameNotification.cpp
 *  MonkeyWorksCore
 *
 *  Created by labuser on 5/23/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#include "MovieStimulusFrameNotification.h"

mMovieStimulusFrameNotification::mMovieStimulusFrameNotification(mMovieStimulus *the_ms){
	movie_stimulus = the_ms;
}

void mMovieStimulusFrameNotification::notify(const Data &data) {
	shared_ptr<Clock> clock = Clock::instance();
	this->notify(data, clock->getCurrentTimeUS());
}

void mMovieStimulusFrameNotification::notify(const Data &data, MonkeyWorksTime time_us) {
	movie_stimulus->stimDisplayUpdateNotification(data, time_us);
}
