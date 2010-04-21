/*
 *  CircleStimulus.h
 *  MonkeyWorksCore
 *
 *  Created by bkennedy on 8/26/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#ifndef CIRCLE_STIMULUS_H
#define CIRCLE_STIMULUS_H

#include "MonkeyWorksCore/StandardStimuli.h"
using namespace mw;

class mCircleStimulus : public PointStimulus {
protected:
public:
	mCircleStimulus(std::string _tag, shared_ptr<Variable> _xoffset, 
					shared_ptr<Variable> _yoffset, 
					shared_ptr<Variable> _xscale,
					shared_ptr<Variable> _yscale,
					shared_ptr<Variable> _rot,
					shared_ptr<Variable> _alpha,
					shared_ptr<Variable> _r, 
					shared_ptr<Variable> _g, 
					shared_ptr<Variable> _b);
	mCircleStimulus(const mCircleStimulus &tocopy);
	~mCircleStimulus();
	virtual Stimulus * frozenClone();
	
	virtual void drawInUnitSquare(StimulusDisplay *display);
	virtual Data getCurrentAnnounceDrawData();
};

#endif 
