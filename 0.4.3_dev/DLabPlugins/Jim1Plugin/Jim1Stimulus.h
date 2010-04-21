/*
 *  Jim1Stimulus.h
 *  Jim1Plugin
 *
 *  Created by dicarlo on 1/9/09.
 *  Copyright 2009 MIT. All rights reserved.
 *
 */


#ifndef JIM1_STIMULUS_H
#define JIM1_STIMULUS_H

#include "MonkeyWorksCore/StandardStimuli.h"
using namespace mw;

class mJim1Stimulus : public PointStimulus {
protected:
public:
	mJim1Stimulus(std::string _tag, 
                    shared_ptr<Variable> _xoffset, 
					shared_ptr<Variable> _yoffset);
	mJim1Stimulus(const mJim1Stimulus &tocopy);
	~mJim1Stimulus();
	virtual Stimulus * frozenClone();
	
    // Jim note: for now, use methods of parent (PointStimulus)
	//virtual void drawInUnitSquare(StimulusDisplay *display);
	//virtual Data getCurrentAnnounceDrawData();
};

#endif 
