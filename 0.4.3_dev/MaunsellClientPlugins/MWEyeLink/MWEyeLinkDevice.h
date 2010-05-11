/*
 *  MWEyeLinkDevice.h
 *  MWEyeLinkPlugin
 *
 *  Created by Jon Hendry on 1/1/10.
 *  Copyright 2010 hhmi. All rights reserved.
 *
 */

#ifndef	_MWEYELINK_DEVICE_H_
#define _MWEYELINK_DEVICE_H_

#include "MonkeyWorksCore/Utilities.h"
#include "MonkeyWorksCore/Plugin.h"
#include "MonkeyWorksCore/IODevice.h"
#include "MonkeyWorksCore/ComponentFactory.h"
#include "MonkeyWorksCore/ComponentRegistry_new.h"

#undef VERBOSE_IO_DEVICE
#define VERBOSE_IO_DEVICE 1 


using namespace std;

namespace mw {
	
	class MWEyeLinkDeviceOutputNotification;
	
	class MWEyeLinkDevice : public IODevice {
		
	protected:
		int								eye_used;
		MonkeyWorksTime					nextSampleTimeS;
		MonkeyWorksTime					EyeLinkSamplePeriodS;
		MonkeyWorksTime					sampleTimeS;
		bool							justStartedEyeLink;
		
		
		bool							connected;
		
		boost::shared_ptr <Scheduler>	scheduler;
		
		shared_ptr<ScheduleTask>		pollScheduleNode;
		
		boost::mutex					pollScheduleNodeLock;	
		
		boost::mutex					dataLock;				
		boost::mutex					MWEyeLinkDriverLock;	// FIXME use boost lock per Mark's LabJack driver	
		
		boost::shared_ptr <Variable> pupilX;
		boost::shared_ptr <Variable> pupilY;
		boost::shared_ptr <Variable> pupilArea;
		
		MonkeyWorksTime update_period;
		
		bool active;
		boost::mutex active_mutex;
		bool deviceIOrunning;
		
		
	public:
		
		
		
		MWEyeLinkDevice(const boost::shared_ptr <Scheduler> &a_scheduler,
						const boost::shared_ptr <Variable> _pupilX,
						const boost::shared_ptr <Variable> _pupilY,
						const boost::shared_ptr <Variable> _pupilArea,
						const MonkeyWorksTime update_time);
		~MWEyeLinkDevice();
		MWEyeLinkDevice(const MWEyeLinkDevice& copy);
		
		virtual bool startup();
		virtual bool updateEyePosition();
		virtual bool shutdown();	
		virtual bool attachPhysicalDevice();
		virtual void detachPhysicalDevice();
        virtual bool startDeviceIO();		
		virtual bool stopDeviceIO();		
		

		
	


		virtual void setActive(bool _active){
			boost::mutex::scoped_lock active_lock(active_mutex);
			active = _active;
		}
		
		virtual bool getActive(){
			boost::mutex::scoped_lock active_lock(active_mutex);
			bool is_active = active;
			return is_active;
		}
		
		shared_ptr<MWEyeLinkDevice> shared_from_this() { return static_pointer_cast<MWEyeLinkDevice>(IODevice::shared_from_this()); }
		
	};
	
	
	class MWEyeLinkDeviceFactory : public ComponentFactory {
		
		shared_ptr<Component> createObject(std::map<std::string, std::string> parameters,
										   mwComponentRegistry *reg);
	};
	
	
	class MWEyeLinkDeviceOutputNotification : public VariableNotification {
		
	protected:
		
		weak_ptr<MWEyeLinkDevice> elink;
		
		
	public:
		
		MWEyeLinkDeviceOutputNotification(weak_ptr<MWEyeLinkDevice> _elink){
			elink = _elink;
		}
		
		virtual void notify(const Data& data, MonkeyWorksTime timeUS){
			
			shared_ptr<MWEyeLinkDevice> shared_elink(elink);
			//shared_elink->dispense(data);
			
		}
	};
	
} // namespace mw

#endif






