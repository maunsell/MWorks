/*
 *  MovieFrame.h
 *  MonkeyWorksCore
 *
 *  Created by labuser on 5/16/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#ifndef MOVIE_FRAME_H_
#define MOVIE_FRAME_H_

#include "MonkeyWorksCore/StimulusNode.h"
using namespace mw;

class mMovieFrame {
private:
	shared_ptr <StimulusNode> frame_node;
	MonkeyWorksTime next_frame_at;
	std::string stim_tag;
public:
	mMovieFrame(const shared_ptr<StimulusNode> frame, 
				const MonkeyWorksTime next_frame_time);
	shared_ptr<StimulusNode> stimNode();
	MonkeyWorksTime nextFrameTime() const;
};



#endif /* MOVIE_FRAME */


