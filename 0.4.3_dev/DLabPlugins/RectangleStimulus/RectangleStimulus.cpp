/*
 *  RectangleStimulus.cpp
 *  MonkeyWorksCore
 *
 *  Created by bkennedy on 8/26/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#include "RectangleStimulus.h"
#include <boost/regex.hpp>
using namespace mw;

mRectangleStimulus::mRectangleStimulus(std::string _tag, 
								 shared_ptr<Variable> _xoffset, 
								 shared_ptr<Variable> _yoffset, 
								 shared_ptr<Variable> _xscale, 
								 shared_ptr<Variable> _yscale,
								 shared_ptr<Variable> _rot,
								 shared_ptr<Variable> _alpha,
								 shared_ptr<Variable> _r, 
								 shared_ptr<Variable> _g, 
								 shared_ptr<Variable> _b) : PointStimulus (_tag, 
																			 _xoffset, 
																			 _yoffset,
																			 _xscale ,
																			 _yscale, 
																			 _rot, 
																			 _alpha, 
																			 _r, 
																			 _g, 
																			 _b) {}   

mRectangleStimulus::mRectangleStimulus(const mRectangleStimulus &tocopy) : PointStimulus((const PointStimulus&)tocopy){}

mRectangleStimulus::~mRectangleStimulus(){ }

Stimulus * mRectangleStimulus::frozenClone(){
	shared_ptr<Variable> x_clone(xoffset->frozenClone());
	shared_ptr<Variable> y_clone(yoffset->frozenClone());
	shared_ptr<Variable> xs_clone(xscale->frozenClone());
	shared_ptr<Variable> ys_clone(yscale->frozenClone());
	shared_ptr<Variable> rot_clone(rotation->frozenClone());
	shared_ptr<Variable> alpha_clone(alpha_multiplier->frozenClone());	
	shared_ptr<Variable> r_clone(r->frozenClone());
	shared_ptr<Variable> g_clone(g->frozenClone());
	shared_ptr<Variable> b_clone(b->frozenClone());
	
	mRectangleStimulus *clone = new mRectangleStimulus(tag,
												 x_clone,
												 y_clone,
												 xs_clone,
												 ys_clone,
												 rot_clone,
												 alpha_clone,
												 r_clone,
												 g_clone,
												 b_clone
												 );
	clone->setIsFrozen(true);
	
	return clone;
}

Data mRectangleStimulus::getCurrentAnnounceDrawData() {
    
    Data announceData(PointStimulus::getCurrentAnnounceDrawData());
	// use image for now
    announceData.addElement(STIM_TYPE,"rectangle");
    
    return (announceData);
}

