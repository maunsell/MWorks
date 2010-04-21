/*
 *  Phidgets.h
 *  MonkeyWorksCore
 *
 *  Created by David Cox on 1/28/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <Phidget21/phidget21.h>
#include <string>
#include <boost/format.hpp>
#include "MonkeyWorksCore/Utilities.h"
#include "MonkeyWorksCore/Plugin.h"
#include "MonkeyWorksCore/IODevice.h"


using namespace std;



int AttachHandler(CPhidgetHandle IFK, void *userptr);
int DetachHandler(CPhidgetHandle IFK, void *userptr);
int ErrorHandler(CPhidgetHandle IFK, void *userptr, int ErrorCode, const char *unknown);
int InputChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int State);
int OutputChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int State);
int SensorChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int Value);



enum mPhidgetChannelType{  M_PHIDGET_DIGITAL_INPUT, M_PHIDGET_DIGITAL_OUTPUT, M_PHIDGET_ANALOG_INPUT };

class mPhidgetDeviceChannel : public mComponent {

	protected:
	
		mPhidgetChannelType channel_type;
		int index;
		
		CPhidgetInterfaceKitHandle ifKit;
		
		shared_ptr<mVariable> variable;
		
	public:
	
		mPhidgetDeviceChannel(mPhidgetChannelType _type, int _index, shared_ptr<mVariable> _variable){
			channel_type = _type;
			index = _index;
			
			ifKit = NULL;
			
			variable = _variable;
		}
		
		
		void setDevice(CPhidgetInterfaceKitHandle _ifKit){
			ifKit = _ifKit;
		}
		
		CPhidgetInterfaceKitHandle getDevice(){
			return ifKit;
		}
		
		shared_ptr<mVariable> getVariable(){
			return variable;
		}
		
		int getIndex(){
			return index;
		}
		
		mPhidgetChannelType getType(){
			return channel_type;
		}
		
};

class mPhidgetDeviceChannelFactory : public mComponentFactory {

	shared_ptr<mComponent> createObject(std::map<std::string, std::string> parameters,
													 mComponentRegistry *reg) {
		
		REQUIRE_ATTRIBUTES(parameters, "variable", "capability", "index");
		
		string capability_string = to_lower_copy(parameters["capability"]);
		mPhidgetChannelType type;
		int index = reg->getNumber(parameters["index"]);
		
		if(capability_string == "digital_input"){
			type = M_PHIDGET_DIGITAL_INPUT;
		} else if(capability_string == "digital_output"){
			type = M_PHIDGET_DIGITAL_OUTPUT;
		} else if(capability_string == "analog_input"){
			type = M_PHIDGET_ANALOG_INPUT;
		} else {
			throw mSimpleException("Unknown phidget channel type", capability_string);
		}
		
		shared_ptr<mVariable> variable = reg->getVariable(parameters["variable"]);
		
		shared_ptr <mComponent> new_channel(new mPhidgetDeviceChannel(type, index, variable));
		return new_channel;
	}

};


class mPhidgetDevice : public mIODevice {

	protected:
	
		CPhidgetInterfaceKitHandle ifKit;

		bool active;
		boost::mutex active_mutex;	

		map< int, weak_ptr<mPhidgetDeviceChannel> > analog_input_channels;
		map< int, weak_ptr<mPhidgetDeviceChannel> > digital_input_channels;
		map< int, weak_ptr<mPhidgetDeviceChannel> > digital_output_channels;

	public:
	
		mPhidgetDevice(){
		
			active = false;
		
			//Declare an InterfaceKit handle
			ifKit = 0;

			setActive(false);
			//create the InterfaceKit object
			CPhidgetInterfaceKit_create(&ifKit);

			//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
			CPhidget_set_OnAttach_Handler((CPhidgetHandle)ifKit, AttachHandler, NULL);
			CPhidget_set_OnDetach_Handler((CPhidgetHandle)ifKit, DetachHandler, NULL);
			CPhidget_set_OnError_Handler((CPhidgetHandle)ifKit, ErrorHandler, NULL);

			//Registers a callback that will run if an input changes.
			//Requires the handle for the Phidget, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
			CPhidgetInterfaceKit_set_OnInputChange_Handler (ifKit, InputChangeHandler, this);

			//Registers a callback that will run if the sensor value changes by more than the OnSensorChange trig-ger.
			//Requires the handle for the IntefaceKit, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
			CPhidgetInterfaceKit_set_OnSensorChange_Handler (ifKit, SensorChangeHandler, this);

			//Registers a callback that will run if an output changes.
			//Requires the handle for the Phidget, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
			//CPhidgetInterfaceKit_set_OnOutputChange_Handler (ifKit, OutputChangeHandler, this);

			//open the interfacekit for device connections
			CPhidget_open((CPhidgetHandle)ifKit, -1);

			//get the program to wait for an accelerometer device to be attached
			mprintf("Waiting for interface kit to be attached....");
			int result;
			const char *err;
			if((result = CPhidget_waitForAttachment((CPhidgetHandle)ifKit, 5000)))
			{
				CPhidget_getErrorDescription(result, &err);
				throw mSimpleException("Problem waiting for attachment", err);
			}
		
			
			
		}
		
		virtual ~mPhidgetDevice();
		
						
		shared_ptr<mPhidgetDeviceChannel> getAnalogInputChannel(int i){
			weak_ptr<mPhidgetDeviceChannel> candidate = analog_input_channels[i];
			if(!candidate.expired()){
				shared_ptr<mPhidgetDeviceChannel> channel = candidate.lock();
				return channel;
			}
			
			return shared_ptr<mPhidgetDeviceChannel>();
		}
		
		shared_ptr<mPhidgetDeviceChannel> getDigitalInputChannel(int i){
			weak_ptr<mPhidgetDeviceChannel> candidate = digital_input_channels[i];
			if(!candidate.expired()){
				shared_ptr<mPhidgetDeviceChannel> channel = candidate.lock();
				return channel;
			}
			
			return shared_ptr<mPhidgetDeviceChannel>();
		}
		
		
		// specify what this device can do
		virtual bool attachPhysicalDevice(){ return true; }
		virtual mExpandableList<mIOCapability> *getCapabilities(){ return NULL; }
		virtual bool mapRequestsToChannels(){  return true;  }
		virtual bool initializeChannels(){  return true;  }
		virtual bool startup(){  
			return true;  
		}

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



		virtual bool startDeviceIO();
		virtual bool stopDeviceIO();
		
		
		// this will stop anyIO behavior on a device and put the device in a shutdown state (if the device has one) -- e.g. turn off x-ray
		virtual bool shutdown(){ return true; }


};


class mPhidgetDeviceFactory : public mComponentFactory {

	shared_ptr<mComponent> createObject(std::map<std::string, std::string> parameters,
													 mComponentRegistry *reg);
};



class mPhidgetDeviceOutputNotification : public mVariableNotification {

	protected:
	
		
		CPhidgetInterfaceKitHandle ifk;
		int index;

	public:
	
		mPhidgetDeviceOutputNotification(shared_ptr<mPhidgetDeviceChannel> channel){
			index = channel->getIndex();
			ifk = channel->getDevice();

		}
	
		virtual void notify(const mData& data, MonkeyWorksTime timeUS){
			CPhidgetInterfaceKit_setOutputState(ifk, index, (int)(data.getBool())); 
		}
};








