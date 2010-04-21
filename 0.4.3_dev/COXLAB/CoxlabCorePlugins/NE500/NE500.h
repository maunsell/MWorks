/*
 *  NE500.h
 *  MonkeyWorksCore
 *
 *  Created by David Cox on 2/1/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef	_NE500_H_
#define _NE500_H_

#include <string>
#include <boost/format.hpp>
#include "MonkeyWorksCore/Utilities.h"
#include "MonkeyWorksCore/Plugin.h"
#include "MonkeyWorksCore/IODevice.h"
#include "MonkeyWorksCore/ComponentFactory.h"




using namespace std;




class mNE500DeviceOutputNotification;

class mNE500DeviceChannel : public mComponent {

	protected:
	
		string pump_id;
		shared_ptr<mVariable> variable;
		
		double syringe_diameter;
		shared_ptr<mVariable> rate;
		
	public:
	
		mNE500DeviceChannel(string _pump_id, 
							shared_ptr<mVariable> _variable,
							double _syringe_diameter,
							shared_ptr<mVariable> _rate){
			
			pump_id = _pump_id;
			variable = _variable;
			syringe_diameter = _syringe_diameter;
			rate = _rate;
			
			if(rate == NULL){
				throw mSimpleException("Invalid rate variable in NE500 pump channel");
			}
		}
		
		
		shared_ptr<mVariable> getVariable(){
			return variable;
		}
		
		string getPumpID(){
			return pump_id;
		}
		
		double getSyringeDiameter(){ return syringe_diameter; }
		double getRate(){ 
			return (double)(rate->getValue()); 
		}
		

};

class mNE500DeviceChannelFactory : public mComponentFactory {

	shared_ptr<mComponent> createObject(std::map<std::string, std::string> parameters,
													 mComponentRegistry *reg) {
		
		REQUIRE_ATTRIBUTES(parameters, "variable", "capability", "syringe_diameter", "flow_rate");
		
		string capability_string = to_lower_copy(parameters["capability"]);
		
		shared_ptr<mVariable> variable = reg->getVariable(parameters["variable"]);
		
		double syringe_diameter = reg->getNumber(parameters["syringe_diameter"]);
		shared_ptr<mVariable> rate = reg->getVariable(parameters["flow_rate"]);
		
		shared_ptr <mComponent> new_channel(new mNE500DeviceChannel(capability_string, variable, syringe_diameter, rate));
		return new_channel;
	}

};


class mNE500PumpNetworkDevice : public mIODevice {

	protected:
	
		string address;
		int port;
		
		// the socket
		int s;
		
		bool connected;
		
		vector<string> pump_ids;
		
		bool active;
		boost::mutex active_mutex;	

	public:
	
		mNE500PumpNetworkDevice(string _address, int _port){
		
			address = _address;
			port = _port;
			
			// TODO: connect to serial server
			connectToDevice();
		}
		
		virtual ~mNE500PumpNetworkDevice(){ 
			disconnectFromDevice();
		}
		
						
		virtual void connectToDevice();
		
		virtual void disconnectFromDevice();
		
		virtual void reconnectToDevice(){
			disconnectFromDevice();
			connectToDevice();
		}
		
		string sendMessage(string message);
		
		virtual void dispense(string pump_id, double rate, mData data){
			
			if(getActive()){
				//initializePump(pump_id, 750.0, 20.0);
			
				if((double)data == 0.0){
					return;
				}

                if(data.getFloat() >= 0){
                    sendMessage("DIR INF"); // infuse
                } else {
                    sendMessage("DIR WDR"); // withdraw
                }

				boost::format rate_message_format("%s RAT %.3f"); 
				string rate_message = (rate_message_format % pump_id % rate).str();
				
				sendMessage(rate_message);
						
				boost::format message_format("%s VOL %.3f"); 
				string message = (message_format % pump_id % data.getFloat()).str();
				
				sendMessage(message);
				
				boost::format program_message_format("%s FUN RAT");
				string program_message = (program_message_format % pump_id).str();
				
				sendMessage(program_message);
				
				boost::format run_message_format("%s RUN"); 
				string run_message = (run_message_format % pump_id).str();
				
				sendMessage(run_message);
			}
		}
	
		virtual void initializePump(string pump_id, double rate, double syringe_diameter){
			
			boost::format function_message_format("%s FUN RAT"); 
			string function_message = (function_message_format % pump_id).str();
			
			sendMessage(function_message);
			
			
			boost::format diameter_message_format("%s DIA %g"); 
			string diameter_message = (diameter_message_format % pump_id % syringe_diameter).str();
			
			sendMessage(diameter_message);
			
			
			boost::format rate_message_format("%s RAT %g"); 
			string rate_message = (rate_message_format % pump_id % rate).str();
			
			sendMessage(rate_message);
		}
		
		// specify what this device can do
		virtual mExpandableList<mIOCapability> *getCapabilities(){ return NULL; }
		virtual bool mapRequestsToChannels(){  return true;  }
		virtual bool initializeChannels(){  return true;  }
		virtual bool startup(){  return true;  }
		virtual bool attachPhysicalDevice(){ return connected; }

		virtual void addChild(std::map<std::string, std::string> parameters,
								mComponentRegistry *reg,
								shared_ptr<mComponent> _child);
		
		virtual void setActive(bool _active){
			boost::mutex::scoped_lock active_lock(active_mutex);
			active = _active;
		}

		virtual bool getActive(){
			boost::mutex::scoped_lock active_lock(active_mutex);
			bool is_active = active;
			return is_active;
		}



		virtual bool startDeviceIO(){  setActive(true); return true; }
		virtual bool stopDeviceIO(){  setActive(false); return true; }
		
		
		// this will stop anyIO behavior on a device and put the device in a shutdown state (if the device has one) -- e.g. turn off x-ray
		virtual bool shutdown(){ return true; }


};


class mNE500DeviceFactory : public mComponentFactory {

	shared_ptr<mComponent> createObject(std::map<std::string, std::string> parameters,
													 mComponentRegistry *reg);
};


class mNE500DeviceOutputNotification : public mVariableNotification {

	protected:
	
		weak_ptr<mNE500PumpNetworkDevice> pump_network;
		weak_ptr<mNE500DeviceChannel> channel;
		//string pump_id;
	
	
	public:
	
		mNE500DeviceOutputNotification(weak_ptr<mNE500PumpNetworkDevice> _pump_network,
									   weak_ptr<mNE500DeviceChannel> _channel){
			pump_network = _pump_network;
			channel = _channel;

		}
	
		virtual void notify(const mData& data, MonkeyWorksTime timeUS){

			shared_ptr<mNE500PumpNetworkDevice> shared_pump_network(pump_network);
			shared_ptr<mNE500DeviceChannel> shared_channel(channel);
			shared_pump_network->dispense(shared_channel->getPumpID(), shared_channel->getRate(), data);
			
		}
};


#endif






