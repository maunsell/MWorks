/*
 *  NidaqDevice.h
 *  NidaqPlugin
 *
 *  Created by Jon Hendry on 1/1/10.
 *  Copyright 2010 hhmi. All rights reserved.
 *
 */

#ifndef	_NIDAQ_DEVICE_H_
#define _NIDAQ_DEVICE_H_

#include "NIDAQmxBase.h"
#include "MonkeyWorksCore/Utilities.h"
#include "MonkeyWorksCore/Plugin.h"
#include "MonkeyWorksCore/IODevice.h"
#undef VERBOSE_IO_DEVICE
#define VERBOSE_IO_DEVICE 2 
#include "MonkeyWorksCore/ComponentFactory.h"
#include "MonkeyWorksCore/ComponentRegistry_new.h"

using namespace std;

namespace mw {

class NidaqDeviceOutputNotification;

class NidaqDevice : public IODevice {

	protected:
		bool						connected;
		MonkeyWorksTime				lastDITransitionTimeUS;
		boost::shared_ptr <Scheduler> scheduler;
		shared_ptr<ScheduleTask>	pulseScheduleNode;
		shared_ptr<ScheduleTask>	pollScheduleNode;
		boost::mutex				pulseScheduleNodeLock;				
		boost::mutex				pollScheduleNodeLock;				
		static Lockable				*NidaqDriverLock;		
		
		boost::shared_ptr <Variable> pulseDurationMS;
		boost::shared_ptr <Variable> pulseOn;
		boost::shared_ptr <Variable> leverPress;
		MonkeyWorksTime update_period;
		
		bool active;
		bool lastLeverPressValue;	
		boost::mutex active_mutex;
		bool deviceIOrunning;
		
		TaskHandle ctr0Task;
		TaskHandle ctr1Task;
		TaskHandle diTask;
		TaskHandle doTask;
		
		int32 niError;
				
		void createDigitalInTask();
		void createDigitalInChannel();
		void startDigitalInTask();
		void stopDigitalInTask();
		
		void createDigitalOutTask();
		void createDigitalOutChannel();
		void startDigitalOutTask();
		void stopDigitalOutTask();

		void startIO();
		void stopIO();
		void pulseDO();
	

	public:
	
		NidaqDevice(const boost::shared_ptr <Scheduler> &a_scheduler,
					const boost::shared_ptr <Variable> _pulseDurationMS,
					const boost::shared_ptr <Variable> _pulseOn,
					const boost::shared_ptr <Variable> _leverPress,
					const MonkeyWorksTime update_time);
		~NidaqDevice();

		virtual bool startup();
		virtual bool updateSwitch();
		virtual bool shutdown();	
		virtual bool attachPhysicalDevice();
		virtual bool startDeviceIO();		
		virtual bool stopDeviceIO();		
	
		virtual void connectToDevice();
		virtual void disconnectFromDevice();
		
		virtual void reconnectToDevice(){
			disconnectFromDevice();
			connectToDevice();
		}
		bool readDI();
		void pulseDOHigh(int pulseLengthUS);
		void pulseDOLow(int pulseLengthUS);		
		virtual void dispense(Data data){
			
			if(getActive()){
				bool doReward = (bool)data;
                
				// Bring DO high for pulseDurationMS ms.
				
				
				if (doReward) {
					//bring DO high
					//mprintf("Yum juice!");
					this->pulseDOHigh(pulseDurationMS->getValue());
				}
			}
		}
		virtual void setActive(bool _active){
			boost::mutex::scoped_lock active_lock(active_mutex);
			active = _active;
		}

		virtual bool getActive(){
			boost::mutex::scoped_lock active_lock(active_mutex);
			bool is_active = active;
			return is_active;
		}

		shared_ptr<NidaqDevice> shared_from_this() { return static_pointer_cast<NidaqDevice>(IODevice::shared_from_this()); }

};


class NidaqDeviceFactory : public ComponentFactory {

	shared_ptr<Component> createObject(std::map<std::string, std::string> parameters,
													 mwComponentRegistry *reg);
};


class NidaqDeviceOutputNotification : public VariableNotification {

	protected:
	
		weak_ptr<NidaqDevice> daq;
	
	
	public:
	
		NidaqDeviceOutputNotification(weak_ptr<NidaqDevice> _daq){
			daq = _daq;
		}
	
		virtual void notify(const Data& data, MonkeyWorksTime timeUS){

			shared_ptr<NidaqDevice> shared_daq(daq);
			shared_daq->dispense(data);
			
		}
};

} // namespace mw

#endif






