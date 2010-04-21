/*
 *  RectangleStimulus.h
 *  MonkeyWorksCore
 *
 *  Created by bkennedy on 8/26/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#ifndef RECTANGLE_STIMULUS_H
#define RECTANGLE_STIMULUS_H

#include "MonkeyWorksCore/StandardStimuli.h"

using namespace mw;

class mRectangleStimulus : public PointStimulus {
protected:
public:
	mRectangleStimulus(std::string _tag, shared_ptr<Variable> _xoffset, 
					shared_ptr<Variable> _yoffset, 
					shared_ptr<Variable> _xscale,
					shared_ptr<Variable> _yscale,
					shared_ptr<Variable> _rot,
					shared_ptr<Variable> _alpha,
					shared_ptr<Variable> _r, 
					shared_ptr<Variable> _g, 
					shared_ptr<Variable> _b);
	mRectangleStimulus(const mRectangleStimulus &tocopy);
	~mRectangleStimulus();
	virtual Stimulus * frozenClone();
	
	virtual Data getCurrentAnnounceDrawData();
};

#endif 
