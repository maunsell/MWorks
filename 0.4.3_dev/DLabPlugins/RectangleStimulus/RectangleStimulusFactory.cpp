/*
 *  RectangleStimulusFactory.cpp
 *  MonkeyWorksCore
 *
 *  Created by bkennedy on 8/26/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#include "RectangleStimulusFactory.h"
#include "RectangleStimulus.h"
#include <boost/regex.hpp>
#include "MonkeyWorksCore/ComponentRegistry_new.h"
#include "MonkeyWorksCore/ParsedColorTrio.h"


shared_ptr<mw::Component> mRectangleStimulusFactory::createObject(std::map<std::string, std::string> parameters,
															mwComponentRegistry *reg) {
	REQUIRE_ATTRIBUTES(parameters, 
					   "tag", 
					   "x_size", 
					   "y_size", 
					   "x_position", 
					   "y_position", 
					   "rotation", 
					   "color"
					   );
	
	std::string tagname(parameters.find("tag")->second);
	shared_ptr<Variable> x_size = reg->getVariable(parameters.find("x_size")->second);	
	shared_ptr<Variable> y_size = reg->getVariable(parameters.find("y_size")->second);	
	shared_ptr<Variable> x_position = reg->getVariable(parameters.find("x_position")->second);	
	shared_ptr<Variable> y_position = reg->getVariable(parameters.find("y_position")->second);	
	shared_ptr<Variable> rotation = reg->getVariable(parameters.find("rotation")->second);	
	shared_ptr<Variable> alpha_multiplier = 
	reg->getVariable(parameters["alpha_multiplier"], "1");
	
	ParsedColorTrio pct(reg, parameters.find("color")->second);
	shared_ptr<Variable> r = pct.getR();
	shared_ptr<Variable> g = pct.getG();
	shared_ptr<Variable> b = pct.getB();
	
	
	
	checkAttribute(x_size, parameters.find("reference_id")->second, "x_size", parameters.find("x_size")->second);                                                  
	checkAttribute(y_size, parameters.find("reference_id")->second, "y_size", parameters.find("y_size")->second);                                                  
	checkAttribute(x_position, parameters.find("reference_id")->second, "x_position", parameters.find("x_position")->second);                                      
	checkAttribute(y_position, parameters.find("reference_id")->second, "y_position", parameters.find("y_position")->second);                                      
	checkAttribute(rotation, parameters.find("reference_id")->second, "rotation", parameters.find("rotation")->second);                                            
	checkAttribute(alpha_multiplier, parameters.find("reference_id")->second, "alpha_multiplier", parameters.find("alpha_multiplier")->second);                    
	
	if(GlobalCurrentExperiment == 0) {
		throw SimpleException("no experiment currently defined");		
	}
	
	shared_ptr<StimulusDisplay> defaultDisplay = GlobalCurrentExperiment->getStimulusDisplay();
	if(defaultDisplay == 0) {
		throw SimpleException("no stimulusDisplay in current experiment");
	}
	
	
	shared_ptr <mRectangleStimulus> new_rectangle_stimulus = shared_ptr<mRectangleStimulus>(new mRectangleStimulus(tagname, 
																									   x_position,
																									   y_position,
																									   x_size,
																									   y_size,
																									   rotation,
																									   alpha_multiplier,
																									   r,
																									   g,
																									   b));
	
	
	
	new_rectangle_stimulus->load(defaultDisplay.get());
	shared_ptr <StimulusNode> thisStimNode = shared_ptr<StimulusNode>(new StimulusNode(new_rectangle_stimulus));
	reg->registerStimulusNode(tagname, thisStimNode);
	
	return new_rectangle_stimulus;
}

