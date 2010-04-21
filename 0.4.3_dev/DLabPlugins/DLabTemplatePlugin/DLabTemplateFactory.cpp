/*
 *  DLabTemplateFactory.cpp
 *  MonkeyWorksCore
 *
 *  Created by bkennedy on 8/26/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#include "MonkeyWorksCore/ComponentRegistry_new.h"
#include "DLabTemplateFactory.h"
#include "DLabTemplate.h"


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
// the error reported to the console.  If any errors occur here, an exception should be thrown.  There's an exception called 
// mw::SimpleException(const char *text) that can be used for this.  
boost::shared_ptr<mw::Component> mw::DLabTemplatePluginFactory::createObject(std::map<std::string, std::string> parameters,
																	  mw::mwComponentRegistry *reg) {
	
    // this specifies the list of XML fields that are required to run the constructor on this component
    // There is an XML file in  the plugin that tell the editor how to build the XML (these should match)
    //  (see the Circle stimulus for example).
    REQUIRE_ATTRIBUTES(parameters, 
					   "tag");
	
	std::string tagname(parameters.find("tag")->second);
	
	boost::shared_ptr <mw::DLabTemplate> new_shell_ = boost::shared_ptr<mw::DLabTemplate>(new mw::DLabTemplate(tagname));
	
	return new_shell_;
}

