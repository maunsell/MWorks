/*
 *  CircleStimulus.cpp
 *  MonkeyWorksCore
 *
 *  Created by bkennedy on 8/26/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#include "CircleStimulus.h"
#include <boost/regex.hpp>
using namespace mw;

mCircleStimulus::mCircleStimulus(std::string _tag, 
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

mCircleStimulus::mCircleStimulus(const mCircleStimulus &tocopy) : PointStimulus((const PointStimulus&)tocopy){}

mCircleStimulus::~mCircleStimulus(){ }

Stimulus * mCircleStimulus::frozenClone(){
	shared_ptr<Variable> x_clone(xoffset->frozenClone());
	shared_ptr<Variable> y_clone(yoffset->frozenClone());
	shared_ptr<Variable> xs_clone(xscale->frozenClone());
	shared_ptr<Variable> ys_clone(yscale->frozenClone());
	shared_ptr<Variable> rot_clone(rotation->frozenClone());
	shared_ptr<Variable> alpha_clone(alpha_multiplier->frozenClone());	
	shared_ptr<Variable> r_clone(r->frozenClone());
	shared_ptr<Variable> g_clone(g->frozenClone());
	shared_ptr<Variable> b_clone(b->frozenClone());
	
	mCircleStimulus *clone = new mCircleStimulus(tag,
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


#define TWO_PI 2*3.14159

void mCircleStimulus::drawInUnitSquare(StimulusDisplay *display) {
    
	// draw point at desired location with desired color
	// fill a (0,0) (1,1) box with the right color
    if(r == NULL || g == NULL || b == NULL ){
		merror(M_DISPLAY_MESSAGE_DOMAIN,
			   "NULL color variable in CircleStimulus.");
	}
	
	
    // get current values in these variables.
	GLfloat _r = (float)(*r);
	GLfloat _g = (float)(*g);
	GLfloat _b = (float)(*b);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable (GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
	
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(_r, _g, _b, *alpha_multiplier);
	
	// the number of sections depends on the size of the circle and the location of the screen
	// this needs to be revisted
	int sections = 10*max(xscale->getValue().getFloat(), yscale->getValue().getFloat());
	
	
	glVertex3f(0.5, 0.5, 0.0);
	
	for (int i=0; i<=sections; ++i)
	{
		glVertex3f(0.5 + (0.5 * cos(i * TWO_PI / sections)), 
				   0.5 + (0.5 * sin(i * TWO_PI / sections)),
				   0.0);		
	}
	glEnd();
	
	//mprintf("fixpoint r: %g, g: %g, b: %g", red, green, blue);
	
	//glColor3f(_r,_g,_b);
	//	glVertex3f(0.0,0.0,0.0);
	//	glVertex3f(1.0,0.0,0.0);
	//	glVertex3f(1.0,1.0,0.0);
	//	glVertex3f(0.0,1.0,0.0);
	//	
	//    glEnd();
    
	glDisable(GL_BLEND);
    
    last_r = _r;
    last_g = _g;
    last_b = _b;
    
	
}

Data mCircleStimulus::getCurrentAnnounceDrawData() {
    
    Data announceData(PointStimulus::getCurrentAnnounceDrawData());
	// use image for now
    announceData.addElement(STIM_TYPE,"circle");
    
    return (announceData);
}


