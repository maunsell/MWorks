/*
 *  MovieStimulus.h
 *  MonkeyWorksCore
 *
 *  Created by labuser on 5/16/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#ifndef MOVIE_STIMULUS_H_
#define MOVIE_STIMULUS_H_

#include <vector>
#include "MovieFrame.h"
#include "MonkeyWorksCore/StimulusNode.h"
#include "MonkeyWorksCore/Scheduler.h"
#include "MonkeyWorksCore/ComponentRegistry_new.h"
#include "MovieStimulusFrameNotification.h"
#include <boost/thread/mutex.hpp>
#include <boost/enable_shared_from_this.hpp>
using namespace mw;


class mMovieStimulusFrameNotification;

class mMovieStimulus : public Stimulus, public boost::enable_shared_from_this<mMovieStimulus> {
	
protected: 
	boost::shared_ptr<Scheduler> scheduler;
	boost::shared_ptr<StimulusDisplay> display;

	boost::shared_ptr<StimulusGroup> stimulus_group;
	boost::shared_ptr<ScheduleTask> schedule_node;
	
	boost::shared_ptr<Variable> frames_per_second;
	boost::shared_ptr<Variable> movie_stats;
	boost::shared_ptr<Variable> error_reporting;
	boost::shared_ptr<Variable> start_frame_index;
	boost::shared_ptr<Variable> end_frame_index;
	boost::shared_ptr<mMovieStimulusFrameNotification> stimDisplayUpdateNotificationObject;
	
	unsigned int current_stimulus_group_index;
	unsigned int updates_scheduled;
	
	std::vector<boost::shared_ptr<std::vector<MonkeyWorksTime> > > times_shown;
	
	bool movie_ended;
	bool movie_will_end;
	bool movie_started;
	MonkeyWorksTime start_time;
	
	boost::mutex movie_lock;
	
public:
	
	mMovieStimulus(const boost::shared_ptr<Scheduler> &a_scheduler,
				   const boost::shared_ptr<StimulusDisplay> &a_display,
				   const std::string &new_tag,
				   const boost::shared_ptr<StimulusGroup> stimulus_group,
				   const boost::shared_ptr<Variable> frames_per_second_var,
				   const boost::shared_ptr<Variable> movie_stats_var,
				   const boost::shared_ptr<Variable> error_reporting_var,
				   const boost::shared_ptr<Variable> start_frame_var,
				   const boost::shared_ptr<Variable> end_frame_var);
	
	Stimulus *frozenClone();
	
	void play();
	void stopAndRewind();
	
	int getNFramesToShow();
	int getNFramesShown();
	
	void draw(StimulusDisplay *display);
	
	void callUpdateDisplay();
	void endMovie();
	Data getCurrentAnnounceDrawData();

	void stimDisplayUpdateNotification(const Data &data, 
									   const MonkeyWorksTime time_us);
};

class mMovieStimulusFactory : public ComponentFactory {
	virtual boost::shared_ptr<mw::Component> createObject(std::map<std::string, std::string> parameters,
													   mwComponentRegistry *reg);
};



#endif /* MOVIE_STIMULUS_H_ */

