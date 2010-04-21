/*
 *  ITC18_IODeviceFactory.cpp
 *  ITC18Plugin
 *
 *  Created by labuser on 8/18/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#include "ITC18_IODeviceFactory.h"
#include "ITC18_IODevice.h"
using namespace mw;


boost::shared_ptr<mw::Component> mITC18_IODeviceFactory::createObject(std::map<std::string, std::string> parameters,
																	  mwComponentRegistry *reg) {
	
	shared_ptr<Scheduler> scheduler = Scheduler::instance();
	boost::shared_ptr <mw::Component> new_itc18 = boost::shared_ptr<mw::Component>(new mITC18_IODevice(scheduler));
	return new_itc18;
}

shared_ptr<mw::Component> ITC18_TriggeredAnalogSnippetChannelRequestFactory::createObject(std::map<std::string, std::string> parameters,
																	  mwComponentRegistry *reg) {
	REQUIRE_ATTRIBUTES(parameters,
					   "capability",
					   "data_interval",
					   "direction",
					   "range_min",
					   "range_max",
					   "resolution",
					   "synchrony",
					   "data_type",
					   "update_interval",
					   "variable",
					   "TTL_trigger_port",
					   "pre_trigger_interval",
					   "post_trigger_interval");
	
	string tag;
	
	if(parameters.find("tag") != parameters.end()) {
		tag = parameters.find("tag")->second;
	}
	
	string capability = parameters.find("capability")->second;                                                 
	
	Data data_interval(reg->getNumber(parameters.find("data_interval")->second));
	Data update_interval(reg->getNumber(parameters.find("update_interval")->second));
	
	string direction_string = parameters.find("direction")->second;
	direction_string = to_lower_copy(direction_string);                                       
	IODataDirection data_direction = M_INPUT_DATA;
	
	string synchrony_string = parameters.find("synchrony")->second;
	synchrony_string = to_lower_copy(synchrony_string);
	IODataSynchronyType synchrony_type = M_HARDWARE_TIMED_SYNCHRONOUS_IO;
	
	Data range_min(reg->getNumber(parameters.find("range_min")->second));
	Data range_max(reg->getNumber(parameters.find("range_max")->second)); 
	Data resolution(reg->getNumber(parameters.find("resolution")->second));  
	
	shared_ptr<Variable> variable = reg->getVariable(parameters.find("variable")->second);
	
	string type_string = parameters.find("data_type")->second;
	type_string = to_lower_copy(type_string);
	IODataType data_type =M_ANALOG_SNIPPET_DATA;
	
	Data ttl_linked_port(reg->getNumber(parameters.find("TTL_trigger_port")->second));
	Data pre_window(reg->getNumber(parameters.find("pre_trigger_interval")->second)); 
	Data post_window(reg->getNumber(parameters.find("post_trigger_interval")->second));  
	
	//checkAttribute(capability, parameters.find("reference_id")->second,	"watch", parameters.find("capability")->second);                                                        
	
	shared_ptr<mw::Component> newIOChannel(new IOChannelRequest_TriggeredAnalogSnippetITC18(tag,
																	  variable,
																	  capability,
																	  data_direction,
																	  data_type,
																	  synchrony_type,
																	  data_interval.getInteger(),
																	  update_interval.getInteger(),
																	  range_min.getFloat(),
																	  range_max.getFloat(),
																	  resolution.getInteger(),
																	  pre_window.getInteger(),
																	  post_window.getInteger(),
																	  ttl_linked_port.getInteger()));
	
	
	return newIOChannel;
}

