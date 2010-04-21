/*
 *  MovieStimulus.cpp
 *  MonkeyWorksCore
 *
 *  Created by labuser on 5/16/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#include "MovieStimulus.h"
#include "MonkeyWorksCore/OpenGLContextManager.h"
#include "MonkeyWorksCore/Experiment.h"
#include "boost/bind.hpp"
#include "MovieStimulusFrameNotification.h"

void *nextUpdate(const shared_ptr<mMovieStimulus> &movie);
void *finalFrame(const shared_ptr<mMovieStimulus> &movie);

#define STIM_TYPE_MOVIE "movie"
#define STIM_MOVIE_PLAYING "playing"
#define STIM_MOVIE_CURRENT_FRAME "current_frame"

mMovieStimulus::mMovieStimulus(const shared_ptr<Scheduler> &a_scheduler,
							   const shared_ptr<StimulusDisplay> &a_display,
							   const std::string &new_tag,
							   const shared_ptr<StimulusGroup> new_stimulus_group,
							   const shared_ptr<Variable> frames_per_seconds_var, 
							   const shared_ptr<Variable> movie_stats_var,
							   const shared_ptr<Variable> error_reporting_var,
							   const shared_ptr<Variable> start_frame_var,
							   const shared_ptr<Variable> end_frame_var) : Stimulus(new_tag) {
	scheduler = a_scheduler;
	display = a_display;
	
	stimulus_group = new_stimulus_group;
	frames_per_second = frames_per_seconds_var;
	movie_stats = movie_stats_var;
	error_reporting = error_reporting_var;
	start_frame_index=start_frame_var;
	end_frame_index=end_frame_var;
	movie_started = false;
	
	updates_scheduled = 0;
	
	// connect this to #stimDisplayUpdate
	stimDisplayUpdateNotificationObject = shared_ptr<mMovieStimulusFrameNotification>(new mMovieStimulusFrameNotification(this));
	stimDisplayUpdate->addNotification(stimDisplayUpdateNotificationObject);
	
	current_stimulus_group_index = -1;
}

// what's a frozen movie stim?  I don't know either...return a copy of the existing movie
Stimulus * mMovieStimulus::frozenClone() {
	boost::mutex::scoped_lock locker(movie_lock);
	
	shared_ptr<Variable> frozen_frames_per_second(frames_per_second->frozenClone());
	shared_ptr<Variable> frozen_start_frame_index(start_frame_index->frozenClone());
	shared_ptr<Variable> frozen_end_frame_index(end_frame_index->frozenClone());
	
	//	static int frozen_counter = 0;
	//	frozen_counter++;
	//	
	//	std::ostringstream x;			
	//				
	//	x << frozen_counter;
	//	
	return new mMovieStimulus(scheduler,
							  display,
							  tag, 
							  stimulus_group, 
							  frozen_frames_per_second, 
							  movie_stats, 
							  error_reporting, 
							  frozen_start_frame_index, 
							  frozen_end_frame_index);
}

int mMovieStimulus::getNFramesToShow() {
	boost::mutex::scoped_lock locker(movie_lock);
	return (end_frame_index->getValue().getInteger()-start_frame_index->getValue().getInteger()) + 1;
}

int mMovieStimulus::getNFramesShown() {
	boost::mutex::scoped_lock locker(movie_lock);
	return updates_scheduled;
}

void mMovieStimulus::play() {
	boost::mutex::scoped_lock locker(movie_lock);
	
	if (!movie_started) {
		if(end_frame_index->getValue().getInteger() == -1) {
			shared_ptr<Variable> temp_end_frame_index(new ConstantVariable(Data(M_INTEGER,stimulus_group->getNElements()-1)));
			end_frame_index = temp_end_frame_index;
		}
		
		const double frames_per_us = frames_per_second->getValue().getFloat()/1000000;

		movie_started = true;
		movie_ended = false;
		movie_will_end = false;
		start_time = scheduler->getClock()->getCurrentTimeUS();
		updates_scheduled = 0;
		
		times_shown.clear();
		
		for(int i=0; 
			i<stimulus_group->getNElements(); 
			++i) {
			// each element of the vector will have a frame and the time when the next frame is shown (also describes when this frame is "erased")
			
			shared_ptr<StimulusNode> p(new StimulusNode(stimulus_group->getElement(i)));
			
			// store a hash of {'stim name' => 'frame index'}
			//    this is used to look up the stimuli when they are announced via #stimDisplayUpdate
			// index_hash.insert(std::pair<std::string, int>(p->getCurrentAnnounceDrawData().getElement(STIM_NAME), i));
			shared_ptr<std::vector<MonkeyWorksTime> > times(new std::vector<MonkeyWorksTime>());
			times_shown.push_back(times);
		}		
		
		shared_ptr <mMovieStimulus> shared_ptr_to_this = shared_from_this();
		
		schedule_node = scheduler->scheduleUS(FILELINE,
											  0,
											  (MonkeyWorksTime)(1/frames_per_us), 
											  (end_frame_index->getValue().getInteger()-start_frame_index->getValue().getInteger()) + 1, 
											  boost::bind(nextUpdate, shared_ptr_to_this),
											  M_DEFAULT_PRIORITY,
											  M_DEFAULT_WARN_SLOP_US,
											  M_DEFAULT_FAIL_SLOP_US,
											  M_MISSED_EXECUTION_CATCH_UP);	
	}	
}

void mMovieStimulus::stopAndRewind() {
	boost::mutex::scoped_lock locker(movie_lock);
	
	// just drew the final frame
	movie_ended = true;
	movie_started = false;
	current_stimulus_group_index = -1;
	
	// cancel any existing updates
	schedule_node->cancel();	
	//schedule_node->kill();	
}

void mMovieStimulus::draw(StimulusDisplay *display) {	
	boost::mutex::scoped_lock locker(movie_lock);
	
	// this is the first draw that the movie has recieved...schedule the display updates accordingly
	if(!movie_ended && movie_started) {
		const double frames_per_us = frames_per_second->getValue().getFloat()/1000000;
		const int number_of_frames_to_show = (end_frame_index->getValue().getInteger()-start_frame_index->getValue().getInteger()) + 1;
		// don't go over the end of the series of frames when you get to it (i.e. the max frame to reach is the last one)		
		current_stimulus_group_index = min((long)floor((scheduler->getClock()->getCurrentTimeUS()-start_time)*frames_per_us), end_frame_index->getValue().getInteger());
		
		stimulus_group->getElement(current_stimulus_group_index)->draw(display);
		if(current_stimulus_group_index >= (unsigned int)end_frame_index->getValue().getInteger()) {
			// just drew the final frame
			movie_will_end = true;
			
			// cancel any existing updates
			schedule_node->kill();
			
			// schedule the time to hide the last frame
			const MonkeyWorksTime time_to_hide_final_frame = start_time + (1/frames_per_us)*number_of_frames_to_show;
			
			shared_ptr <mMovieStimulus> shared_ptr_to_this = shared_from_this();
			
			schedule_node = scheduler->scheduleUS(FILELINE,
												  time_to_hide_final_frame - scheduler->getClock()->getCurrentTimeUS(),
												  0, 
												  1, 
												  boost::bind(finalFrame, shared_ptr_to_this),
												  M_DEFAULT_PRIORITY,
												  M_DEFAULT_WARN_SLOP_US,
												  M_DEFAULT_FAIL_SLOP_US,
												  M_MISSED_EXECUTION_CATCH_UP);	
			
		}
	}
}

void mMovieStimulus::endMovie() {
	{
		boost::mutex::scoped_lock locker(movie_lock);
		
		schedule_node->kill();
		
		// package up the results of the movie
		Data times_shown_report(M_LIST, 1);		
		int total_frames_shown = 0;
		
		for(std::vector<shared_ptr<std::vector<MonkeyWorksTime> > >::const_iterator i = times_shown.begin();
			i != times_shown.end();
			++i) {
			Data times_shown_for_stim(M_LIST, 1);
			
			for(std::vector<MonkeyWorksTime>::const_iterator j = (*i)->begin();
				j != (*i)->end();
				++j) {
				times_shown_for_stim.addElement(*j);
				++total_frames_shown;
			}
			
			times_shown_report.addElement(times_shown_for_stim);
		}
		
		const bool error_occured = total_frames_shown != (end_frame_index->getValue().getInteger()-start_frame_index->getValue().getInteger()) + 1;
		
		Data movie_stats_report(M_DICTIONARY, 7);
		movie_stats_report.addElement("movie_tag", tag);
		movie_stats_report.addElement("times_shown", times_shown_report);
		movie_stats_report.addElement("frames_per_second", frames_per_second->getValue());
		movie_stats_report.addElement("start_frame_index", start_frame_index->getValue());
		movie_stats_report.addElement("end_frame_index", end_frame_index->getValue());
		movie_stats_report.addElement("total_frames_shown", Data(M_INTEGER, total_frames_shown));
		movie_stats_report.addElement("error_occured", error_occured);
		
		movie_stats->setValue(movie_stats_report);
		error_reporting->setValue(error_occured);
	}
	
	display->updateDisplay();		
}



void mMovieStimulus::callUpdateDisplay() {
	{
		boost::mutex::scoped_lock locker(movie_lock);
		++updates_scheduled;
	}

	display->asynchronousUpdateDisplay();
}

void mMovieStimulus::stimDisplayUpdateNotification(const Data &data, const MonkeyWorksTime time_us) {
	boost::mutex::scoped_lock locker(movie_lock);
	
	if(movie_started && !movie_ended) {
		
		// go through each #stimDisplayUpdate set and pull out the frames that are shown in the movie	
		for(int i=0; i<data.getNElements(); ++i) {
			std::string stim_tag(data.getElement(i).getElement(STIM_NAME));
			
			if(stim_tag == tag) {
				//				std::cerr << "current_stimulus_group_index = " << current_stimulus_group_index << std::endl;
				//				std::cerr << "times_show.size() = " << times_shown.size() << std::endl;
				if(current_stimulus_group_index < times_shown.size()) {
					shared_ptr<std::vector<MonkeyWorksTime> > times_to_date = times_shown.at(current_stimulus_group_index);
					times_to_date->push_back(time_us);
					//			std::map<std::string, std::vector<shared_ptr<std::vector<MonkeyWorksTime> > >::size_type>::iterator iter = index_hash.find(stim_tag);
					//			if(iter != index_hash.end()) {			
					//				shared_ptr<std::vector<MonkeyWorksTime> > times_to_date = times_shown.at(iter->second);
					//				times_to_date->push_back(time_us);
					//			}
				}
			}
		}
		
		// if the movie is planned on ending after this frame, end it
		if(movie_will_end) {
			movie_ended = true;
			current_stimulus_group_index = -1;
		}
	}
}

Data mMovieStimulus::getCurrentAnnounceDrawData() {
	boost::mutex::scoped_lock locker(movie_lock);
	//	mprintf("%s, announcing: %d", tag.c_str(), current_stimulus_group_index);
	//
	//	if(movie_playing && !movie_ended) {
	Data announceData(M_DICTIONARY, 3);
	announceData.addElement(STIM_NAME,tag);        // char
	announceData.addElement(STIM_ACTION,STIM_ACTION_DRAW);
	announceData.addElement(STIM_TYPE,STIM_TYPE_MOVIE);  
	announceData.addElement(STIM_MOVIE_PLAYING, Data(movie_started && !movie_ended));  
	announceData.addElement(STIM_MOVIE_CURRENT_FRAME, Data((long)current_stimulus_group_index));  
	return (announceData);
}

void *nextUpdate(const shared_ptr<mMovieStimulus> &movie){
	movie->callUpdateDisplay();
    return 0;
}

void *finalFrame(const shared_ptr<mMovieStimulus> &movie){
	movie->endMovie();
    return 0;
}

shared_ptr<mw::Component> mMovieStimulusFactory::createObject(std::map<std::string, std::string> parameters,
														   mwComponentRegistry *reg) {
	const char *TAG = "tag";
	const char *STIMULUS_GROUP = "stimulus_group";
	const char *FRAMES_PER_SECOND = "frames_per_second";
	const char *MOVIE_STATS = "movie_stats";
	const char *ERROR_REPORTING = "error_reporting";
	const char *START_FRAME_INDEX = "start_frame_index";
	const char *END_FRAME_INDEX = "end_frame_index";
	
	
	REQUIRE_ATTRIBUTES(parameters, 
					   TAG, 
					   STIMULUS_GROUP,
					   FRAMES_PER_SECOND,
					   MOVIE_STATS,
					   ERROR_REPORTING);
	
	std::string tagname(parameters.find(TAG)->second);
	
	boost::shared_ptr<StimulusGroup> the_group = reg->getObject<StimulusGroup>(parameters.find(STIMULUS_GROUP)->second);
	if(the_group == 0) {
		throw MissingReferenceException(parameters.find("reference_id")->second, STIMULUS_GROUP, parameters.find(STIMULUS_GROUP)->second);		
	}	
	
	shared_ptr<Variable> frames_per_second = reg->getVariable(parameters.find(FRAMES_PER_SECOND)->second);	
	checkAttribute(frames_per_second, parameters.find("reference_id")->second, FRAMES_PER_SECOND, parameters[FRAMES_PER_SECOND]);
	
	shared_ptr<Variable> movie_stats = reg->getVariable(parameters.find(MOVIE_STATS)->second);	
	checkAttribute(movie_stats, parameters.find("reference_id")->second, MOVIE_STATS, parameters.find(MOVIE_STATS)->second);
	
	shared_ptr<Variable> error_reporting = reg->getVariable(parameters.find(ERROR_REPORTING)->second);	
	checkAttribute(error_reporting, parameters.find("reference_id")->second, ERROR_REPORTING, parameters.find(ERROR_REPORTING)->second);
	
	
	boost::shared_ptr<Variable> start_frame_index = boost::shared_ptr<Variable>(new ConstantVariable(Data(M_INTEGER, 0)));	
	if(parameters.find(START_FRAME_INDEX) != parameters.end()) {
		start_frame_index = reg->getVariable(parameters.find(START_FRAME_INDEX)->second);	
		checkAttribute(start_frame_index, 
					   parameters.find("reference_id")->second, 
					   START_FRAME_INDEX, 
					   parameters.find(START_FRAME_INDEX)->second);
	}
	
	boost::shared_ptr<Variable> end_frame_index = boost::shared_ptr<Variable>(new ConstantVariable(Data(M_INTEGER, -1)));	
	if(parameters.find(END_FRAME_INDEX) != parameters.end()) {
		end_frame_index = reg->getVariable(parameters.find(END_FRAME_INDEX)->second);	
		checkAttribute(end_frame_index, 
					   parameters.find("reference_id")->second, 
					   END_FRAME_INDEX, 
					   parameters.find(END_FRAME_INDEX)->second);
	}
	
	boost::shared_ptr <Scheduler> scheduler = Scheduler::instance();
	if(scheduler == 0) {
		throw SimpleException("Attempt to create movie stimulus without a valid scheduler");
	}
	
	boost::shared_ptr <StimulusDisplay> display = GlobalCurrentExperiment->getStimulusDisplay();
	if(display == 0) {
		throw SimpleException("Attempt to create movie stimulus without a valid stimulus display");
	}
	
	shared_ptr <mMovieStimulus> new_movie = shared_ptr<mMovieStimulus>(new mMovieStimulus(scheduler,
																						  display,
																						  tagname, 
																						  the_group,
																						  frames_per_second,
																						  movie_stats,
																						  error_reporting,
																						  start_frame_index, 
																						  end_frame_index));
	
	shared_ptr <StimulusNode> thisStimNode = shared_ptr<StimulusNode>(new StimulusNode(new_movie));
	reg->registerStimulusNode(tagname, thisStimNode);
	
	
	return new_movie;
}
