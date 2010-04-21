/*
 *  Jim1Stimulus.cpp
 *  Jim1Plugin
 *
 *  Created by dicarlo on 1/9/09.
 *  Copyright 2009 MIT. All rights reserved.
 *
 */

#include "Jim1Stimulus.h"
#include <boost/regex.hpp>
using namespace mw;


mJim1Stimulus::mJim1Stimulus(std::string _tag, 
                             shared_ptr<Variable> _xoffset, 
                             shared_ptr<Variable> _yoffset 
                             ) : PointStimulus (_tag, 
                                                _xoffset, 
                                                _yoffset,
                                                shared_ptr<ConstantVariable>(new ConstantVariable(3L)),
                                                shared_ptr<ConstantVariable>(new ConstantVariable(3L)), 
                                                shared_ptr<ConstantVariable>(new ConstantVariable(0L)), 
                                                shared_ptr<ConstantVariable>(new ConstantVariable(1L)), 
                                                shared_ptr<ConstantVariable>(new ConstantVariable(1L)), 
                                                shared_ptr<ConstantVariable>(new ConstantVariable(1L)), 
                                                shared_ptr<ConstantVariable>(new ConstantVariable(1L))) {}   

mJim1Stimulus::mJim1Stimulus(const mJim1Stimulus &tocopy) : PointStimulus((const PointStimulus&)tocopy){}

mJim1Stimulus::~mJim1Stimulus(){ }

Stimulus * mJim1Stimulus::frozenClone(){
	shared_ptr<Variable> x_clone(xoffset->frozenClone());
	shared_ptr<Variable> y_clone(yoffset->frozenClone());
	
	mJim1Stimulus *clone = new mJim1Stimulus(tag,
                                             x_clone,
                                             y_clone
                                             );
	clone->setIsFrozen(true);
	
	return clone;
}


