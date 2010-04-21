/*
 *  Jim1Factory.cpp
 *  MonkeyWorksCore
 *
 *  Created by bkennedy on 8/26/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */
#include "MonkeyWorksCore/Experiment.h"
#include "MonkeyWorksCore/ComponentRegistry_new.h"
#include "Jim1Factory.h"
#include "Jim1.h"
#include "Jim1Stimulus.h"

// This is where the fields for the MonkeyWorks object are checked for errors, are parsed from text into whatever format the 
// constructor requires and the constructor is called and the result passed back to whatever called "createObject()".  
// createObject() is called from the parser.

// The arguement "parameters" contains a list of atributes and values.  For example, a variable defined as:
// <variable tag="var_name" default_value="3.14159" scope="global" persistant="YES"/>
// would generate a std::map of:
// "tag" .... "var_name"
// "default_value" .... "3.14159"
// "scope" .... "global"
// "persistant" .... "YES"

// it's up to the developer to parse these into something useful
// There are a few macros to make this clearer.
// REQUIRE_ATTRIBUTES(parameters, "attribute_name1", "attribute_name2", ...)
// checks the parameter list to make sure there is a attribute of each name listed.  An exception is thrown if one is missing.

// reg->getVariable(std::string variable_value) will return a shared_ptr to a variable passed to the constructor.  The arguement 
// "variable_value" can be a number ("45", "-1.6934"), a variable name ("#task_mode", "variable_name"), or an 
// expression ("14+some_variable_name", "rand(0,4)").

// reg->getStimulus(std::string stimulus_name) will return a shared_ptr to a stimulus node

// if any of the previous calls fail for any reason, they throw an exception, which is eventually caught by the parser and 
// the error reported to the console.  If any errors occur here, an exception shoudl be thrown.  There's an exception called 
// mw::SimpleException(const char *text) that can be used for this.  
boost::shared_ptr<mw::Component> mw::Jim1PluginFactory::createObject(std::map<std::string, std::string> parameters,
																	  mw::mwComponentRegistry *reg) {
	REQUIRE_ATTRIBUTES(parameters, 
					   "tag", "x_position", "y_position");
	
	std::string tagname(parameters.find("tag")->second);
	shared_ptr<Variable> x_position = reg->getVariable(parameters.find("x_position")->second);	
	shared_ptr<Variable> y_position = reg->getVariable(parameters.find("y_position")->second);	

    shared_ptr<Variable> alpha_multiplier = 
	reg->getVariable(parameters["alpha_multiplier"], "1");
    
    // if anything left on the list, that mean there is XML info that this plugin that is not needed -- warn the user
    // for now, just check size
    mw::mprintf("In Jim1Plugin Factory:  size of parameter map is %d", parameters.size() );
    mw::mwarning(M_PARSER_MESSAGE_DOMAIN,"Some XML info ignored by Jim1Plugin because not needed or used.");
    
    if(mw::GlobalCurrentExperiment == 0) {
		throw mw::SimpleException("no experiment currently defined");		
	}
	
	shared_ptr<StimulusDisplay> defaultDisplay = mw::GlobalCurrentExperiment->getStimulusDisplay();
	if(defaultDisplay == 0) {
		throw mw::SimpleException("no stimulusDisplay in current experiment");
	}
	
    
     // rgb should be 0-1    (1 is full saturation)
	
	shared_ptr <mJim1Stimulus> new_Jim1_stimulus = shared_ptr<mJim1Stimulus>(new mJim1Stimulus(tagname, 
                                                                                               x_position,
                                                                                               y_position));
                                                                          
	new_Jim1_stimulus->load(defaultDisplay.get());
	shared_ptr <StimulusNode> thisStimNode = shared_ptr<StimulusNode>(new StimulusNode(new_Jim1_stimulus));
	reg->registerStimulusNode(tagname, thisStimNode);
	
	return new_Jim1_stimulus;
    
    
	//boost::shared_ptr <mw::Jim1> new_shell_ = boost::shared_ptr<mw::Jim1>(new mw::Jim1(tagname));	
	//return new_shell_;
    
}

