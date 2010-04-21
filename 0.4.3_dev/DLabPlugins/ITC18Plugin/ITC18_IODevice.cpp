/**
 * ITC18_IODevice.cpp
 *
 * Desscription:
 *
 * History:
 * James DiCarlo created June 2005
 *
 * Copyright (c) 2002 MIT. All rights reserved.
 */
//#ifdef __ppc__

#include "ITC18_IODevice.h"
#include "boost/bind.hpp"
//#include "boost/format.hpp"

using namespace mw;

/*
 
 // what the core might do to setup any device (including the ITC) =========================================	
 bool setupMyITC() {
 
 theITC18device = new mITC18_IODevice();			// will actually open the only ITC (if there is one)
 theITC18device->attachPhysicalDevice();			// will attach the only ITC (and update to object to have its capabilities)
 
 IOChannelRequest * channelRequest;
 
 
 #define MY_EYE_SAMPLE_INTERVAL_USEC  1000
 #define MY_EYE_UPDATE_INTERVAL_USEC  1000
 
 
 // request a specific channel for the horizontal eye position
 channelRequest = new IOChannelRequest(pEyeXparam, 'ITC18_AD0',  
 M_INPUT_DATA, M_ANALOG_DATA,  MY_EYE_SAMPLE_INTERVAL_USEC, MY_EYE_UPDATE_INTERVAL_USEC);
 theITC18device->requestChannel(channelRequest);	// add to list of requests
 
 // request a specific channel for the vertical eye position
 channelRequest = new IOChannelRequest(pEyeYparam, 'ITC18_AD1',  
 M_INPUT_DATA, M_ANALOG_DATA,  MY_EYE_SAMPLE_INTERVAL_USEC, MY_EYE_UPDATE_INTERVAL_USEC);
 theITC18device->requestChannel(channelRequest);	// add to list of requests
 
 
 // try to match requests to capabilities	(assumes requests and capabilities have both been set)
 // THIS IS WHEN EACH CHANNEL OBJECT IS CREATED (A COMBINATION OF THE REQUEST AND THE CAPABILITY)
 // (EVERY TIME THIS IS RUN, ALL THE EXITING CHANNELS ARE DISCARDED FIRST)
 if (!theITC18device->mapRequestsToChannels()) {		// will create the channel objects (one for each match)
 mprintf('one or more ITC18 requests could not be matched')
 return false; 
 }
 
 // setup the channels on the hardware and make sure that they are "active"  (assumes channel objects have already been established)
 // for the ITC, this is where the instruction sequence gets created and loaded (and ITC software buffers are initialized)
 if (!theITC18device->initializeChannels()) {
 mprintf('ITC18 ' %s ' could not be initialized' ( (theITC18device->getAttachedDevice)->getName() ) );
 return false;
 }
 
 theITC18device->startup();			// does nothing for the ITC class, but good form
 
 return true;
 }
 
 
 // start collection of data (events to the event stream)
 theITC18device->startDeviceIO();		// this basically gets the checking for data on the device scheduled
 /
 // run your trials, etc.
 //
 // pause collection of data
 theITC18device->stopDeviceIO();
 
 // restart collection of data
 theITC18device->startDeviceIO();
 // pause collection of data
 theITC18device->stopDeviceIO();
 
 // finish up for good
 theITC18device->shutdown();		// does nothing
 delete theITC18device;				
 
 // ===================================================================================== 
 
 */ 

// debug only 
//MonkeyWorksTime        timeITCwasOpenedUS = 0;
//ofstream outDataFile; 


// TODO -- all of this should be put in a singleton class.

// Keep track of physical ITC18's in the world.  No matter how many ITC18_device objects
//   are created, there is only one copy of each of these vars
ExpandableList<IOPhysicalDeviceReference> *mITC18_IODevice::visible_ITC18_devices;
bool mITC18_IODevice::visible_ITC18_devices_initialized;
long mITC18_IODevice::numITC18objects;	// number of device objects

// JJD added this lockable Sept 30, 2006
// it is used here to make the ITC18 driver thread-safe from the outside
//  (That is, no matter how many itc18 devices exist, or how many threads call ITC18 methods,
//  only one call to the ITC18 driver can be made at any moment in time.)
Lockable *mITC18_IODevice::ITC18DriverLock;


// ---------------------------------


// constructor function for ITC18_IODevice objects
mITC18_IODevice::mITC18_IODevice(const shared_ptr<Scheduler> &a_scheduler)  : IODevice() {
	
	scheduler = a_scheduler;
	// set up some defaults
	itc = NULL;
	attachedDeviceElementNumber = -1;	// no physical ITC device yet
	fullyInitialized = false;
	deviceIOrunning = false;
	instruction_seq_running = false;
	time_since_inst_seq_started_US = 0;
	num_FIFO_clears_since_itc_started = 0;
	asychLinesStatus = 0;
	capabilitiesSet = false;
	setupSize = 0;
	pSamples = NULL;		// Don't leave these in an indeterminate state!
	pInstructions = NULL;
	//userSetupDone = false;
	numInputAD = 0;
	numInputDig = 0;
	fast_time_slice_us = 0;
	full_time_slice_us = 0;
	AllChannelRequestsMetWithinSamplingTolerance = false;
	
	clockDriftAdjustFactor = 1.0000;
	fudgeTime = 0;
	instructionSequenceIsNeeded = false;
	flushFIFOintervalUS = 10000;   // will be overriden.  Just a safety
	
	temporaryIncompatabilityList = new ExpandableList<IOChannelIncompatibility>();
	
	// 
	//itc = new char[ITC18_GetStructureSize()];		// structure to hold info about ITC (used as the handle)
	//if (itc == NULL) {
	//	//mprintf("%s %s -- %s", __FILE__, __LINE__, sMemError);
	//    merror("itc data structure was not created.");
	//}
	
	setup = NULL;   
    
	// the first time you create an object of this class, go out and find all the
	//  available ITC physical devices (you only need to do this once, 
	// so we use static vars to hold the pointers and the boolean to tell us it has been done)
	if (!mITC18_IODevice::visible_ITC18_devices_initialized) {	
		numITC18objects = 0;	
		visible_ITC18_devices = new ExpandableList<IOPhysicalDeviceReference>();
		ITC18DriverLock = new Lockable();
		
		// TODO -- needs updating ------------------------------------------
		// go find all physical ITC18 devices ...
		// add each to the list ...
		
		// for now, just assume there is one device and add it to the list
		// for now, look for an itc.  If you find one, keep the handle, and keep it open (we will not open it again)
		int i = 0;		// should loop and check for available hardware here
		
		void *itcStructTemp = openITC18();  // try to open the ITC and initialize
		
		if (itcStructTemp != NULL) {	
			//itcStructList[i] = itcStructTemp;		// this is our handle to the physical device
			itc = itcStructTemp;  // temp kludge for now 
			shared_ptr<IOPhysicalDeviceReference> physical_device_ref(
																	   new IOPhysicalDeviceReference(i, "ITC18_Phys_Device_0"));      
			visible_ITC18_devices->addElement(physical_device_ref);
			
		} else {    // ITC device could not be openned.  
			mwarning(M_IODEVICE_MESSAGE_DOMAIN, "ITC18 device requested for experiment, but could not be opened.");
			mwarning(M_IODEVICE_MESSAGE_DOMAIN, "Using 'alt' tag instead.");
		}
		
		// ------------------------------------------------------------
		
		// 
		visible_ITC18_devices_initialized =true;    
	}
	else {
		numITC18objects++;
	}
	
	getCapabilities();      // find the capabilities of this type of device
	
}


// destructor function for ITC18_IODevice objects
mITC18_IODevice::~mITC18_IODevice() {
	
	// make sure the ITC is shutdown
	closeITC18();
	
	// delete the itc structure from memory
	free(itc);										
	itc = NULL;	
	
	// delete the setup buffer and instruction buffer from memory
	deleteInstructionSeqArrays();
	
	// deletion of the channel objects should destroy all the buffers and notification objects
	
	// free the device on the ITC list in case others want to use it
	if ((attachedDeviceElementNumber>=0) && (attachedDeviceElementNumber<visible_ITC18_devices->getNElements())) {
		(visible_ITC18_devices->getRawElement(attachedDeviceElementNumber))->setAvailable(true);
	}
	
	// if this is the only existing ITC object, delete the visible_ITC18_devices list (free the memory)
	numITC18objects--;
	// if (numITC18objects=0) { // BBK: did you really mean this?  I bet you meant this instead:
	if (numITC18objects==0) {
		visible_ITC18_devices->clear();
		visible_ITC18_devices_initialized = false;
	}
}



// =========================================================================================================================================================
// public ITC18 methods (overrides of base class) ==========================================================================================================


// -------------------------------------------------------------------------------------
// attach the next available ITC18  -- for now, there should be just one -- attach it.
bool mITC18_IODevice::attachPhysicalDevice() {
	
	// just skip the attach if debugging without a physical ITC
#ifdef ITC18_DEBUG	   
	return(true);
#endif
	
	// for now, just take the next avaialble device on the list
	for (int i=0; i<visible_ITC18_devices->getNElements(); i++) { 
		shared_ptr<IOPhysicalDeviceReference> potentialDevice = 
		visible_ITC18_devices->getElement(i);
		if (potentialDevice->isAvailable()) {
			
			// this makes a copy of the device info (base class  model)
			attached_device = potentialDevice.get();
			attached_device->setAvailable(false);	// device now claimed
			attachedDeviceElementNumber = i;		// so we can find it later if we want to release it
			
			// TODO -- what we want to do here for multiple ITC support is to set itc = itcStruct for this device
			// then we should re-open that itc and get all its info
			//itc = itcStructList[i];
			//openITC18(itc)	// open a specific piece of hardware
			
			// get an estiamte of how much this ITC's crystal drifts
			//		relative to the global clock
			clockDriftAdjustFactor = getClockDriftFactor(attached_device);
			
			// make sure all asych out lines are set low
			setAsychLinesLow(0xFFFF);	// all 16 lines set low
			
			return(true);
		}
	}
	
	return false;
	
}


// TODO -- this routine should be more sophisticated
//		(e.g. run the ITC for about 500 msec).
double mITC18_IODevice::getClockDriftFactor(IOPhysicalDeviceReference* deviceRef) {
	
	return(1.00005);	// fudge for now based on testing		(1 part in 20000)
	
}


// --------------------------------------------------------------------------------------------
ExpandableList<IOCapability> *mITC18_IODevice::getCapabilities() {
	
	int  minSamplingIntervalUS;
	
	
	// should set capabilities in here (if they are not already set)
	if (!capabilitiesSet) {
        
		
		// make list of ITC18 capabilities
		minSamplingIntervalUS = (int)(ITC18_MINIMUM_TICKS*ITC18_TICK_DURATION_US);	// (i.e. 5 usec , or 200 kHz) can only achieve this if only one line on the instruction sequence (this is the min duration between instructions)
		
		// *** the last number of the ID is currently the physical port number on the device 
		// synchronous analog inputs
		// Each of these currently supports two user requested data types: 
		//    - standard continuous analog  = ITC_ADC_INPUT_CONTINUOUS_TYPE,	
		//    - waveform (triggered analog continuous for some user-indicated duration) = ITC_ADC_INPUT_WAVEFORM_TYPE,
		for(int i = 0; i < 8; i++){
			
			std::string name = "ITC18_ADC_INPUT" + (boost::format("%d") % i).str();
			IOCapability * capability(
									   new IOCapability(100 + i,
														 name,
														 M_INPUT_DATA,
														 M_DEVICE_SPECIFIC_DATA,
														 M_HARDWARE_TIMED_SYNCHRONOUS_IO,
														 minSamplingIntervalUS,
														 -10, 10, 16));
			registerCapability(capability);
		}
		/*				
		 capability = new IOCapability(100, "ITC18_ADC_INPUT0", M_INPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_HARDWARE_TIMED_SYNCHRONOUS_IO, minSamplingIntervalUS, -10, 10, 16); registerCapability(capability); delete capability;
		 capability = new IOCapability(101, "ITC18_ADC_INPUT1", M_INPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_HARDWARE_TIMED_SYNCHRONOUS_IO, minSamplingIntervalUS, -10, 10, 16); registerCapability(capability); delete capability;
		 capability = new IOCapability(102, "ITC18_ADC_INPUT2", M_INPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_HARDWARE_TIMED_SYNCHRONOUS_IO, minSamplingIntervalUS, -10, 10, 16); registerCapability(capability); delete capability;
		 capability = new IOCapability(103, "ITC18_ADC_INPUT3", M_INPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_HARDWARE_TIMED_SYNCHRONOUS_IO, minSamplingIntervalUS, -10, 10, 16); registerCapability(capability); delete capability;
		 capability = new IOCapability(104, "ITC18_ADC_INPUT4", M_INPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_HARDWARE_TIMED_SYNCHRONOUS_IO, minSamplingIntervalUS, -10, 10, 16); registerCapability(capability); delete capability;
		 capability = new IOCapability(105, "ITC18_ADC_INPUT5", M_INPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_HARDWARE_TIMED_SYNCHRONOUS_IO, minSamplingIntervalUS, -10, 10, 16); registerCapability(capability); delete capability;
		 capability = new IOCapability(106, "ITC18_ADC_INPUT6", M_INPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_HARDWARE_TIMED_SYNCHRONOUS_IO, minSamplingIntervalUS, -10, 10, 16); registerCapability(capability); delete capability;
		 capability = new IOCapability(107, "ITC18_ADC_INPUT7", M_INPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_HARDWARE_TIMED_SYNCHRONOUS_IO, minSamplingIntervalUS, -10, 10, 16); registerCapability(capability); delete capability;
		 */		
		// synchronous digital inputs.  The way these will run is determined by the data type field provided by the user
		// each of these currently support four user requested data types:
		//  - ITC_TTL_INPUT_CONTINUOUS_TYPE,
		//  - ITC_TTL_INPUT_EDGE_LOW_TO_HIGH_TYPE,
		//  - ITC_TTL_INPUT_EDGE_HIGH_TO_LOW_TYPE,
		//  - ITC_TTL_INPUT_EDGE_ANY_TYPE
		
		for(int i = 0; i < 4; i++){
			std::string name = "ITC18_TTL_INPUT" + (boost::format("%d") % i).str();
			IOCapability * capability(
									   new IOCapability(200 + i,
														 name,
														 M_INPUT_DATA,
														 M_DEVICE_SPECIFIC_DATA,
														 M_HARDWARE_TIMED_SYNCHRONOUS_IO,
														 minSamplingIntervalUS,
														 0, 5, 1));
			registerCapability(capability);
		}
		
		/*			capability = new IOCapability(200, "ITC18_TTL_INPUT0", M_INPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_HARDWARE_TIMED_SYNCHRONOUS_IO, minSamplingIntervalUS, 0, 5, 1); registerCapability(capability); delete capability;
		 capability = new IOCapability(201, "ITC18_TTL_INPUT1", M_INPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_HARDWARE_TIMED_SYNCHRONOUS_IO, minSamplingIntervalUS, 0, 5, 1); registerCapability(capability); delete capability;
		 capability = new IOCapability(202, "ITC18_TTL_INPUT2", M_INPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_HARDWARE_TIMED_SYNCHRONOUS_IO, minSamplingIntervalUS, 0, 5, 1); registerCapability(capability); delete capability;
		 capability = new IOCapability(203, "ITC18_TTL_INPUT3", M_INPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_HARDWARE_TIMED_SYNCHRONOUS_IO, minSamplingIntervalUS, 0, 5, 1); registerCapability(capability); delete capability;
		 */			
		
		// digital asych outputs  //TODO -- update min interval that these ports can be toggled (ITC spec??)
		// each of these ports currently supports three types of user requested data type:
		//  - standard asych out (i.e. go high or low at users request -- all timing is determined by user)  ITC_AUXPORT_ASYCH_OUT_TYPE
		//  - pulsed low (go low on user request for requested duration)  ITC_AUXPORT_ASYCH_OUT_PULSE_ACTIVE_LOW
		//  - pulsed high (go high on user request for requested duration) ITC_AUXPORT_ASYCH_OUT_PULSE_ACTIVE_HIGH
		
		for(int i = 0; i < 16; i++){
			std::string name = "ITC18_TTL_ASYCH_OUT" + (boost::format("%02d") % i).str();
			IOCapability * capability = new IOCapability(1000 + i,
														   name,
														   M_OUTPUT_DATA,
														   M_DEVICE_SPECIFIC_DATA,
														   M_ASYNCHRONOUS_IO,
														   0,
														   0, 5, 1);
			registerCapability(capability);
			//delete capability;
		}
		/*			capability = new IOCapability(1000, "ITC18_TTL_ASYCH_OUT00", M_OUTPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_ASYNCHRONOUS_IO, 0, 0, 5, 1); registerCapability(capability); delete capability;
		 capability = new IOCapability(1001, "ITC18_TTL_ASYCH_OUT01", M_OUTPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_ASYNCHRONOUS_IO, 0, 0, 5, 1); registerCapability(capability); delete capability;
		 capability = new IOCapability(1002, "ITC18_TTL_ASYCH_OUT02", M_OUTPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_ASYNCHRONOUS_IO, 0, 0, 5, 1); registerCapability(capability); delete capability;
		 capability = new IOCapability(1003, "ITC18_TTL_ASYCH_OUT03", M_OUTPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_ASYNCHRONOUS_IO, 0, 0, 5, 1); registerCapability(capability); delete capability;
		 capability = new IOCapability(1004, "ITC18_TTL_ASYCH_OUT04", M_OUTPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_ASYNCHRONOUS_IO, 0, 0, 5, 1); registerCapability(capability); delete capability;
		 capability = new IOCapability(1005, "ITC18_TTL_ASYCH_OUT05", M_OUTPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_ASYNCHRONOUS_IO, 0, 0, 5, 1); registerCapability(capability); delete capability;
		 capability = new IOCapability(1006, "ITC18_TTL_ASYCH_OUT06", M_OUTPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_ASYNCHRONOUS_IO, 0, 0, 5, 1); registerCapability(capability); delete capability;
		 capability = new IOCapability(1007, "ITC18_TTL_ASYCH_OUT07", M_OUTPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_ASYNCHRONOUS_IO, 0, 0, 5, 1); registerCapability(capability); delete capability;
		 capability = new IOCapability(1008, "ITC18_TTL_ASYCH_OUT08", M_OUTPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_ASYNCHRONOUS_IO, 0, 0, 5, 1); registerCapability(capability); delete capability;
		 capability = new IOCapability(1009, "ITC18_TTL_ASYCH_OUT09", M_OUTPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_ASYNCHRONOUS_IO, 0, 0, 5, 1); registerCapability(capability); delete capability;
		 capability = new IOCapability(1010, "ITC18_TTL_ASYCH_OUT10", M_OUTPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_ASYNCHRONOUS_IO, 0, 0, 5, 1); registerCapability(capability); delete capability;
		 capability = new IOCapability(1011, "ITC18_TTL_ASYCH_OUT11", M_OUTPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_ASYNCHRONOUS_IO, 0, 0, 5, 1); registerCapability(capability); delete capability;
		 capability = new IOCapability(1012, "ITC18_TTL_ASYCH_OUT12", M_OUTPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_ASYNCHRONOUS_IO, 0, 0, 5, 1); registerCapability(capability); delete capability;
		 capability = new IOCapability(1013, "ITC18_TTL_ASYCH_OUT13", M_OUTPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_ASYNCHRONOUS_IO, 0, 0, 5, 1); registerCapability(capability); delete capability;
		 capability = new IOCapability(1014, "ITC18_TTL_ASYCH_OUT14", M_OUTPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_ASYNCHRONOUS_IO, 0, 0, 5, 1); registerCapability(capability); delete capability;
		 capability = new IOCapability(1015, "ITC18_TTL_ASYCH_OUT15", M_OUTPUT_DATA, M_DEVICE_SPECIFIC_DATA, M_ASYNCHRONOUS_IO, 0, 0, 5, 1); registerCapability(capability); delete capability;
		 */        
		
		// TODO -- add waveform capabilities
		
		// note:  here, 2000 will be mapped to hardware port 0 (in ITC18_channel class) (in the channel subclass used for words, this is called "word port 0")
		// note:  here, 2001 will be mapped to hardware port 1 (in ITC18_channel class) (in the channel subclass used for words, this is called "word port 1")
		IOCapability *capability;          
		capability = new IOCapability(2000, "ITC18_TTL_AUX_WORD0", M_OUTPUT_DATA, M_DIGITAL_UINT8_BIT, M_ASYNCHRONOUS_IO, 0, 0, 5, 1); registerCapability(capability); 
		capability = new IOCapability(2001, "ITC18_TTL_AUX_WORD1", M_OUTPUT_DATA, M_DIGITAL_UINT8_BIT, M_ASYNCHRONOUS_IO, 0, 0, 5, 1); registerCapability(capability); 
		
		capabilitiesSet = true;						
	}
	
	return capabilities;        // return a pointer to the list of capabilities (protected data)
}


// this method tranlaates the user request info into ITC-specific types
// have to determine what type of buffer to use for the data on this channel
ITC18DataType mITC18_IODevice::computeITCDataType(IOChannel* chan) {
	
	IODataDirection dataDirection = (chan->getCapability())->getDataDirection();
	
	//IODataType dataType = (chan->getCapability())->getDataType();    // JJD change Jan 2007
	IODataType requestedDataType = (chan->getRequest())->getRequestedDataType();        // this is what the user asked for
	if (requestedDataType == M_DEVICE_SPECIFIC_DATA) {
		merror(M_IODEVICE_MESSAGE_DOMAIN, "ITC18 ERROR: requested ITC data type must be specified by user request.");
		return ITC_UNKNOWN_TYPE;
	}
	
	IODataSynchronyType synchronyType = (chan->getCapability())->getSynchronyType();
	
	if ((dataDirection == M_INPUT_DATA) && (synchronyType == M_HARDWARE_TIMED_SYNCHRONOUS_IO)) {
		if (requestedDataType == M_ANALOG_DATA) return ITC_ADC_INPUT_CONTINUOUS_TYPE;
		if (requestedDataType == M_ANALOG_SNIPPET_DATA) return ITC_ADC_INPUT_WAVEFORM_TYPE;     
		// this type also requires: a pointer to one of the digital ports, a start time (re digital pulse), and an end time (re digital pulse)
		// ( all of this should be contianed in the capability and be checked at the time the the channel is established)  // TODO
		
		if (requestedDataType == M_EDGE_LOW_TO_HIGH) return ITC_TTL_INPUT_EDGE_LOW_TO_HIGH_TYPE;
		if (requestedDataType == M_EDGE_HIGH_TO_LOW) return  ITC_TTL_INPUT_EDGE_HIGH_TO_LOW_TYPE;
		if (requestedDataType == M_EDGE_ANY) return ITC_TTL_INPUT_EDGE_ANY_TYPE;
		if (requestedDataType == M_DIGITAL_DATA) return ITC_TTL_INPUT_CONTINUOUS_TYPE;
	}
	
	if ((dataDirection == M_OUTPUT_DATA) && (synchronyType == M_ASYNCHRONOUS_IO)) {
		if (requestedDataType == M_DIGITAL_DATA) return ITC_AUXPORT_ASYCH_OUT_TYPE;
		if (requestedDataType == M_DIGITAL_PULSE_ACTIVE_HIGH) return ITC_AUXPORT_ASYCH_OUT_PULSE_ACTIVE_HIGH;
		if (requestedDataType == M_DIGITAL_PULSE_ACTIVE_LOW) return ITC_AUXPORT_ASYCH_OUT_PULSE_ACTIVE_LOW;
		if (requestedDataType == M_DIGITAL_UINT8_BIT) return ITC_AUXPORT_ASYCH_OUT_WORD_TYPE;    // JJD added Nov 7, 2007
	}
	
	
	merror(M_IODEVICE_MESSAGE_DOMAIN, "ITC18 ERROR: ITC data type not known. Please check user requested data type.");
	return ITC_UNKNOWN_TYPE;
}

short mITC18_IODevice::setupHardwarePort(IOChannel * chan) {
	
	short capabilityID = ((chan->getCapability())->getIdentifier());
	short hardwarePort;
	
	hardwarePort = capabilityID - (((short)(capabilityID/100))*100);
	
	if (capabilityID<700) {
		// harware port is the physical number on the board (e.g. 0-8)
		if ((hardwarePort<0) || (hardwarePort>=N_ANALOG_INPUT_CHANNELS))  {
			merror(M_IODEVICE_MESSAGE_DOMAIN,
				   "ITC18 ERROR: hardware port out of range. requested hardware port=%d",hardwarePort);
			return 0;
		}
	}
	return hardwarePort;
	
	
	merror(M_IODEVICE_MESSAGE_DOMAIN,
		   "ITC18 channel error:  hardware port number could not be determined.");
	return 0;
}


// check each channel for individual compatability with the device    
// in this case, the base class method takes care of creating the incompatability 
//  object and fills the modified request contained in the incompatability object to be the same as the original request.
//  All we need to do here is:
//  1) do more advanced checking on the capability that is specific to this device
//      in particular, ITC18 supports multiple types of digital data out (continuous, edge detect high, edge detect low, both, waveforms, ...)
//  2) if we find a problem with the request, change 
//  any parts of the modified request that we do not like.
bool mITC18_IODevice::validateIndividualChannel(IOChannel * tentative_channel, 
												IOChannelIncompatibility *  incompatibility){
	
	bool success = IODevice::validateIndividualChannel(tentative_channel, incompatibility);    // call parent method
    
	// more specific validation depends on the specific type of ITC channel 
	success = ((IOChannel_ITC18 *)tentative_channel)->validate(incompatibility); // DDC: super dangerous
	return (success);
}




// this function is meant to take a list of tentative channels (first_pass_validated_channels)
//  that have been partly screened (individually), and to decide if those channles can actually be run together
//
// if the passed list of channels cannot run together as a group for any 
//      reason, the method will return false
//   In this case, a set of incompatabilities will be created (one per 
//      tentative channel)
//   This list should be made so that they can run together as a group
//   In this case, the core should formulate a new set of requests (based on 
//      the incompatabilities) and retry to mapRequestToChannels
//  
// If the passed list of channels can run together exactly as specified, then this method will return true
//   and no incompatibilities will be created.
//   In this case, the core should make all the tentative channels into actual channels
bool mITC18_IODevice::validateMultipleChannels(ExpandableList<IOChannel> *first_pass_validated_channels) {				
	
	//IOChannelRequest* request;
	
	// communication with the core is through the list of incompatabilities.
	incompatibilities->clear();     // the base class should do this, but just to be sure
	
	// work with temporary list of incompatabilities
	//  so that the calls can change modifed requests.
	// this will only be added to the base class set of incompatabiliteis if we are unsuccessful
	
	// first clear the temporary incompatability list
	temporaryIncompatabilityList->clear();
	
	// now add elements to the list
	// note, there is a one-to-one mapping between the channles and the temp incomp list
	for(int i = 0; i < first_pass_validated_channels->getNElements(); i++){
		// get the request in question
		IOChannelRequest* request = (first_pass_validated_channels->getRawElement(i))->getRequest();
		shared_ptr<IOChannelIncompatibility> incompatibility(
															  new IOChannelIncompatibility(request, request));
		temporaryIncompatabilityList->addElement(incompatibility);
		
	}
	
	// checks:
	// 1) number of channels requested
	//   for now, we will limit things so that we do not have mnore than 8 Dig and 8 analog input channels
	//   the first 8 of each will be considered further to try to create a usable request list 
	//   and the remaining requests will have their incompatabilies "modified_request" set to NO_CAPABILITY
    
	//TODO
	//if 
	//temporaryIncompatabilityList->clear();
	//return false;
	
	// 2) we must also check that each capability is only occupied once
	//TODO
	
	// 3) Some channels depend on others being present.  In particular, a waveform channels requires the 
	//      presence of its requested edge channel
	IOChannel_ITC18 *chan;
	IOChannel_ITC18_TTL_edge *edgeChannel;
	
	for(int i = 0; i < first_pass_validated_channels->getNElements(); i++){
		chan = (IOChannel_ITC18 *)(first_pass_validated_channels->getRawElement(i));
		
		// debugging
		int dataType = ((chan->getRequest()))->getRequestedDataType();
		if (VERBOSE_IO_DEVICE) mprintf("ITC18 validation.  Debugging.  Data type in request = %d",dataType);
		
		if ( (chan->getITCDataType()) == ITC_ADC_INPUT_WAVEFORM_TYPE) {     // TODO -- use channel object type instead
			int requestedLinkedDigitalPort = ((IOChannelRequest_TriggeredAnalogSnippetITC18 *)(chan->getRequest()))->getRequestedLinkedDigitalPort();
			if (VERBOSE_IO_DEVICE) mprintf("ITC18 is validating a waveform channel.  Looking for an edge channel on TTL port %d",requestedLinkedDigitalPort);
			
			// will find the first edge channel using this hardware port
			edgeChannel = findEdgeChannelUsingTTLport(first_pass_validated_channels, requestedLinkedDigitalPort);
			if (edgeChannel == NULL) {
				merror(M_IODEVICE_MESSAGE_DOMAIN,"  ITC18 ERROR:validateMultipleChannels: Waveform channel will not operate because there is no edge channel using request hardware port (port = %d) .",requestedLinkedDigitalPort);
				// TODO incompatability ??  
				return false;
			}
			if (VERBOSE_IO_DEVICE) mprintf("ITC18:validateMultipleChannels: Waveform channel validated OK -- found edge channel on hardware port %d",requestedLinkedDigitalPort);
		}
	}
	
	
	// 4) try to build an instruction sequence with requested channels
	//  (the critical issue here is the sampling rate)
	if (!createInstructionSequence(first_pass_validated_channels, true)) {  
		merror(M_IODEVICE_MESSAGE_DOMAIN,"ITC18 ERROR:validateMultipleChannels: could not create instruction sequence.");
		temporaryIncompatabilityList->clear();
		return false;
	}
	
	// TODO -- check that user is not using an aux out channel for both a word and some other output.  If so, probably let proceed, but warn.
	
	
	// if we got this far, tentative incompatabilities have been added to the list with 
	//  modified requests that should work.  Update these in the object
	if (!AllChannelRequestsMetWithinSamplingTolerance) {
		for(int i = 0; i < temporaryIncompatabilityList->getNElements(); i++){
			incompatibilities->addElement( temporaryIncompatabilityList->getElement(i));
		}
		temporaryIncompatabilityList->clear();
		return false;
	}
	
	temporaryIncompatabilityList->clear();
	return true;                                
	
}            	




// ----------------------------------------------------------------------------------------------------------------------
// here the ITC will try to create an instruction sequence to satisfy all the established channels (i.e. matched capabilities) 
//		and load that instruction sequence into the ITC
//  - take all requests and try to make an instruction sequence  (only data output assumed at this point)
//  - report errors if instruction sequence cannot be built
bool mITC18_IODevice::initializeChannels(){
	
	instructionSequenceIsNeeded = checkIfInstructionSequenceIsNeeded();
	
	// translate the established channels into a "SETUP", build the instruction sequence, and setup the software buffers
	if (!createInstructionSequence(channels, false)) {
		merror(M_IODEVICE_MESSAGE_DOMAIN,
			   "ITC18 ERROR:initializeChannels: could not create instruction sequence.");
		return false;
	}
	
	// load the instruction sequence into the ITC18 and setup ranges
	if (!loadInstructionSequence()) {
		merror(M_IODEVICE_MESSAGE_DOMAIN,
			   "ITC18 ERROR:initializeChannels: could not load instruction sequence.");
		return false;
	}
	
	// clear buffers, setup notifications, and activate
	// we make sure check that all the software buffers are clear 
	//	(we just initialized, so there should be no valid data)
	// and make sure that all channels are initialized as "active"
	// setup (create and attach) any notifications that are needed
	// the channel objects take care of creating and destroying these
	for (int i=0;i<channels->getNElements(); i++) {
		IOChannel_ITC18 *chan = (IOChannel_ITC18 *)(channels->getRawElement(i));
		chan->clearAllLinkedChannels();   // make sure nothing is attached to these channels -- it will be done in "setup" below
	}
	for (int i=0;i<channels->getNElements(); i++) {
		IOChannel_ITC18 *chan = (IOChannel_ITC18 *)(channels->getRawElement(i));
		
		shared_ptr<mITC18_IODevice> this_one = shared_from_this();
		
		chan->setup(this_one,i);    // ITC specific setup (e.g. link waveform channel to edge channel)
		chan->initialize();     // base class
		chan->activate();       // base class
	}
	
	fullyInitialized = true;
	return true;
	
}

// Specify the AD voltage range that will be used each analog channels (these are hardware port indecies, NOT mIOchannel indicies!)
// this will force 10V defaults on all the anlog inputs 
bool mITC18_IODevice::setupADRanges() {
	
	for (int ADhardwarePort = 0; ADhardwarePort < ITC18_AD_CHANNELS; ADhardwarePort++) {
		inputRangesOnADhardwarePorts[ADhardwarePort] = ITC18_AD_RANGE_10V;			// This means +/- 10.24 Volts,  each integer step = 312.5 uV of input
	}
	
	// override with any requests by the channels
	for (int i=0;i<channels->getNElements(); i++) {
		
		IOChannel_ITC18 *chan = (IOChannel_ITC18 *)(channels->getRawElement(i));
		int ADhardwarePort = chan->getHardwarePort();
		int itc_range_tag = chan->getITCrangeTag();
		if (itc_range_tag >= 0) {	// override requested
			inputRangesOnADhardwarePorts[ADhardwarePort] = itc_range_tag;
		}
		
	}
	
	return true;
}


bool mITC18_IODevice::checkIfInstructionSequenceIsNeeded(){
	for(int i = 0; i < channels->getNElements(); i++){
		IOChannel_ITC18 *chan = (IOChannel_ITC18 *)(channels->getRawElement(i));
		if (chan->isInstructionSeqNeeded()) {
			return true;
			break;
		}
	}
	return false;
}



// analog input range:  convert from a requested value to a int tag known to the itc
int mITC18_IODevice::getAnalogInputRangeTag(double requestedAnalogInRangeV) {
    
	if (requestedAnalogInRangeV == VALID_ITC_ANALOG_IN_RANGE_VALUE_1V) {
		return(ITC18_AD_RANGE_1V);
	}
	if (requestedAnalogInRangeV == VALID_ITC_ANALOG_IN_RANGE_VALUE_2V) {
		return(ITC18_AD_RANGE_2V);
	}
	if (requestedAnalogInRangeV == VALID_ITC_ANALOG_IN_RANGE_VALUE_5V) {
		return(ITC18_AD_RANGE_5V);
	}
	if (requestedAnalogInRangeV == VALID_ITC_ANALOG_IN_RANGE_VALUE_10V) {
		return(ITC18_AD_RANGE_10V);
	}
	merror(M_IODEVICE_MESSAGE_DOMAIN,
		   "ITC18 analog range error:  analog input range not known.");
	return(-1);
}

float mITC18_IODevice::getMultiplierToGetMV(int itc_range_tag) {
	
	float multiplier_to_get_UV = -1.0;
	
	if (itc_range_tag == ITC18_AD_RANGE_1V) {
		multiplier_to_get_UV = 31.25;
	}
	else if (itc_range_tag == ITC18_AD_RANGE_2V) {
		multiplier_to_get_UV = 62.5;
	}
	else if (itc_range_tag == ITC18_AD_RANGE_5V) {
		multiplier_to_get_UV = 156.25;
	}
	else if (itc_range_tag == ITC18_AD_RANGE_10V) {
		multiplier_to_get_UV = 312.5;
	}
	
	if (multiplier_to_get_UV>0)  {
		return ( (multiplier_to_get_UV/1000.));
	}
	else {
		merror(M_IODEVICE_MESSAGE_DOMAIN,
			   "ITC18 analog range mult error:  itc range tag unknown.");
		return(-1);
	}
}

// override these methods to allow starting and stopping of the running of the instruction sequence
bool mITC18_IODevice::startDeviceIO(){
	
	if (deviceIOrunning) {
		mwarning(M_IODEVICE_MESSAGE_DOMAIN,
				 "ITC18 startDeviceIO:  startDeviceIO request was made without first stopping IO.  Attempting to stop and restart now.");
		if (!stopDeviceIO()) {		// try to stop
			merror(M_IODEVICE_MESSAGE_DOMAIN,
				   "ITC18 IO could not be stopped.  Stop and restart has failed.");
			return false;
		}
	}
	
	
	if (VERBOSE_IO_DEVICE) mprintf("ITC18:  Starting device IO \n");
	
	if (!isAttached())  {       // if deviced not attached, return false and error message
		merror(M_IODEVICE_MESSAGE_DOMAIN,
			   "Error: IODevice::startDeviceIO:  startDeviceIO request made without first attaching device.");
		return false;
	}
	
	if (!fullyInitialized) {
		merror(M_IODEVICE_MESSAGE_DOMAIN,
			   "Error: IODevice::startDeviceIO:  startDeviceIO request made without full device initialization.");
		return false;
	}
	
	// debug only -- open a data file
	//outDataFile.open("JJD_test.txt", ios::out | ios::trunc);
	//mprintf("opening test data file:  JJD_test.txt");
	//if (!outDataFile) {
	//	mprintf("Could not open data file!!!");
	//	return false;
	//}
	
	// do not start instruction seq unless it is needed
	// note:  no scheduling is needed unless an instruction sequence is needed   
	if (instructionSequenceIsNeeded) {
		// time 0 !   will make sure the FIFO is blank and then start the instruction sequence
		if (!startRunningITC18instructSequence()) {
			merror(M_IODEVICE_MESSAGE_DOMAIN,
				   "Error: IODevice::could not start running instruction seq...");
			return false;			
		}
	}
	
	// debug only (next two lines) -- force all data to be read and post at the end (must only run for short time)
	//deviceIOrunning = true;
	//return true;
	
	// start scheduling FIFO clearing and update of all channels
	
	if (!startDeviceIOchannels()) {
		stopRunningITC18instructSequence();
		merror(M_IODEVICE_MESSAGE_DOMAIN,
			   "Error: IODevice::could not start scheduled channel checking...");
		return false;		
	}
	
	deviceIOrunning = true;
	return true;
}



bool mITC18_IODevice::stopDeviceIO(){
	
	if (!deviceIOrunning) return false;
    
	if (VERBOSE_IO_DEVICE) mprintf("ITC18:  Stopping device IO... \n");
	
	if (!isAttached()) {      // if no device is attached, return false and error message
		merror(M_IODEVICE_MESSAGE_DOMAIN,
			   "Error: IODevice::stopDeviceIO:  stopDeviceIO request made without first attaching device.");
		return false;
	} 
	
	if (!fullyInitialized) {
		merror(M_IODEVICE_MESSAGE_DOMAIN,
			   "Error: IODevice::startDeviceIO:  stopDeviceIO request made without full device initialization.");
		return false;
	}
	
	
	// this is ITC thread safe (itc locked from the outside).
	if (instructionSequenceIsNeeded) stopRunningITC18instructSequence();		// will stop the itc -- no more data into FIFO.
	
	// stop all the scheduled channel checking (i.e. stop calls to "updateChannel")
	// this will stop both flushing of the FIFO and posting of data in the software buffers
	stopAllScheduleNodes();		// IO device base class method -- this is thread safe
	
	// now get and post all available data after the sequence is stopped.
	// this next call will flush anything still in the FIFO
	if (instructionSequenceIsNeeded) fullFlushITC18FIFO();	// this is ITC thread safe (itc locked from the outside).
	
	// this will do a channel update for all established channels
	// effectively, this will move any and all unposted data from the software buffers to the event stream
	for(int i = 0; i < channels->getNElements(); i++){
		
		IOChannel_ITC18 *chan = (IOChannel_ITC18 *)(channels->getRawElement(i));
		chan->stopChannelIO();       // ITC channel specific:  post all data, clear waveforms, etc.
		
	}
	
	
	// debug only
	//outDataFile.close();
	//mprintf("closing test data file");
	
	deviceIOrunning = false;
	
	if (VERBOSE_IO_DEVICE) mprintf("ITC18:  ITC18 has stopped and all data has been flushed. \n");
	
	return true;
	
}


// There are two things that need to be scheduled
//  1) a call to flush the FIFO -- this will be done at  the rate that the user actually 
//		 wants the data (i.e. at the update interval) and will full flush each time
//       and it will pull all avialable data off the FIFO
//       (You cannot pull data until a full instruction sequence is presented)
//       This flush will cause any data pulled off to be parsed to the appropriate buffers
//       (during this parsing, channels with requested sampling at a lower rate, will have some of their data discarded)
//  2) a set of calls to move the data from each software buffer (one per channel) to the even stream
//       The frequency of each of these calls is controlled by the channelRequest (requested_update_interval_usec)
bool mITC18_IODevice::startDeviceIOchannels() {
	
	// be sure that all my nodes are stopped and 
	//    free the memory of any existing nodes
	stopAllScheduleNodes();		// this is a base class method to cancel any IO nodes that are running
	
	IOChannel_ITC18 *chan;
	
	// 1) schedule FIFO flushing
	
	// JJD change Oct 11, 2006 ================================================
	
	// determine FIFO update interval here
	bool needScheduledFIFOclear = false; 
	// look at the channels and find the duration of the shortest update interval
	long minUpdateIntervalUS = DEFAULT_FIFO_UPDATE_INTERVAL_US;
	for(int i = 0; i < channels->getNElements(); i++){
		chan = (IOChannel_ITC18 *)(channels->getRawElement(i));
		if (chan->isSchedulable()) {
			long updateInterval = (chan->getRequest())->getRequestedUpdateIntervalUsec();
			if (updateInterval < minUpdateIntervalUS) {
				minUpdateIntervalUS = updateInterval;
			}
			//mprintf("ITC:  Channel index %d  update interval = %d us",i,updateInterval);
			needScheduledFIFOclear = true;
		} 
	}
	
	if (needScheduledFIFOclear) {
		//mprintf("ITC:  Min update interval (across channels) = %d us",minUpdateIntervalUS);
		if (usingUSB) {
			flushFIFOintervalUS = m_max(((long)(minUpdateIntervalUS)),USB_MIN_FIFO_FLUSH_INTERVAL_US);
		} 
		else {
			flushFIFOintervalUS = m_max(((long)(minUpdateIntervalUS)),PCI_MIN_FIFO_FLUSH_INTERVAL_US);
		}
		//mprintf("ITC:  flush FIFO interval = %d us", flushFIFOintervalUS);
		
		shared_ptr<UpdateIOChannelArgs> args = shared_ptr<UpdateIOChannelArgs>(new UpdateIOChannelArgs());
		args->device = shared_from_this();
		args->channel_index = FLUSH_FIFO_CHANNEL;
		//mprintf("Scheduling ITC FIFO flush at  %d usec intervals", flushFIFOintervalUS);			
		
		
		shared_ptr<Scheduler> scheduler = Scheduler::instance();
		
		//#define DO_CRAZY_SHIT
#ifdef DO_CRAZY_SHIT
		ScheduleTask *node = scheduler->scheduleConstrainedUS(0, 
															   flushFIFOintervalUS, 
															   M_REPEAT_INDEFINITELY, 
															   boost::bind(update_io_channel, 
																		   args),
															   50,
															   (flushFIFOintervalUS+WARN_SCHEDULER_ADD_US),			// warn interval equals the update interval
															   M_DEFAULT_FAIL_SLOP_US,
															   M_MISSED_EXECUTION_DROP);
#else
		shared_ptr<ScheduleTask> node = scheduler->scheduleUS(FILELINE,
															   0,		// normally 0
															   flushFIFOintervalUS, 
															   M_REPEAT_INDEFINITELY, 
															   boost::bind(update_io_channel, 
																		   args),
															   M_DEFAULT_IODEVICE_PRIORITY,
															   (flushFIFOintervalUS+WARN_SCHEDULER_ADD_US),
															   M_DEFAULT_FAIL_SLOP_US,
															   M_MISSED_EXECUTION_DROP);
#endif
		if (node == NULL) {
			merror(M_IODEVICE_MESSAGE_DOMAIN,
				   "Node could not be scheduled");
			return false;
		}			
		if (VERBOSE_IO_DEVICE) mprintf("Scheduled flush of FIFO at %d us intervals (node= %d) task = %u", flushFIFOintervalUS, schedule_nodes.size(),node.get());
		schedule_nodes_lock.lock();		 // JJD Feb 2007
		schedule_nodes.push_back(node); 
		schedule_nodes_lock.unlock();		 // JJD Feb 2007
		
	}
	
	
	// 2) individual channels
	//if (!DO_NEW_SCHEDULER_APPROACH) {
	for(int i = 0; i < channels->getNElements(); i++){
		
		chan = (IOChannel_ITC18 *)(channels->getRawElement(i));
		
		if ((chan->isSchedulable()) && (chan->isInitialized()) ) {		// check that we have already initialized the cahnnel before we try to schedule its update method 
			
			
			shared_ptr<UpdateIOChannelArgs> args = shared_ptr<UpdateIOChannelArgs>(new UpdateIOChannelArgs());
			args->device = this->shared_from_this();
			args->channel_index = i;
			
			// here, we ask the scheduler to call update_io_channel at an interval specified by the channel
			// JJD note: for continously sampled data types, we expect that this each such call will cause ALL the available to data to come down off the device 
			//  (e.g. the parameter may be updated several times by a single call to update_io_channel)
			// of course, this depends on the sampling rate of the channel and the update rate of the channel (there is no requirement that they be matched)
			
#ifdef DO_CRAZY_SHIT
			shared_ptr<Scheduler> scheduler = Scheduler::instance();
			ScheduleTask *node = scheduler->scheduleConstrainedUS(0, 
																   (chan->getRequest())->getRequestedUpdateIntervalUsec(), 
																   M_REPEAT_INDEFINITELY, 
																   boost::bind(update_io_channel, 
																			   args),
																   
																   50,((chan->getRequest())->getRequestedUpdateIntervalUsec()+WARN_SCHEDULER_ADD_US),
																   M_DEFAULT_FAIL_SLOP_US,
																   M_MISSED_EXECUTION_DROP);
#else
			shared_ptr<Scheduler> scheduler = Scheduler::instance();
			shared_ptr<ScheduleTask> node = scheduler->scheduleUS(std::string(FILELINE " :") + chan->getName(), 0, 
																   (chan->getRequest())->getRequestedUpdateIntervalUsec(), 
																   M_REPEAT_INDEFINITELY, 
																   boost::bind(update_io_channel, 
																			   args),
																   M_DEFAULT_IODEVICE_PRIORITY,((chan->getRequest())->getRequestedUpdateIntervalUsec()+WARN_SCHEDULER_ADD_US),
																   M_DEFAULT_FAIL_SLOP_US,
																   M_MISSED_EXECUTION_DROP);
#endif	
			
			if (node == NULL) {
				merror(M_IODEVICE_MESSAGE_DOMAIN,
					   "Node could not be scheduled");
				return false;
			}	
			
			schedule_nodes_lock.lock();		 // JJD Feb 2007
			if (VERBOSE_IO_DEVICE) mprintf("Scheduled flush of channel matched to capability = %s at %d us intervals (node= %d) task = %u",  
										   (chan->getCapability())->getName().c_str(), (chan->getRequest())->getRequestedUpdateIntervalUsec(), schedule_nodes.size(),node.get());
			schedule_nodes.push_back(node); 
			schedule_nodes_lock.unlock();		 // JJD Feb 2007
		}	
		
	}
	//}
	
	return true;
}




// this is called by the scheduler once every 'requested update interval' for EACH channel
// the channel must have been initialized to get this far, but it may not be active (the update method will handle that)
// Nov 5, 2004 -- this looks ready to go. 	
// July 2005, base class now expects a boolean return 
// for the ITC, this routine will only push ALL data from the channel's software buffer to the event stream
//  (IT WILL NOT flush the FIFO! -- a separate update is scheduled for that. 
// returns false only on an error, otherwise return true

// inputs will run using this method, as well as some outputs (e.g. STOP pulse)
bool mITC18_IODevice::updateChannel(int channel_index){
    
	// if channel is made inactive, must discard the data from software buffers, but do not post.
	// this is handled by the channel "update" method in the base class (do not need to do anything here)
	
	if (VERBOSE_IO_DEVICE > 2) {
		shared_ptr<Clock> clock = Clock::instance();
		mprintf("Call to update an input/output channel received:  channel index = %d, global time = %d msec", channel_index,(long)clock->getCurrentTimeMS());
	}
	
	if (channel_index == FLUSH_FIFO_CHANNEL) {      // signal to flush
		
		//setAsychLinesHigh(0x0004); 
		if (VERBOSE_IO_DEVICE > 2) {
			shared_ptr<Clock> clock = Clock::instance();
			mprintf("Call to update channel received:  now calling for fullFlushITC18FIFO. %d",(long)(clock->getCurrentTimeUS()));
		}
		fullFlushITC18FIFO();
		//setAsychLinesLow(0x0004);	
		
		// JJD change Jan 2007 -- minimize number of scheduler calls -- only one needed to flush all channels
		/*
		 if (DO_NEW_SCHEDULER_APPROACH) {
		 for(int i = 0; i < channels->getNElements(); i++){
		 IOChannel_ITC18 *chan = (IOChannel_ITC18 *)(channels->getRawElement(i));
		 if (chan->isSchedulable()) chan->flushChannel();
		 }
		 }
		 */
		return true;
		
	}
	
	else {
		
		// move any and all data on the software buffer for this channel into the event stream
		//setAsychLinesHigh(0x0002);
		//mprintf("Flushing channel %d",channel_index);
		((IOChannel_ITC18 *)(channels->getRawElement(channel_index)))->flushChannel();
		//setAsychLinesLow(0x0002);
		return true;
	}
	
	return false;	// channel index out of range ?
	
}	

// this method is called by the base class after an asych notiofication object has been attached to the variable
// outputs will run using this method
//  e.g. the notify method in the notification object will call this with the channel index and the data
bool mITC18_IODevice::updateChannel(int channel_index, Data data){	
	
	// call the notify method for this ITC18 channel
	if (VERBOSE_IO_DEVICE > 2) {
		shared_ptr<Clock> clock = Clock::instance();
		mprintf("ITC18: Call to update an OUTPUT channel received:  channel index = %d, global time = %d msec", channel_index,(long)clock->getCurrentTimeMS());
	}
	((IOChannel_ITC18_AsychOut *)(channels->getRawElement(channel_index)))->notify(data);
	return true;
}



// these functions are special ITC 18 functions to be used for asych digital output 
// only the indicated lines will be toggled -- others will be unchanged
bool mITC18_IODevice::setAsychLinesHigh(short asychLinesToSetHigh) {
	
	//bit pattern conventions:
	// "0x" = hex prefix (converts the hex number to a short)
	// 16 bits on ITC output board
	// each group of 4 is controlled by one of the four digits
	// 0x0001 = first bit is high (others low)
	// 0x0002 = second bit is high
	// 0x0003 = first AND second bits are high
	// 0xFFFF = all bits high
	//  e.g. ->setAsychOutLines(0x0002); will set the second port on the ITC high
	//  e.g. ->setAsychOutLines(0x0000); will set all outputs low
	
	//mprintf("Call to set ITC18 bit pattern");
	if (itc != NULL) {	
		shared_ptr<Clock> clock = Clock::instance();
		
		asychLinesStatus |= asychLinesToSetHigh;		//"OR" will cause any new lines to go high
		ITC18DriverLock->lock();
		if (VERBOSE_IO_DEVICE) mprintf("About to Set ITC aux outputs.  Current time: %d ms", (long)clock->getCurrentTimeMS());
		int e = ITC18_WriteAuxiliaryDigitalOutput(itc, asychLinesStatus); 
		if (VERBOSE_IO_DEVICE) mprintf("Finished setting ITC aux outputs.  Current time: %d ms", (long)clock->getCurrentTimeMS());
		
		ITC18DriverLock->unlock();
		return (e == noErr); 
	}
	return false;
}

bool mITC18_IODevice::setAsychLinesLow(short asychLinesToSetLow) {
	
	if (itc != NULL) {	
		shared_ptr<Clock> clock = Clock::instance();
		
		asychLinesStatus &= (~asychLinesToSetLow);	// take complement (~) and then "AND"
		ITC18DriverLock->lock();
		if (VERBOSE_IO_DEVICE) mprintf("About to Set ITC aux outputs.  Current time: %d ms", (long)clock->getCurrentTimeMS());
		int e = ITC18_WriteAuxiliaryDigitalOutput(itc, asychLinesStatus);
		if (VERBOSE_IO_DEVICE) mprintf("Finished setting ITC aux outputs.  Current time: %d ms", (long)clock->getCurrentTimeMS());
		ITC18DriverLock->unlock();
		return (e == noErr);
	}		
	return false;
	
}

bool mITC18_IODevice::setAsychOutputWord8(short wordPort, char desiredWordIn8bitFormat) {
	
	// word port should be 0 or 1
	// word port "0" is ITC lines 0-7 (0 is the least sig bit in the word)
	// word port "1" is ITC lines 8-15 (8 is the least sig bit in the word)
	
	short desiredAsychLinesStatus;
	short desiredWordIn16bitFormat = desiredWordIn8bitFormat;	
	
	switch (wordPort) {
		case WORD_PORT_0:
			desiredAsychLinesStatus = ((asychLinesStatus & 0xFF00) | desiredWordIn16bitFormat);
			break;
		case WORD_PORT_1:
			desiredWordIn16bitFormat = (desiredWordIn16bitFormat<<8);		// move 8 bit word over by 8 bits.
			desiredAsychLinesStatus = ((asychLinesStatus & 0x00FF) | desiredWordIn16bitFormat);
			break;
		default:
			mwarning(M_IODEVICE_MESSAGE_DOMAIN,"ITC18 channel: call to set on output word, but word port not recognized");
			return (false);
	}		
	
	if (VERBOSE_IO_DEVICE) {
		mprintf("ITC asych out:  About to put a word on ITC aux outs.  current16bitLineStatus=%d desiredWord=%d desired16bitLineStatus=%d",
				asychLinesStatus, desiredWordIn16bitFormat,desiredAsychLinesStatus);
	}
	return(setAsychLinesToSpecifiedPattern(desiredAsychLinesStatus));
}


// this function will change ALL aux lines to the specified pattern
bool mITC18_IODevice::setAsychLinesToSpecifiedPattern(short desiredAsychLinePattern) {
	
	//bit pattern conventions:
	// "0x" = hex prefix (converts the hex number to a short)
	// 16 bits on ITC output board
	// each group of 4 is controlled by one of the four digits
	// 0x0001 = first bit is high (others low)
	// 0x0002 = second bit is high
	// 0x0003 = first AND second bits are high
	// 0xFFFF = all bits high
	//  e.g. ->setAsychOutLines(0x0002); will set the second port on the ITC high
	//  e.g. ->setAsychOutLines(0x0000); will set all outputs low
	
	if (itc != NULL) {	
		shared_ptr<Clock> clock = Clock::instance();
		
		ITC18DriverLock->lock();
		if (VERBOSE_IO_DEVICE) mprintf("About to Set ITC aux outputs.  Current time: %d ms", (long)clock->getCurrentTimeMS());
		int e = ITC18_WriteAuxiliaryDigitalOutput(itc, desiredAsychLinePattern); 
		asychLinesStatus = desiredAsychLinePattern;		// update what we know about the status of these lines
		if (VERBOSE_IO_DEVICE) mprintf("Finished setting ITC aux outputs.  Current time: %d ms", (long)clock->getCurrentTimeMS());
		ITC18DriverLock->unlock();
		return (e == noErr); 
	}
	return false;
}



// =========================================================================================================================================================
// protected ITC18 methods =================================================================================================================================

// Open and initialize the ITC18 -- return a pointer to the device
void  *mITC18_IODevice::openITC18() {
	
	void *pLocal;
	
	ITC18DriverLock->lock();
	
	// if already open, then close
	if (itc != NULL) {		// then we have already openned a device
		pLocal = itc;
		itc = NULL;
		ITC18_Close(pLocal);
	}
	else {
		pLocal = new char[ITC18_GetStructureSize()];
	}
	
	// open an ITC.
	// JJD updated Oct 3, 2006 to allow USB
	// old code (PCI):  ITC18_Open(itc, 0)
	// new code (USB):  ITC18_Open(itc, 0x10000)
	//                  ITC18_Open(itc, 0x10001) for second device
	
	// Now the ITC is closed, and we have a valid sized pointer  
	//mprintf("Trying to open an ITC18 device.");
	
	// try to open and initialize 
	bool tryUSB = true;
	usingUSB = false;
	if (ITC18_Open(pLocal, 0) == noErr) {	// PCI
		// the ITC has opened, now initialize it
		if (ITC18_Initialize(pLocal,ITC18_STANDARD) == noErr) {
			mprintf(M_IODEVICE_MESSAGE_DOMAIN, "The ITC18 device was successfully openned and initialized via PCI.");
			tryUSB = false;
		} 
		else {
			ITC18_Close(pLocal);
		}
	}
	
	if (tryUSB) {
		mprintf(M_IODEVICE_MESSAGE_DOMAIN,"The ITC18 remote device did not open using via PCI. Trying USB ...");
		if (ITC18_Open(pLocal, 0x10000) == noErr) {     // USB try
			if (ITC18_Initialize(pLocal,ITC18_STANDARD) == noErr) {
				mprintf("The ITC18 device was successfully openned and initialized via USB.");
				usingUSB = true;
			}
			else {
				merror(M_IODEVICE_MESSAGE_DOMAIN, 
					   "The ITC18 remote device did not INITIALIZE using PCI or USB. ITC unavialable"); 
				//delete pLocal; // BBK: this seems problematic ... put it two lines down
				ITC18_Close(pLocal);
				//					delete pLocal;
				ITC18DriverLock->unlock();
				return (NULL);
			}
		} else {
			mwarning(M_IODEVICE_MESSAGE_DOMAIN,"The ITC18 remote device did not OPEN via USB. ITC unavailable.");
			ITC18DriverLock->unlock();  
			return (NULL);
		}
	}
	
	justStartedITC18 = true;
	
	
	// debug only
	//timeITCwasOpenedUS = clock->getCurrentTimeUS();	
	
	ITC18DriverLock->unlock();  
	return(pLocal);
}



// Close the ITC18.  We do a round-about with the pointers to make sure that the
// pointer is nulled out before we close the ITC.  This is needed so that interrupt
// driven routines won't use the itc after ITC18_Close has been called.

void mITC18_IODevice::closeITC18(void)
{
	//Ptr pLocal;
	void *pLocal;   // JJD updated July 2005
	
	stopRunningITC18instructSequence();
	
	if (itc != NULL) {
		pLocal = itc;
		itc = NULL;
		ITC18DriverLock->lock();
		ITC18_Close(pLocal);
		ITC18DriverLock->unlock();
		//DisposePtr(pLocal);  // JJD updated July 2005
	}
}



bool mITC18_IODevice::startRunningITC18instructSequence(void) {
	
	if (VERBOSE_IO_DEVICE) mprintf("About to try to start running the itc instruction sequence...");
	
	if (!fullyInitialized) return false;
	if (instruction_seq_running) return false;
	
	
	// start the ITC instruct sequence
	if (itc != NULL) {
		justStartedITC18 = true;											// will allow us to discard the first few junk bytes
		
		ITC18DriverLock->lock();
		ITC18_SetSamplingInterval(itc, ITCTicksPerInstruction, false);		// note: ITC ticks per instruction controls the rate
		ITC18_InitializeAcquisition(itc);									// this will clear the FIFO
		ITC18_Start(itc, false, false, false, false);						// no trigger, no output
		ITC18DriverLock->unlock();
		
		instruction_seq_running = true;
		if (VERBOSE_IO_DEVICE) mprintf(" Started ITC18 instruction sequence...");
	}
	
	fudgeTime = (MonkeyWorksTime)(round(full_time_slice_us*0.5));
	shared_ptr<Clock> clock = Clock::instance();
	globalClockTimeAtStartUS = clock->getCurrentTimeUS();	
	
	//num_FIFO_clears_since_itc_started = 0;
	//time_since_inst_seq_started_US = 0;		// reset ITC timing		(this is kept in usec)
	
	// TODO -- call Instrutech to check this
	// testing shows that the ITC takes ~1 intruction cycle to post the first data after the sequence is started.
	num_FIFO_clears_since_itc_started = 1;
	time_since_inst_seq_started_US = (MonkeyWorksTime)(round(((double)num_FIFO_clears_since_itc_started)*full_time_slice_us*clockDriftAdjustFactor)) + fudgeTime;
	absoluteTimeUSbasedOnITCclock = globalClockTimeAtStartUS + time_since_inst_seq_started_US;
	
	nextCheckTimeUS = absoluteTimeUSbasedOnITCclock + (INTERVAL_TO_CHECK_CLOCK_DRIFT_MS*1000);  // next time to check for clock drift
	alreadyWarnedAboutDrift = false;
	
	// JJD notes:
	// *** the shortest number of ticks per instruction is 4 (ITC18_MINIMUM_TICKS)  (5 us, because each tick is 1.25 us).
	// thus, the fastest sampling rate is 200 kHz. (using only one channel)
	
	if (VERBOSE_IO_DEVICE) mprintf("Started ITC instruction seq running.");
	
	return true;
}


bool mITC18_IODevice::stopRunningITC18instructSequence(void)
{
	
	if (!instruction_seq_running) return false;
	
	if (itc != NULL) {
		
		ITC18DriverLock->lock();
		ITC18_Stop(itc);
		ITC18DriverLock->unlock();
		
		instruction_seq_running = false;
		
		// flush out any waveforms into buffers (these will be immediately available to the next call to get waveforms)
		//doingFinalFlushOfITC = true;		//  allow one more access read to data
		//flushOutWaveformsInProcess();
	}
	
	if (VERBOSE_IO_DEVICE) mprintf("Stopped ITC instruction seq running.");
	return true;
}



// delete the setup buffer and instruction buffer from memory
// these things are all created during the call to create instruction seq.
void mITC18_IODevice::deleteInstructionSeqArrays(void) {
	
	// I'm going straight to HELL...I hope garbage collection is working correctly! BEN 08/18/08 
	//
	//		if (setup != NULL) {
	//			for (int index = 0; index < setupSize; index++) { delete [] setup[index]; };
	//			delete [] setup; 
	//			setup = NULL;
	//		}	
	//		if (pInstructions != NULL) {
	//			delete [] pInstructions;
	//			pInstructions = NULL;
	//		}
	//		numInstr = 0;
	//		if (pSamples != NULL) {
	//			delete [] pSamples;
	//			pSamples = NULL;
	//		}
	
	setup=0;
	pInstructions=0;
	pSamples=0;
	//END Ben going to hell
	
	
}

// old initITC function is split here in three now
// 1) create instruction sequence
// 2) load instruction sequence
// 3) initialize all the software buffers 


// we pass in the channels here because this routine must be called to check potential channels
bool mITC18_IODevice::createInstructionSequence(ExpandableList<IOChannel> *channels_for_instruction_seq, bool checkForIncompatabilities) {
	
	int fastestSetupRate = 1;
	
	int *pInstrBuff = NULL;
	int currentInstruct;
	
	int thisSpeed;
	int index;
	double	itc18_ticks_per_us;
	float	desiredFullSequencePeriodUS,shortestFullSequencePeriodUS, exactITCticksPerInstructionNeeded;
	float	actualFastSamplingRateHz,fraction,actualSamplingRateHz;
	short	numberOfReadsPerTakenRead;
	float   actualSamplingIntervalUS;
	
	//ShortDataTimeStampedRingBuffer *analogBuffer;
	//ShortDataTimeStampedBufferReader *analogBufferReader;
	//mBoolDataTimeStampedRingBuffer *digitalBuffer; 
	//mBoolDataTimeStampedBufferReader *digitalBufferReader;
	
	//int numOutputAD = 0;
	//int numOutputDig = 0;
	float fastestRequestedSamplingRateHz = 0;
	
	//SKIP_DESC skipOutputAD[MAX_INSTRUCTS_PER_INSTRUCT_TYPE];
	//SKIP_DESC skipOutputDig[MAX_INSTRUCTS_PER_INSTRUCT_TYPE];
	
	//IOChannel * channel;
	//long	requestedSamplingIntervalUS;
	//short   requestedSamplingFrequencyHZ;
	//short   dataDirection, dataType;
	//short   ad_index, hardwarePort_index, type_index, assoc_chan_index;
	
	AllChannelRequestsMetWithinSamplingTolerance = true;    // assume OK until proven wrong
	
	// create and fill the "Setup" array
	// Now, "SETUP" is established, the OLD ITC18 code follows for creating and loading the instruction sequence ...
	makeSetup(channels_for_instruction_seq);	
	
	// find num of instruction lines needed
	if (!computeNumInstrLines()) return false;	// error
	if (!checkIfInstructionSequenceIsNeeded()) return true;	// we are successful because we do not need an instruction seq
	
	
	// compute rates and sampling   TODO -- should change everything to use sampling intervals, not Hz -----
	
	// This looks for the fastest setup item
	int *thisInstr = NULL;
	for (index = 0; index < setupSize; index++){
		thisInstr = setup[index];
		thisSpeed = thisInstr[SETUP_RATE_INDEX];
		if (thisSpeed > fastestSetupRate){
			fastestSetupRate = thisSpeed;
		}
	}
	fastestRequestedSamplingRateHz = fastestSetupRate;
	if (VERBOSE_IO_DEVICE) {
		mprintf("fastestRequestedSamplingRateHz = %f nsetups = %d",fastestRequestedSamplingRateHz,setupSize);
	}
	
	// check on rate (can we do it?)
	itc18_ticks_per_us = (ITC18_TICKS_PER_MS/1000.);		// typically 1/1.25  (1.25 us per tick)
	
	// the sampling period required (in us) = 1000000/ (desired sampling rate)
	desiredFullSequencePeriodUS = (1000000. / (float)fastestRequestedSamplingRateHz); 
	
	// given the number of instructions, the fastest sampling period (in us) is: minimum number of ticks needed / number of ticks in each uS (1.25)
	shortestFullSequencePeriodUS = (numInstr * ITC18_MINIMUM_TICKS) / itc18_ticks_per_us;
	if (desiredFullSequencePeriodUS < shortestFullSequencePeriodUS) {
		merror(M_IODEVICE_MESSAGE_DOMAIN,
			   "ITC18 is not fast enough to perform at the requested rates");  
		return(false);
	}
	
	// compute the number of ticks to put between each instruction
	exactITCticksPerInstructionNeeded = (itc18_ticks_per_us * desiredFullSequencePeriodUS) / ((float)numInstr);
	ITCTicksPerInstruction  = (long)exactITCticksPerInstructionNeeded;		// should act as floor (i.e. go faster if things are not nice integers)
	
	if (VERBOSE_IO_DEVICE) mprintf("ITC ticks per instruction desired: %f  actual: %d ",exactITCticksPerInstructionNeeded,ITCTicksPerInstruction);
	if (ITCTicksPerInstruction < ITC18_MINIMUM_TICKS) {
		merror(M_IODEVICE_MESSAGE_DOMAIN,
			   "ITC18 is not fast enough to perform at the requested rates");  
		return(false);
	}
	
	full_time_slice_us = ((double)(ITCTicksPerInstruction*numInstr))/itc18_ticks_per_us;		// time between the start of each full instruction sequence
	actualFastSamplingRateHz = (1./full_time_slice_us) * 1000000;
	if (VERBOSE_IO_DEVICE) {
		mprintf("ITC18:  numInstr: %d, ticksPerInstruction: %d, Time slice for each FIFO = %f us", numInstr, ITCTicksPerInstruction, full_time_slice_us);
	}
	
	// determine how many of the lower sampling rate samples will be ignored (e.g. if fastest sampling is at 20kHz, and chan 1 at 1kHz, only take every 20th sample)
	int inputAD, inputDig, outputAD, outputDig;
	inputAD = inputDig = outputAD = outputDig = 0;
	
	for (index = 0; index < setupSize; index++){
		
		thisInstr = setup[index];
		int channel_index = thisInstr[MIODEVICE_CHAN_INDEX];
		thisSpeed = thisInstr[SETUP_RATE_INDEX];
		fraction = actualFastSamplingRateHz/(float)thisSpeed;
		numberOfReadsPerTakenRead = (short)(fraction);			// should be a number between 1 and ?? 100
		if (numberOfReadsPerTakenRead < 1) {
			merror(M_IODEVICE_MESSAGE_DOMAIN,
				   "Failed to make instruction sequence."); 
			return(false);      // setup failed in this case !!!
		} 
		
		
		// this a bit kludgy -- need to refactor sometime and get rid of all this Setup stuff
		IOChannel_ITC18 *chan = (IOChannel_ITC18 *)(channels_for_instruction_seq->getRawElement(channel_index));
		chan->setSkippingMod(numberOfReadsPerTakenRead);
		
		
		switch (thisInstr[SETUP_AD_INDEX]) {
			case input_AD:
				inputAD++;
				if (inputAD >= MAX_INSTRUCTS_PER_INSTRUCT_TYPE) return(false); 
				break;
			case input_digital:
				inputDig++;
				if (inputDig >= MAX_INSTRUCTS_PER_INSTRUCT_TYPE) return(false); 
				break;
			case output_AD:
				outputAD++;
				if (outputAD >= MAX_INSTRUCTS_PER_INSTRUCT_TYPE) return(false); 
				break;
			case output_digital:
				outputDig++;
				if (outputDig >= MAX_INSTRUCTS_PER_INSTRUCT_TYPE) return(false); 
				break;
		}
		
		actualSamplingRateHz = (actualFastSamplingRateHz/(float)numberOfReadsPerTakenRead);
		actualSamplingIntervalUS = (float)full_time_slice_us * (float)numberOfReadsPerTakenRead;
		if (VERBOSE_IO_DEVICE) {
			mprintf("Setup %d:  requested rate: %d  Hz   actualRate: %f   Hz  actualSamplingInterval: %f us",index, thisSpeed, actualSamplingRateHz,actualSamplingIntervalUS);
		}
		
		// compare what we got with what was requested  ----------------------------------
		// these are part of the object and accesible outside 
		
		// ---------------------------------------------------------------------------------
		// may be asking for a report back on incompatibilites
		//  do not try to access these if they are out of range
		if (checkForIncompatabilities) {
			int listSize = temporaryIncompatabilityList->getNElements();
			if (channel_index >= listSize) {
				merror(M_IODEVICE_MESSAGE_DOMAIN,
					   "ITC18 channel_index is out of range");
				return false;
			}
			
			// save the actual itc computed interval to send back something that can work exactly...
			((temporaryIncompatabilityList->getRawElement(channel_index))->getModifiedRequest())->setRequestedDataIntervalUsec((int)actualSamplingIntervalUS);
			//    ... and determine if we should say it did not work or not
			long requestedSamplingIntervalUS = ((channels_for_instruction_seq->getRawElement(channel_index))->getRequest())->getRequestedDataIntervalUsec();
			float tol = (((float)SAMPLING_INTERVAL_TOLERANCE_PERCENT)/100);
			if (((float)requestedSamplingIntervalUS > (actualSamplingIntervalUS*(1.0+tol))) 
				|| ((float)requestedSamplingIntervalUS < (actualSamplingIntervalUS*(1.0-tol))) ) {
				AllChannelRequestsMetWithinSamplingTolerance = false;
			}
		}
		// ---------------------------------------------------------------------------------
		
		// this field was removed by DDC:
		// implicit relationship of channels and setup lines -- keep track of what the hardware will actually try to do
		//channel = channels->getRawElement(index);
		//channel->setNominalDataIntervalUsec(actualSamplingIntervalUS);		//this is the interval that the ITC thinks that it is using to sample the channel 
        
	}    
	
	
	
	// Make the instruction sequence, allocating memory first if needed
	if (VERBOSE_IO_DEVICE) {
		mprintf("About to make instruction sequence:  number of instructions = %d", numInstr);
	}
	
	// make memory to hold instructions
	if (pInstructions != NULL) {
		delete [] pInstructions;
		pInstructions = NULL;
	}
	pInstructions = new int [numInstr];
	//OLD pInstructions = (int *)NewPtr(numInstr * sizeof(int));			 
	if (pInstructions == NULL) {
		mprintf("ERROR: could not create memory to hold instruction sequence");
	}
	
	// make memory to hold data download from ITC FIFO
	if (pSamples != NULL) {
		delete [] pSamples;
		pSamples = NULL;		
	}
	pSamples = new short [numInstr * 10];
	//OLD pSamples = (short *)NewPtr(numInstr * 10 * sizeof(short));
	if (pSamples == NULL) {
		mprintf("ERROR: could not create memory to hold instruction sequence");
	}
	//OLD instructionBufferLength = numInstr;
	
	
	// open buffer big enough to abosorb bulk FIFO downloads.
	//  (if this turns out to be too small, it will be expanded, but this is time expesive, so err on the side of too big)
	/*
	 int maxAnticipatedNumSets = min(((flushFIFOintervalUS/full_time_slice_us)),1)*10;   // ten times as big as expected
	 pSampleLargeNelements = numInstr * maxAnticipatedNumSets;
	 mprintf("ITC: max anticipated number of samples sets to pull off FIFO (=10x overkill) = %d", pSampleLargeNelements);
	 */
	
	ITC18DriverLock->lock();
	pSampleLargeNelements = ITC18_GetFIFOSize(itc);		// make buffer as large as FIFO so we don't need to worry about size
	ITC18DriverLock->unlock();
	pSamplesLarge = new short [pSampleLargeNelements]; 
	
	// ================================================================       
	
	
	
	
	// NB: No new values are read into the ITC until a command with the ITC18_INPUT_UPDATE bit
	// set is read.  This applies to the digital input lines as well as the AD lines.  For this
	// reason, every digital input command must have the update bit set.   FURTHERMORE, it is
	// essential that none of the AD read commands does an update.  If it does, it will cause
	// the digital values to be updated, clearing any latched bits.  When the next digital
	// read command goes, its update will cause the a new digital word to be read, so that any
	// previously latched values that were updated by the Analog read command would be lost.
	
	
	// TO_CHECK:  Dave (Van Aken), please explain the instruction sequence that you are making here.
	
	
	// First Instruction is a combination of all the input and output updates that are necessary
	pInstrBuff = pInstructions;
	
	currentInstruct = 0;
	if (inputAD > 0){
		pInstrBuff[0] = ITC18_INPUT_UPDATE;
	}
	if (outputAD > 0){
		pInstrBuff[0] = pInstrBuff[0] | ITC18_OUTPUT_UPDATE;
	}
	if (inputDig > 0){
		pInstrBuff[0] = ITC18_INPUT_UPDATE;
		pInstrBuff[0] = pInstrBuff[0] | ITC18_INPUT_DIGITAL;
	}
	if (outputDig > 0){
		pInstrBuff[0] = pInstrBuff[0] | ITC18_OUTPUT_DIGITAL1;
	}
	
	// Now the AD Inputs are added to the Instruction Set
    //the Itc18 Inputs
    int		ADIn[] = {ITC18_INPUT_AD0, ITC18_INPUT_AD1, ITC18_INPUT_AD2, 
		ITC18_INPUT_AD3, ITC18_INPUT_AD4, ITC18_INPUT_AD5, 
	ITC18_INPUT_AD6, ITC18_INPUT_AD7};
	
	currentInstruct = 1;
	for (index = 0; index < setupSize; index++){
		int *InstrAD;
		int which_channel;
		
		InstrAD = setup[index];
		if(InstrAD[SETUP_AD_INDEX] == input_AD) {
			which_channel = InstrAD[SETUP_CHAN_INDEX];
			pInstrBuff[currentInstruct] = ADIn[which_channel];
			currentInstruct++;
		}			
	}			
	
	// pad the end with skip instructions       
	while (currentInstruct < numInstr)  {
		pInstrBuff[currentInstruct] = ITC18_INPUT_SKIP;
		currentInstruct++;
	}
	
	/*    
	 // Now the AD Outputs are added to the Instruction Set
	 // (This is not tested at all and is propobably not working (JJD))
	 for (index = 0; index < setupSize; index++){
	 int *InstrOut;
	 int value;
	 InstrOut = setup[index];
	 value = InstrOut[0];
	 if(value == output_AD) {
	 int which_channel = InstrOut[1];
	 //           printf("ADOUT");
	 *pInstrBuff++;
	 *pInstrBuff = *pInstrBuff | ADOut[which_channel];
	 }
	 }
	 
	 //   for (index = 0; index < numInstr; index++){
	 //      	rprintf("Instruction %X\n", pInstrBuff[index]);
	 //    	}  
	 */
	
	// no!  -- do not delete this -- it is kept as part of setup and is deleted there
	//if (thisInstr != NULL) delete thisInstr;
	
	if (VERBOSE_IO_DEVICE) {
		mprintf("Successfully made instruction sequence.");
	}
	
	return(true);			// instruction sequence successfully created.
}


void mITC18_IODevice::makeSetup(ExpandableList<IOChannel> *channels_for_instruction_seq) {
	
	
	// will any existing setup arrays -- they will be created below
	deleteInstructionSeqArrays();   
	
	
	// here, we translate between the list of established "channels"  (i.e. requests that were matched) and a "SETUP" array (the old ITC code)
	// (This is just to translate between the new access methods and the old code -- could be updated at some point, bnut this is already tested)
	
	// for each channel request, create an element of "SETUP"
	
	// create the memory to hold the setup info (the handle is part of this object) 
	int n_elements_of_setup = channels_for_instruction_seq->getNElements();
	setup = new int* [n_elements_of_setup];					 // this is as big, or bigger than we need
	
	
	// make sure all software data buffers do not yet exist, as the relationship with channels is implicit
	
	int *thisInstr = NULL;
	int setupIndex = 0;
	IOChannel_ITC18 *channel;
	for (int channel_index = 0; channel_index < channels_for_instruction_seq->getNElements(); channel_index++){   // each of these is a channel
		
		// get a pointer to the channel objects -- tells what we want to do with this channel
		channel = (IOChannel_ITC18 *)(channels_for_instruction_seq->getRawElement(channel_index));				
		
		// unpack the info about the channel to a form that is useful to the old ITC code ---------------------------					
		if (channel->isInstructionSeqNeeded()) {
			
			// for the ITC, we will assume a one to one mapping between channels and setup lines (OLD code setup lines, e.g. one line for eyeX, etc.)
			// NOTE: this applies to both the instruction sequence set up and the FIFO read-out. (the relationship is IMPLICIT, not explicit)
			
			thisInstr = new int [NUM_ELEMENTS_ON_EACH_SETUP_LINE];		// create memory for (5) ints (original) -- JJD added three new ones
			
			// build the setup values for each channel			
			thisInstr[SETUP_AD_INDEX] = channel->ad_index;
			thisInstr[SETUP_CHAN_INDEX] = channel->getHardwarePort();			// this is not the same as the IO device "channel" index
			thisInstr[SETUP_TYPE_INDEX] = channel->type_index;
			//thisInstr[SETUP_ASSOC_CHAN_INDEX] = channel->assoc_chan_index;
			thisInstr[SETUP_RATE_INDEX] = (short)((1000*1000)/((double)(channel->getRequest())->getRequestedDataIntervalUsec()));		//requestedSamplingFrequencyHZ
			thisInstr[MIODEVICE_CHAN_INDEX] = channel_index;				// can be used to get back to the channel object that caused this setup line, and thus the software buffer
			
			setup[setupIndex] = thisInstr;		// keep the handle for this memory
			setupIndex++;
		}
	}
	setupSize = setupIndex;
	
	
	// ---------------------------------------------------------------------------------------------------------------
	// Now, "SETUP" is established, the OLD ITC18 code follows for creating and loading the instruction sequence ...
	
}


bool mITC18_IODevice::computeNumInstrLines() {
	
	// Count the number of the kinds of inputs, and outputs
	// place a point to each setup in the appropriate array
	int numberSetup = 0;
	int inputAD = 0;
	int outputAD = 0;
	int inputDig = 0;
	int outputDig = 0;
	
	for (int index = 0; index < setupSize; index++){
		int *thisInstr = setup[index];
		int value = thisInstr[SETUP_AD_INDEX];
		
		if (value == input_AD){			
			inputADs[inputAD] = thisInstr;
			inputAD += 1;
			if (inputAD >= MAX_INSTRUCTS_PER_INSTRUCT_TYPE) 
				return(false); 
		}
		if (value == input_digital){
			inputDigs[inputDig] = thisInstr;
			inputDig += 1;
			if (inputDig >= MAX_INSTRUCTS_PER_INSTRUCT_TYPE) return(false);
		}
		if (value == output_AD){
			outputADs[outputAD] = thisInstr;
			outputAD += 1;
			if (outputAD >= MAX_INSTRUCTS_PER_INSTRUCT_TYPE) return(false);
		}
		if (value == output_digital){
			outputDigs[outputDig] = thisInstr;
			outputDig += 1;
			if (outputDig >= MAX_INSTRUCTS_PER_INSTRUCT_TYPE) return(false);
		}
	}
	
	numberSetup = m_max(inputAD, outputAD);		// I guess that outputs and inputs can be in one instruction? == yes per DEVA		
	
	// the following 5 vals are needed later	for parsing	
	numInputAD = inputAD;
	numOutputAD = outputAD;
	numInputDig = inputDig;
	numOutputDig = outputDig;		
	numInstr = numberSetup + 1;					// extra one is for digital
	return true;
}



bool mITC18_IODevice::loadInstructionSequence() {
	
	if (itc == NULL) return false;		// itc should alredy be open and initialized
	
	if (!instructionSequenceIsNeeded) return true;	  // we are successful because we do not need an instruction seq
	
	// setup ranges for any analog channels that asked for them (defaults used if no analog channels) 
	if (!setupADRanges()) {
		merror(M_IODEVICE_MESSAGE_DOMAIN,
			   "ITC18 ERROR:initializeChannels: failed to specify AD input ranges.");
		return false;
	}
	
	ITC18DriverLock->lock();
	
	ITC18_SetRange(itc, inputRangesOnADhardwarePorts);	
	
	// Set to latch the digital input.  
	//  Latching will assure that even a very brief pulse will be detected 
	//   (i.e. if the digital input line ever goes high, the digital value goes high and stays high until read, even if the digital input line has returned low before the read).
	//  Latching is not the same thing as edge triggering!!!  If the input line is still high after the read, it will be high at the next read!
	//  That is, a short pulse will produce a positive value at the next read, but a steady level can also produce a series of positive values.	
	ITC18_SetDigitalInputMode(itc, true, false);     // Latching is on, inverted is off (same as JHRM version)
	
	if (VERBOSE_IO_DEVICE) {
		mprintf("Loading instruction sequence...");
		for (int i = 0; i<numInstr; i++) {
			mprintf(" %d  %d", i, pInstructions[i]);
		}
	}
	
	// Load the ITC18 with the correct sequence for the sampling periods that have been requested	
	ITC18_SetSequence(itc, numInstr, pInstructions); 		// here is where the instruction sequence is set.
	
	ITC18DriverLock->unlock();
	
	return(true);
	
}

void mITC18_IODevice::fullFlushITC18FIFO(void) {
	// check if ready to pull data off the FIFO (most of these requests will probably be false)
	// if data does come off the FIFO, it will be immediately parsed to the software buffers
	// timing is computed in here and adjustments made based on the global clock start time
	
	//while (flushITC18FIFO()) {};	// keep getting data off the FIFO until no more to get
	// Oct 11, 2006 -- changed this method to get all the data off in one call
	flushITC18FIFO_all();  
}

// method to flush ITC18 FIFO buffer and parse ITC18 FIFO data to software buffers based on current instruction sequence and channels
// Each call will pull down all data (up to round of instruction set).
bool mITC18_IODevice::flushITC18FIFO_all(void) {
	
	short index;
	int 	available = 0;
	short 	digitalData = 0x0000;		 
	int		status;
	
	if (VERBOSE_IO_DEVICE > 2) mprintf("FIFO flush -- called");		
	
	
	// If the ITC is not initialized or does not exist, there is nothing to do
	if (itc == NULL) {
		return(false);
	}
	if (!fullyInitialized) return false;
	
	//If there is ITC hardware, we must provide some minimal servicing whether or not we are 
	// using it for samples.  When a sequence is started, the first three entries in the FIFO 
	// are garbage.  They should be thrown out.  
	ITC18DriverLock->lock();
	
	if (justStartedITC18) {
		if (VERBOSE_IO_DEVICE >= 1) mprintf("FIFO flush called, checking for garbarge data");
		ITC18_GetFIFOReadAvailable(itc, &available);
		if (available < GARBAGE_LENGTH + 1) {
			ITC18DriverLock->unlock();
			return(false);
		}
		ITC18_ReadFIFO(itc, GARBAGE_LENGTH, pSamples);  
		if (VERBOSE_IO_DEVICE >= 1) mprintf("FIFO flush called, removing garbarge data!");
		justStartedITC18 = false;
	}
	
	// see how much data is available on the FIFO
	ITC18_GetFIFOReadAvailable(itc, &available);
	
	int sets = (available-1) / numInstr;		// subtract 1 so we do not every fully empty the FIFO
	//fprintf(stderr,"FIFO flush: %d avial %d numInstr --> %d sets available \n", available, numInstr, sets);
	//fflush(stderr);
	
	// If no data are ready, there is nothing to do. (this may often be the case because we may decide to call this routine (e.g.) twice as fast as needed)
	if (sets == 0) {
		if (VERBOSE_IO_DEVICE >= 1) mprintf("FIFO flush called, but not enough data ready -- returning");
		ITC18DriverLock->unlock();
		return(false);
	}
	
	//if (VERBOSE_IO_DEVICE) mprintf("FIFO flush -- new data is avaialble in the FIFO");		
	
	// readbiggest available chunk of data all at once
	status = ITC18_ReadFIFO(itc, numInstr*sets, pSamplesLarge);
	shared_ptr<Clock> clock = Clock::instance();
	MonkeyWorksTime now = clock->getCurrentTimeUS();
	//mprintf("FIFO flushed %d sets", sets);
	ITC18DriverLock->unlock();
	if (VERBOSE_IO_DEVICE > 2) mprintf("FIFO flush -- data has been read off ITC,  numInstr=%d  status=%d",numInstr, status);
	
	
	int digitalEdgeHardwarePort, channel_index;
	bool digitalValue;
	int *SetupLine;
	short analogSample;
	
	
	
	
	
	// process each set
	for (int set = 0; set < sets; set++) {	
		pSamples = pSamplesLarge + (numInstr*set);  // point to start of each set	
		
		// time is incremented by the time taken to complete a full instruction sequence
		//   (note, full_time_slice_us may be a non-integral value, so we now keep track of num slices)
		num_FIFO_clears_since_itc_started++;
		time_since_inst_seq_started_US = (MonkeyWorksTime)(round(((double)num_FIFO_clears_since_itc_started)*full_time_slice_us*clockDriftAdjustFactor)) + fudgeTime;		
		absoluteTimeUSbasedOnITCclock = globalClockTimeAtStartUS + time_since_inst_seq_started_US;
		
		
		// the form of the data off the FIFO are:
		// pSamples[0] = bitwise contents of digital registers
		// pSamples[1] = value on user's first AD channel
		// pSamples[2] = value on user's second AD channel
		// pSamples[3] = ...
		//mprintf("FIFO flush: pSamples[0]:%d [1]:%d [2]%d",pSamples[0],pSamples[1],pSamples[2]);
		
		
		// process the digital word
		IOChannel_ITC18_TTL *chan;
		digitalData = pSamples[0];
		for (index = 0; index < numInputDig; index++){
			// old code
			//parseDigital(digitalData, inputDigs[index],index);		
			
			SetupLine = inputDigs[index];
			digitalEdgeHardwarePort = SetupLine[SETUP_CHAN_INDEX];                  // find the hardware port to check
			digitalValue = digitalData & (DIGITAL_BIT << digitalEdgeHardwarePort);	// get value for this port out of the digital word  		
			
			// pass the digital value to the appropriate channel for processing (may be ignored, buffered, etc.)
			channel_index = SetupLine[MIODEVICE_CHAN_INDEX];				
			chan = (IOChannel_ITC18_TTL *)(channels->getRawElement(channel_index));
			chan->newSample(digitalValue, absoluteTimeUSbasedOnITCclock); 
		}
		
		// Parse the analog inputs
		// skipInputAD[index].mod may be more than 1 if we are discarding some analog data (i.e. downsampling)
		IOChannel_ITC18_ADC *chanA;
		for (index = 0; index < numInputAD; index++){
            
			analogSample = pSamples[1 + index];
			
			SetupLine = inputADs[index];
			channel_index = SetupLine[MIODEVICE_CHAN_INDEX];
			chanA = (IOChannel_ITC18_ADC *)(channels->getRawElement(channel_index));
			chanA->newSample(analogSample, absoluteTimeUSbasedOnITCclock);
            
		}
		
		
		
	}        // go parse next set
	
	// check on time drift:
	// note that this is cumulative, so if you run things for a long time, and there is a slight discrepancy, you are more likely 
	// to get an error (as you should be)
	// a fancier version should adjust the ongoing time stamps, but this might be a bit tricky
	// since we just read off everything, the last data are fresh, so their time should match up well with the global time
	
	if (absoluteTimeUSbasedOnITCclock >= nextCheckTimeUS) {
		
		// DDC edit: put the clock comparison times closer
		//    "now" is set immediately after the fifo read
		MonkeyWorksTime diff = (absoluteTimeUSbasedOnITCclock - now );  		
		
		if (VERBOSE_IO_DEVICE > 1) {
			shared_ptr<Clock> clock = Clock::instance();
			mprintf("ITC clock check engaged at global time = %d ms", (long)(clock->getCurrentTimeMS()) );
			mprintf("Diff between ITC clock and global clock = %d us (+ means itc is leading) time slice = %d us", (long)diff, (long)full_time_slice_us);
		}
		// this is approximately our resolution to tell if there is a problem.
		float drift_percent_of_sample_duration = (abs((long)diff))/((long)full_time_slice_us);
		if ( drift_percent_of_sample_duration > MAX_ALLOWABLE_DRIFT_FRACTION_OF_SAMPLE_DURATION) {
			if (!alreadyWarnedAboutDrift) {
				mwarning(M_IODEVICE_MESSAGE_DOMAIN,
						 "Warning: IODevice: ITC clock differs from system clock by more than %f3 percent of sample duration (diff=%d us; + means itc is leading)",
						 (100.*MAX_ALLOWABLE_DRIFT_FRACTION_OF_SAMPLE_DURATION), (long)diff);
				//alreadyWarnedAboutDrift = true;
			}
		}	
		nextCheckTimeUS = (MonkeyWorksTime)(round(absoluteTimeUSbasedOnITCclock + (INTERVAL_TO_CHECK_CLOCK_DRIFT_MS*1000)));
	}
	
	return(true);
	
}


/*                    	
 
 // method to flush ITC18 FIFO buffer and parse ITC18 FIFO data to software buffers based on current instruction sequence and channels
 // Each call will pull down (at most) one full set of data resulting from the instruction sequence
 // Thus, to fully clear the FIFO of data, must call until returned false.
 bool mITC18_IODevice::flushITC18FIFO(void) {
 
 short index;
 int 	available;
 short 	digitalData = 0x0000;		 
 int		status;
 
 if (VERBOSE_IO_DEVICE >= 1) mprintf("FIFO flush -- called");		
 
 // If the ITC is not initialized or does not exist, there is nothing to do
 if (itc == NULL) {
 return(false);
 }
 if (!fullyInitialized) return false;
 
 //If there is ITC hardware, we must provide some minimal servicing whether or not we are 
 // using it for samples.  When a sequence is started, the first three entries in the FIFO 
 // are garbage.  They should be thrown out.  
 ITC18DriverLock->lock();
 
 if (justStartedITC18) {
 if (VERBOSE_IO_DEVICE >= 1) mprintf("FIFO flush called, checking for garbarge data");
 ITC18_GetFIFOReadAvailable(itc, &available);
 if (available < GARBAGE_LENGTH + 1) {
 ITC18DriverLock->unlock();
 return(false);
 }
 ITC18_ReadFIFO(itc, GARBAGE_LENGTH, pSamples);  
 if (VERBOSE_IO_DEVICE >= 1) mprintf("FIFO flush called, removing garbarge data!");
 justStartedITC18 = false;
 }
 
 // If no data are ready, there is nothing to do. (this will often be the case because we call this routine twice as fast as needed)
 ITC18_GetFIFOReadAvailable(itc, &available);
 if (available < (numInstr + 1)) {
 if (VERBOSE_IO_DEVICE >= 1) mprintf("FIFO flush called, but not enough data ready -- returning");
 ITC18DriverLock->unlock();
 return(false);
 }
 
 
 //if (VERBOSE_IO_DEVICE) mprintf("FIFO flush -- new data is avaialble in the FIFO");		
 
 // this next call justs get a number of values equal to the numInstr (number of instructions)
 // (if there are still values in there after this this, we will get them on the next call to this function)
 
 // Data are ready, move (n=numInstr) values from the FIFO into the pSamples array (this will remove the values from the FIFO)
 status = ITC18_ReadFIFO(itc, numInstr, pSamples);
 ITC18DriverLock->unlock();
 if (VERBOSE_IO_DEVICE >= 1) mprintf("FIFO flush -- data has been read off ITC,  numInstr=%d  status=%d",numInstr, status);
 
 // time is incremented by the time taken to complete a full instruction sequence
 //   (note, full_time_slice_us may be a non-integral value, so we now keep track of num slices)
 num_FIFO_clears_since_itc_started++;
 time_since_inst_seq_started_US = (MonkeyWorksTime)(round(((double)num_FIFO_clears_since_itc_started)*full_time_slice_us*clockDriftAdjustFactor)) + fudgeTime;		
 absoluteTimeUSbasedOnITCclock = globalClockTimeAtStartUS + time_since_inst_seq_started_US;
 
 
 // the form of the data off the FIFO are:
 // pSamples[0] = bitwise contents of digital registers
 // pSamples[1] = value on user's first AD channel
 // pSamples[2] = value on user's second AD channel
 // pSamples[3] = ...
 //mprintf("FIFO flush: pSamples[0]:%d [1]:%d [2]%d",pSamples[0],pSamples[1],pSamples[2]);
 
 
 // All of the digital Inputs are parsed (note: we just ignore some values if the sampling rate is less than max)
 digitalData = pSamples[0];
 
 // *** note:  you cannot skip digital inputs if you are using latching and looking for rising edges (the current setup)
 // (this is because each of the reads will clear the latch and you will just not see some of the rises)
 // thus skipInputDig[index].mod should be equal to 1 (i.e. parse every digital input)
 //	However, we can just drop the digital parsing and rely on the waveform events for both spike times and waveforms
 for (index = 0; index < numInputDig; index++){
 if (skipInputDig[index].count == 0) parseDigital(digitalData, inputDigs[index]);
 skipInputDig[index].count = (skipInputDig[index].count + 1) % skipInputDig[index].mod;
 }     
 
 
 
 // Parse the analog inputs
 // skipInputAD[index].mod may be more than 1 if we are discarding some analog data (i.e. downsampling)
 for (index = 0; index < numInputAD; index++){
 if (skipInputAD[index].count == 0) {
 float actualSamplingIntervalUS = full_time_slice_us*(float)skipInputAD[index].mod;
 parseAD(pSamples[1 + index], inputADs[index], digitalData, actualSamplingIntervalUS);
 }
 skipInputAD[index].count = (skipInputAD[index].count + 1) % skipInputAD[index].mod;
 }
 
 // note that this is cumulative, so if you run things for a long time, and there is a slight discrepancy, you are more likely 
 // to get an error (as you should be)
 // a fancier version should adjust the ongoing time stamps, but this might be a bit tricky
 ITC18DriverLock->lock();
 if (absoluteTimeUSbasedOnITCclock >= nextCheckTimeUS) {
 ITC18_GetFIFOReadAvailable(itc, &available);
 if (available < (numInstr + 1)) {   // nothing left on itc, thus the current data are fresh!
 MonkeyWorksTime diff = (absoluteTimeUSbasedOnITCclock - clock->getCurrentTimeUS() );
 if (VERBOSE_IO_DEVICE > 1) {
 mprintf("ITC clock check engaged at global time = %d ms", (long)(clock->getCurrentTimeMS()) );
 mprintf("Diff between ITC clock and global clock = %d us (+ means itc is leading) time slice = %d us", (long)diff, (long)full_time_slice_us);
 }
 // this is approximately our resolution to tell if there is a problem.
 float drift_percent_of_sample_duration = (abs((long)diff))/((long)full_time_slice_us);
 if ( drift_percent_of_sample_duration > MAX_ALLOWABLE_DRIFT_FRACTION_OF_SAMPLE_DURATION) {
 if (!alreadyWarnedAboutDrift) {
 mwarning(M_IODEVICE_MESSAGE_DOMAIN,
 "Warning: IODevice::flushITC18FIFO:  ITC clock leads system clock by more than %f3.1 percent of sample duration (drift = %d us)",
 (100.*MAX_ALLOWABLE_DRIFT_FRACTION_OF_SAMPLE_DURATION), (long)diff);
 //alreadyWarnedAboutDrift = true;
 }
 }	
 nextCheckTimeUS = (MonkeyWorksTime)(round(absoluteTimeUSbasedOnITCclock + (INTERVAL_TO_CHECK_CLOCK_DRIFT_MS*1000)));
 }
 }
 ITC18DriverLock->unlock();
 
 return(true);
 
 }
 
 
 */



// override base class method so that we can keep more details about what we need for our ITC channels 
IOChannel* mITC18_IODevice::makeNewChannel(IOChannelRequest* _request, IOCapability* _capability) {
	shared_ptr<mITC18_IODevice> this_one = shared_from_this();	
	
    // base class first
    IOChannel_ITC18 tentative_channel(_request, _capability, this_one);	// keep the device also
    ITC18DataType itcDataType = this->computeITCDataType(&tentative_channel);
	
    
    // build a subclassed ITC channel depending on the type of behavior your need
    IOChannel* chan;
	
    switch (itcDataType) {
			
        case ITC_ADC_INPUT_CONTINUOUS_TYPE:	
            if (VERBOSE_IO_DEVICE) mprintf("Creating an ITC18 channel for ADC continuous input");
            chan = new IOChannel_ITC18_ADC(_request, _capability, this_one);	   
            break;
			
		case ITC_ADC_INPUT_WAVEFORM_TYPE:
            if (VERBOSE_IO_DEVICE) mprintf("Creating an ITC18 channel for ADC waveform input");
            chan = new IOChannel_ITC18_ADC_waveform((IOChannelRequest_TriggeredAnalogSnippetITC18 *)_request, _capability, this_one);	         
            break;
            
		case ITC_TTL_INPUT_CONTINUOUS_TYPE:
            if (VERBOSE_IO_DEVICE) mprintf("Creating an ITC18 channel for TTL continuous input");
            chan = new IOChannel_ITC18_TTL(_request, _capability, this_one);	
            break;
			
		case ITC_TTL_INPUT_EDGE_LOW_TO_HIGH_TYPE:
		case ITC_TTL_INPUT_EDGE_HIGH_TO_LOW_TYPE:
		case ITC_TTL_INPUT_EDGE_ANY_TYPE:
            if (VERBOSE_IO_DEVICE) mprintf("Creating an ITC18 channel for TTL edge input");
            chan = new IOChannel_ITC18_TTL_edge(_request, _capability, this_one);	         
            break;
            
		case ITC_AUXPORT_ASYCH_OUT_TYPE:
            if (VERBOSE_IO_DEVICE) mprintf("Creating an ITC18 channel for Asych output");
            chan = new IOChannel_ITC18_AsychOut(_request, _capability, this_one);	
            break;
            
		case ITC_AUXPORT_ASYCH_OUT_PULSE_ACTIVE_HIGH: 
            if (VERBOSE_IO_DEVICE) mprintf("Creating an ITC18 channel for PULSED HIGH Asych output");
            chan = new IOChannel_ITC18_AsychOut_pulsed(_request, _capability, this_one, true);	
            break;
		case ITC_AUXPORT_ASYCH_OUT_PULSE_ACTIVE_LOW:
            if (VERBOSE_IO_DEVICE) mprintf("Creating an ITC18 channel for PULSED LOW Asych output");
            chan = new IOChannel_ITC18_AsychOut_pulsed(_request, _capability, this_one, false);	
            break;
            
		case ITC_AUXPORT_ASYCH_OUT_WORD_TYPE:   // JJD added Nov 7, 2007
            if (VERBOSE_IO_DEVICE) mprintf("Creating an ITC18 channel for Asych output words.");
            chan = new IOChannel_ITC18_AsychOutWord(_request, _capability, this_one);	
            break;			
			
		case ITC_UNKNOWN_TYPE:
		default:
            mwarning(M_IODEVICE_MESSAGE_DOMAIN,"ERROR: mITC18_IODevice::makeNewChannel:  itc channel type is unknown. ");
			
            break;
			
	}
    
	IOChannel* ch = dynamic_cast<IOChannel *>(chan);
	return (ch);
}


// method to check all existing channels and return a pointer to the one using a digital edge on the indicated hardware port
// return NULL if no such channel exists

IOChannel_ITC18_TTL_edge  *mITC18_IODevice::findEdgeChannelUsingTTLport(ExpandableList<IOChannel> *channelListToCheck, int TTLhardwarePortNumber) {
    
    IOChannel_ITC18 *chan;
    IOChannel_ITC18_TTL_edge *edgeChannelToTriggerWaveform;
	
    if (VERBOSE_IO_DEVICE) mprintf("findEdgeChannelUsingTTLport");    
    for (int i=0; i<(channelListToCheck->getNElements()); i++) {
        chan = (IOChannel_ITC18 *)(channelListToCheck->getRawElement(i));
        
        if (VERBOSE_IO_DEVICE) mprintf("findEdgeChannelUsingTTLport:  edge?: %d  port: %d  Looking for port: %d", chan->isEdgeChannel(), chan->getHardwarePort(), TTLhardwarePortNumber);
        // check if this channel is a TTL edge channel attached to the indicated hardware port
        if ( (chan->isEdgeChannel()) && (chan->getHardwarePort() == TTLhardwarePortNumber)) {
            edgeChannelToTriggerWaveform = (IOChannel_ITC18_TTL_edge *)chan;
            return edgeChannelToTriggerWaveform;
        }
    }
    return NULL;
}

// =============================================================


IOChannel_ITC18::IOChannel_ITC18(IOChannelRequest * _request, IOCapability * _capability, shared_ptr<mITC18_IODevice> _device) : IOChannel(_request,_capability) {
	
	this_device = _device;
	itcDataType = this_device->computeITCDataType(this);
	hardwarePort = this_device->setupHardwarePort(this);
	edgeChannel = false;
	
	//defaults that will protect the device (i.e. not run, no data, etc.)
	schedulable = false;
	instructionSeqNeeded = false;
	ad_index = -1;	//	this MUST be defined for all caps that use an instruciton seq line
	type_index = -1; //	this MUST be defined for all caps that use an instruciton seq line
	//assoc_chan_index = NO_CHANNEL;
	
	reportNow = true;       // debugging only
	
}

IOChannel_ITC18::~IOChannel_ITC18() {};


void IOChannel_ITC18::stopChannelIO() {
	// the flushChannel method is locked, so do not lock here
	flushChannel();		// will post all data (if an input channel)
}


bool IOChannel_ITC18::validate(IOChannelIncompatibility *  incompatibility) {
	
    bool success = true;
    if (itcDataType == ITC_UNKNOWN_TYPE) {
        incompatibility->addIncompatibilityType(M_INCORRECT_DATA_TYPE);
        success = false;
    }    
    return success;
}

// ===================================================================

IOChannel_ITC18_input::IOChannel_ITC18_input(IOChannelRequest * _request, IOCapability * _capability, shared_ptr<mITC18_IODevice> _device) : IOChannel_ITC18(_request,_capability,_device) {
	
	schedulable = true;
	instructionSeqNeeded = true;
	
	buffer = NULL;
	
	// buffers
	if (VERBOSE_IO_DEVICE > 1)  mprintf("Instantiating an IOChannel_ITC18_input object.  buffer size chosen = %d",computeBufferSize());
	
	buffer = new ShortDataTimeStampedRingBuffer(computeBufferSize());
	bufferReader = shared_ptr<ShortDataTimeStampedBufferReader >(new ShortDataTimeStampedBufferReader(buffer));
	
	// skipping
	skipInput.count = 0;
	skipInput.mod = 1;
	
}

IOChannel_ITC18_input::~IOChannel_ITC18_input() {
	if (buffer != NULL) delete buffer;
}

void IOChannel_ITC18_input::setSkippingMod(int numberOfReadsPerSavedRead) {
	if (numberOfReadsPerSavedRead > 0) skipInput.mod = numberOfReadsPerSavedRead;
}

void IOChannel_ITC18_input::setup(shared_ptr<mITC18_IODevice> _device, int chan_index) {
	IOChannel_ITC18::setup(_device, chan_index);
	clearBuffer();
}

void IOChannel_ITC18_input::bufferDatum(short data, MonkeyWorksTime timeUS) {
	
	if ((buffer == NULL) || (bufferReader == NULL))  {
		merror(M_IODEVICE_MESSAGE_DOMAIN,"ITC18 channel: analog software buffer is NULL.");
	}
	buffer->putData(data, timeUS);
	
	lock();
	timeOfDataElementAtBufferWriteHeadUS = timeUS;
	unlock();
}

void IOChannel_ITC18_input::clearBuffer() {
	if (bufferReader != NULL) bufferReader->advance(bufferReader->getNItemsUnserviced());
}

int IOChannel_ITC18_input::computeBufferSize() {
    
	int buffer_size = MIN_BUFFER_SIZE_SAMPLES;
	
	// DDC: accesss to the request is okay from a thread safety point of view because the access is read-only
	float samples_per_update =  ((float)((this->getRequest())->getRequestedUpdateIntervalUsec())) / ((float)(this->getRequest())->getRequestedDataIntervalUsec());
	float updates_per_ms = 1000. / ((this->getRequest())->getRequestedUpdateIntervalUsec());
	int  ideal_buffer_size = round(samples_per_update*updates_per_ms*MAX_EXPECTED_BUFFER_TIME_MS);
	buffer_size = m_max(ideal_buffer_size, MIN_BUFFER_SIZE_SAMPLES);
	//if (VERBOSE_IO_DEVICE) mprintf("ITC input:  software buffer size: ideal = %d samples  actual = %d samples", ideal_buffer_size, buffer_size);
	
	return (buffer_size);
}

// post all the data
int IOChannel_ITC18_input::flushChannel() {
	
	//lock();			// JJD CHAN LOCK;  DDC: not necessary
	
	if ((buffer == NULL) || (bufferReader == NULL))  {
		merror(M_IODEVICE_MESSAGE_DOMAIN,"ITC18 channel: analog software buffer is NULL.");
    }		
	
    //short data;
    short ndata;
    int posted = 0;
    
    double fr = bufferReader->getFillRatio();
    if (VERBOSE_IO_DEVICE > 2) mprintf("ITC18 channel:flushChannel: software buffer is now %f percent full",(fr*100));
    if (fr > 0.5) {
        mwarning(M_IODEVICE_MESSAGE_DOMAIN,
				 "ITC18 channel: analog software buffer is more than %f percent full",(fr*100));
    }			
	
	ndata = bufferReader->getNItemsUnserviced();
	if (VERBOSE_IO_DEVICE > 2) mprintf("ITC18 channel:flushChannel: number of data elements in software buffer that will be posted is %d", ndata);
	if ((STALE_DATA_WARN) && (ndata >= STALE_DATA_WARN_NDATA)) {
		mprintf("ITC channel:  high level of unserviced  data in software buffer = %d data", ndata);
	}
	
    while ((bufferReader->getNItemsUnserviced()) > 0) {
		
		
        postDataElement(bufferReader->getData(),bufferReader->getTime());   // depends on channel
        
        bufferReader->advance();	
        posted++;
		
        //if (VERBOSE_IO_DEVICE > 1) mprintf("Flush software buffer: Moved one data off software buf, buffer time MS = %d ms  data = %f V",timeMS,dataV);
		
    }
	//unlock();			// JJD CHAN LOCK; DDC: not necessary
    return (posted);
}


// time is supposed to be the time since the ITC started  
void IOChannel_ITC18_input::newSample(short analogSample, MonkeyWorksTime time) {
	
	//lock();			// JJD CHAN LOCK
    lock();
	int count = skipInput.count;
	unlock();
	
	if (count == 0) {
        bufferDatum(analogSample,time); 
        //float actualSamplingIntervalUS = full_time_slice_us*(float)skipInputAD[index].mod;
    }
	
	lock();
    skipInput.count = (skipInput.count + 1) % skipInput.mod;
	unlock();
	
	//unlock();		// JJD CHAN LOCK
}

// time is supposed to be absolute time 
void IOChannel_ITC18_input::newSample(bool thisDigitalCheck, MonkeyWorksTime time) {
	
    // downsample if needed
	lock();			// JJD CHAN LOCK
	int count = skipInput.count;
	unlock();
	
    if (count == 0){ 
		bufferDatum((short)thisDigitalCheck,time);
	}
    
	lock();
	skipInput.count = (skipInput.count + 1) % skipInput.mod;
	unlock();		// JJD CHAN LOCK
	
}

// generic version
void IOChannel_ITC18_input::postDataElement(short stemp, MonkeyWorksTime timeUS) {  // generic version
    long timeMS = (long)(timeUS/1000.);
    double data = (double)stemp;
    if (VERBOSE_IO_DEVICE_DATA) mprintf("ITC18 channel:flushChannel: generic value about to be posted. Value = %d  Posted time = %d ms",stemp,timeMS);
    update((Data)data,timeUS);		// Base class function (post the data to the event stream)
}








// ======================================================




IOChannel_ITC18_ADC::IOChannel_ITC18_ADC(IOChannelRequest * _request, IOCapability * _capability, shared_ptr<mITC18_IODevice> _device) : IOChannel_ITC18_input(_request,_capability,_device) {
	
    // instruction seq info 
    ad_index = input_AD;
	
    // Analog range
    itc_range_tag = this_device->getAnalogInputRangeTag( (this->getRequest())->getRequestedRangeMax() );
    if (itc_range_tag < 0) {
        merror(M_IODEVICE_MESSAGE_DOMAIN,
			   "failed to get itc range tag for analog channel.");
    }
    else {
        multiplierToGetMV = this_device->getMultiplierToGetMV(itc_range_tag);
    }
	
    lastClippingWarnTimeMS = 0;
	
}



bool IOChannel_ITC18_ADC::validate(IOChannelIncompatibility *  incompatibility) {
	
    bool success = IOChannel_ITC18_input::validate(incompatibility); // parent class version
    
    // 1) high and low range on analog input requests must be same and either 1, 2, 5, or 10
    double req_min = (this->getRequest())->getRequestedRangeMin();
    double req_max = (this->getRequest())->getRequestedRangeMax();
    
    double maxRangeV = m_max(fabs(req_min),req_max);
    if (  (((-1*req_min) != req_max)) || (req_max<0) ||
		( (maxRangeV != VALID_ITC_ANALOG_IN_RANGE_VALUE_10V) && 
		 (maxRangeV != VALID_ITC_ANALOG_IN_RANGE_VALUE_5V) &&
		 (maxRangeV != VALID_ITC_ANALOG_IN_RANGE_VALUE_2V) &&
		 (maxRangeV != VALID_ITC_ANALOG_IN_RANGE_VALUE_1V) )) {
		// not valid, make best guess and set incompt mod request to best guess 
		
		merror(M_IODEVICE_MESSAGE_DOMAIN,"IOChannel_ITC18_ADC::validate.  Requested voltage range is inappropriate.");
		
		
        // bestGuessRange = take highest of the two values, round up to 1,2,3 or 10, and set for both
        double bestGuessRangeMax = maxRangeV;
        if (bestGuessRangeMax > VALID_ITC_ANALOG_IN_RANGE_VALUE_5V) {
            bestGuessRangeMax = VALID_ITC_ANALOG_IN_RANGE_VALUE_10V;
        }
        else if (bestGuessRangeMax > VALID_ITC_ANALOG_IN_RANGE_VALUE_2V) {
            bestGuessRangeMax = VALID_ITC_ANALOG_IN_RANGE_VALUE_5V;
        }
        else if (bestGuessRangeMax > VALID_ITC_ANALOG_IN_RANGE_VALUE_1V) {
            bestGuessRangeMax = VALID_ITC_ANALOG_IN_RANGE_VALUE_2V;
        }
        else {
            bestGuessRangeMax = VALID_ITC_ANALOG_IN_RANGE_VALUE_1V;
        }
		
        // modify the incompatability
        incompatibility->addIncompatibilityType(M_CAPABILITY_NOT_AVAILABLE); 
        (incompatibility->getModifiedRequest())->setRequestedRangeMin(-bestGuessRangeMax);
        (incompatibility->getModifiedRequest())->setRequestedRangeMax(bestGuessRangeMax);
        success = false;
    }
	
    // 2) resolution on analog chans must be 16 bit (only option)
    if (((this->getRequest())->getRequestedResolution()) != VALID_ITC_ANALOG_IN_RESOLUTION_BITS) {
		
        merror(M_IODEVICE_MESSAGE_DOMAIN,"IOChannel_ITC18_ADC::validate.  Requested resolution is inappropriate.");
		
        (incompatibility)->addIncompatibilityType(M_CAPABILITY_NOT_AVAILABLE);  
        (incompatibility->getModifiedRequest())->
		setRequestedResolution(VALID_ITC_ANALOG_IN_RESOLUTION_BITS);
        success = false;
    }
    
    return (success);
}


// analog version
void IOChannel_ITC18_ADC::postDataElement(short stemp, MonkeyWorksTime timeUS) {
	
    long timeMS = (long)(timeUS/1000.);
    double dataV = (multiplierToGetMV*((double)(stemp)))/1000.;	// output in volts
	
    if (VERBOSE_IO_DEVICE_DATA) mprintf("ITC18 channel:flushChannel: value about to be posted from ADC hardware port: %d   Value: %d ITCunits = %f volts.  Posted time: %d ms", getHardwarePort(), stemp, dataV, timeMS);
    this->update((Data)dataV,timeUS);		// Base class function (post the data to the event stream)
	
    // Warn for clipping on analog channels
	shared_ptr<Clock> clock = Clock::instance();
    MonkeyWorksTime timeSinceLastWarnMS = clock->getCurrentTimeMS() - lastClippingWarnTimeMS;
    if ((abs(stemp) > 32000) && (timeSinceLastWarnMS > 1000)) {
        lastClippingWarnTimeMS = clock->getCurrentTimeMS();
        mwarning(M_IODEVICE_MESSAGE_DOMAIN,"ITC channel on ADC port %d is probably clipping. Value = %f volts.  We suggest a larger range on your channel request or a lower gain on your input signal.",getHardwarePort(),dataV);
    }
}



// ============================================================================




IOChannel_ITC18_ADC_waveform::IOChannel_ITC18_ADC_waveform(IOChannelRequest_TriggeredAnalogSnippetITC18* _request, IOCapability * _capability,shared_ptr<mITC18_IODevice> _device) : IOChannel_ITC18_ADC(_request,_capability,_device) {
    
    // todo -- these should be part of the channel request -- here jsut defined for testing
    //long requested_preSpikeWindowTimeUS = 3000;
    //long requested_postSpikeWindowTimeUS = 5000;
    //int requested_linkedTTLport = 0;        // makey this shoul dbe the entire request -- starts to interact with how this looks in the XML
    // =================
    
    preSpikeWindowTimeUS = _request->getRequestedPreSpikeWindow();          // parameter -- include data from up to this many US BEFORE the digital edge
    postSpikeWindowTimeUS = _request->getRequestedPostSpikeWindow();       // after the digital edge
    linkedTTLport = _request->getRequestedLinkedDigitalPort();
	
    //activeWaveforms = new LinkedList<mWaveform>();
	
	// upgrade the buffers (need bigger) -- NO!  The buffer size is really only a function of sampling and the time it takes to service the buffer (update interval plus possible scheduler delays)
	/*
	 if (buffer != NULL) delete buffer;
	 if (bufferReader != NULL) delete bufferReader;
	 buffer = new ShortDataTimeStampedRingBuffer(computeBufferSize());
	 bufferReader = new ShortDataTimeStampedBufferReader(buffer);
	 */
	
	if (VERBOSE_IO_DEVICE > 1)  mprintf("Instantiating an IOChannel_ITC18_ADC_waveform object.");
	
}

IOChannel_ITC18_ADC_waveform::~IOChannel_ITC18_ADC_waveform() {
    //delete activeWaveforms;
}





void IOChannel_ITC18_ADC_waveform::setup(shared_ptr<mITC18_IODevice> _device, int chan_index) {
	
    IOChannel_ITC18_ADC::setup(_device, chan_index);  // run stuff in parent setup
    
    // find the edge channel that the user would like to trigger waveforms on this channel and establish a link
    IOChannel_ITC18_TTL_edge *edgeChannelToTriggerWaveform = _device->findEdgeChannelUsingTTLport(_device->getChannels(),linkedTTLport);  // inked TTL port comes from the user request
    
    // link to the edge channel (i.e. tell the edge channel to notify this cahnnel when it gets an edge)
    if (edgeChannelToTriggerWaveform != NULL) {
        edgeChannelToTriggerWaveform->linkToWaveformChannel(this);
    }
    else {
        mwarning(M_IODEVICE_MESSAGE_DOMAIN,
				 "ITC18 waveform channel could not link to a digital channel because no digitial channel on requested TTL port exists -- please add one to the experiment.  For now, no waveforms will be captured.");
    }
	
	if (VERBOSE_IO_DEVICE) mprintf("A waveform channel (ADC port %d) has just been setup.", getHardwarePort());  
	
    
}


// start tracking a waveform
void IOChannel_ITC18_ADC_waveform::startNewWaveform(MonkeyWorksTime spikeTimeUS) {
	
	//lock();			// JJD CHAN LOCK 
    // make a new one an put it on the linked list
	
    shared_ptr<mWaveform> waveform(new mWaveform(spikeTimeUS,preSpikeWindowTimeUS, postSpikeWindowTimeUS, request->getRequestedDataIntervalUsec(),linkedTTLport)); 
    activeWaveforms.addToFront(waveform);
	
	if (VERBOSE_IO_DEVICE) {
		shared_ptr<Clock> clock = Clock::instance();
		mprintf("A waveform has just been started on (port %d). CurrentTime = %d ms ", getHardwarePort(), (long)clock->getCurrentTimeMS());  
	}
	//unlock();		// JJD CHAN LOCK 
}


/*
 // waveform buffer may need to be a bit longer
 int IOChannel_ITC18_ADC_waveform::computeBufferSize() {
 
 int buffer_size = MIN_BUFFER_SIZE_SAMPLES;
 
 
 float samples_per_update =  ((float)((this->getRequest())->getRequestedUpdateIntervalUsec())) / ((float)(this->getRequest())->getRequestedDataIntervalUsec());
 float updates_per_ms = 1000. / ((this->getRequest())->getRequestedUpdateIntervalUsec());
 
 // the max time here depends on the update time of the other channel -- could get more clever here
 long waveformTimeMS = max((round((preSpikeWindowTimeUS + postSpikeWindowTimeUS)/1000)),1);	// convert to MS
 long maxExpectedBufferTimeMS = 10*waveformTimeMS;
 
 int  ideal_buffer_size = round(samples_per_update*updates_per_ms*maxExpectedBufferTimeMS);
 //buffer_size = min( ideal_buffer_size, MAX_BUFFER_SIZE_SAMPLES);
 //buffer_size = max(buffer_size, MIN_BUFFER_SIZE_SAMPLES);
 buffer_size = ideal_buffer_size;
 //if (VERBOSE_IO_DEVICE) mprintf("ITC WAVEFORM input:  software buffer size factors: %f %f %d", samples_per_update,updates_per_ms, maxExpectedBufferTimeMS);
 //if (VERBOSE_IO_DEVICE) mprintf("ITC WAVEFORM input:  software buffer size: ideal = %d samples  actual = %d samples", ideal_buffer_size, buffer_size);
 
 return (buffer_size);
 }
 
 */



// here, the data to be posted are completed waveforms.
// but we take this opportunity to check if any waveforms are complete.
int IOChannel_ITC18_ADC_waveform::flushChannel() {
	
	//lock();			// JJD CHAN LOCK 
	
    MonkeyWorksTime timeUS;
	MonkeyWorksTime headTimeUS;
	long bufferReaderLagTimeUS;
    short stemp;
    //Data wavePackage;
    int nWaveformsPosted = 0;
	
    double fr = bufferReader->getFillRatio();
    if (fr>0.5) mwarning(M_IODEVICE_MESSAGE_DOMAIN,"ITC18 channel:flushChannel: waveform channel main buffer is %f percent full",(fr*100));
    if (bufferReader->getNItemsUnserviced() == 0) {
		//unlock();			// JJD CHAN LOCK 
		return(nWaveformsPosted);   // nothing to post
    }
	
	timeUS = bufferReader->getTime();
	
	// pay attention
	lock();
	headTimeUS = timeOfDataElementAtBufferWriteHeadUS;	// the time of the element placed in the buffer most recently
	unlock();
	
	/*
	 if (VERBOSE_IO_DEVICE) {
	 mprintf("Currently have %d active waveforms on port %d.", activeWaveforms->getNElements(), getHardwarePort()); 
	 timeUS = bufferReader->getTime();
	 mprintf("About to parse waveforms on port %d. The time at the buffer reader = %d ms  time at the buffer head = %d ms Current time: %d ms", getHardwarePort(), (long)(timeUS/1000), (long)(headTimeUS/1000), (long)clock->getCurrentTimeMS()); 
	 }
	 */
	
	
	bufferReaderLagTimeUS = (long)(headTimeUS - timeUS);		// should be positive
    // make sure the reader stays back from the head of the buffer as far as we need in case a spike shows up any time
    while ((bufferReaderLagTimeUS > preSpikeWindowTimeUS) && (bufferReader->getNItemsUnserviced() > 0)) {
		
		//mprintf("WHILE LOOP: bufferReaderLagTime: %d ms  Number of data values still in waveform buffer: %d", (bufferReaderLagTimeUS/1000), bufferReader->getNItemsUnserviced()); 
		
		timeUS = bufferReader->getTime();
		
        // keep advancing the reader, sending data to waveform object when in range
        stemp = bufferReader->getData();
		
		// pay attention
		lock();
        double dataV = (multiplierToGetMV*((double)(stemp)))/1000.;	// output in volts 
        unlock();
		
		//mprintf("WHILE LOOP:  time %d ms  volts = %f", long(timeUS/1000),dataV);
		
		// pay attention
        // check all active waveforms
		activeWaveforms.lock();
        shared_ptr<mWaveform> waveform = activeWaveforms.getFrontmost(); // returns NULL if none
        activeWaveforms.unlock();
		
		//DEBUG ONLY
		//if(waveform != NULL) {
		
		while (waveform != NULL) {
			
			//mprintf("checking ONE waveform:  startTime = %d ms  endTime = %d ms",(long)(waveform->getStartTime()/1000),(long)(waveform->getEndTime()/1000));
			
            if (timeUS > waveform->getEndTime()) {  // waveform is complete
                
                // post the waveform data to the file
				this->update((waveform->getWaveformPackage()),timeUS);  // get all data in a Data and post to channel (base class method)
				
				nWaveformsPosted++;
                if (VERBOSE_IO_DEVICE_DATA) {
					shared_ptr<Clock> clock = Clock::instance();
					mprintf("A waveform channel (ADC port %d) just posted a complete waveform.  current time: %d ms   Waveform end time = %d ms", getHardwarePort(), (long)clock->getCurrentTimeMS(), (long)(waveform->getEndTime()/1000));  
				}
				
                // delete waveform off the list
                shared_ptr<mWaveform> waveformNext = waveform->getNext();  // return NULL when at bottom of list
				waveform->remove();             // remove the object from the linked list
				// delete waveform;                // kill the object
				waveform = waveformNext;        // set the pointer (for while loop)
            } 
			else {
				if (timeUS >= waveform->getStartTime()) { 
					waveform->newData(dataV,timeUS);     // accumulate the data in the waveform object
				}
				waveform = waveform->getNext();  // return NULL when at bottom of list
			}
			
        }
		
		
		
        bufferReaderLagTimeUS = headTimeUS - timeUS; 
        bufferReader->advance();        // note:  non-accumualted data is effectively discarded (no posting)
    }
	
	//unlock();			// JJD CHAN LOCK 
	
    return (nWaveformsPosted);
}


void IOChannel_ITC18_ADC_waveform::stopChannelIO() {
	
	// these methods are both locked, so do not lock here
	flushChannel();		// will clear the buffer and post any waveforms that are complete
	flushWaveforms();      // make sure any waveforms in progress are then posted as they are.
}


// here, the data to be posted are completed waveforms.
// but we take this opportunity to check if any waveforms are complete.
void IOChannel_ITC18_ADC_waveform::flushWaveforms() {
	
    shared_ptr<mWaveform> waveformNext;
    shared_ptr<mWaveform> waveform;
    
	//lock();			// JJD CHAN LOCK
    
	// check all active waveforms
	activeWaveforms.lock();
    waveform = activeWaveforms.getFrontmost(); // returns NULL if none
    activeWaveforms.unlock();
	
    while (waveform != NULL) {
		
		update((waveform->getWaveformPackage()),timeOfDataElementAtBufferWriteHeadUS);  
		
		if (VERBOSE_IO_DEVICE_DATA) {
			shared_ptr<Clock> clock = Clock::instance();
			mprintf("A waveform channel (ADC port %d) just posted a complete waveform.  current time: %d ms   Waveform end time = %d ms", getHardwarePort(), (long)clock->getCurrentTimeMS(), (long)(waveform->getEndTime()/1000));  
        }
		
		// delete waveform off the list
		waveformNext = waveform->getNext();  // return NULL when at bottom of list
		waveform->remove();             // remove the object from the linked list
		//delete waveform;                // kill the object
		waveform = waveformNext;        // set the pointer (for while loop)
    }
	//unlock();			// JJD CHAN LOCK
	
}




// ============================================================================

IOChannel_ITC18_TTL::IOChannel_ITC18_TTL(IOChannelRequest * _request, IOCapability * _capability, shared_ptr<mITC18_IODevice> _device) : IOChannel_ITC18_input(_request,_capability,_device) {
    
	// instruction seq info 
	ad_index = input_digital;
    
	// buffers
	//buffer = new mBoolDataTimeStampedRingBuffer(computeBufferSize());	
	//bufferReader = new mBoolDataTimeStampedBufferReader(digitalBuffer);
}


// digital version
void IOChannel_ITC18_TTL::postDataElement(short stemp, MonkeyWorksTime timeUS) {
    long timeMS = (long)(timeUS/1000.);
    bool dataBoolean = (bool)(stemp);  
    if (VERBOSE_IO_DEVICE_DATA) mprintf("ITC18 channel:flushChannel: value about to be posted from TTL hardward port: %d  Value = %d (digital). Posted time = %d ms", getHardwarePort(), dataBoolean, timeMS);
    update((Data)dataBoolean,timeUS);		// Base class function (post the data to the event stream)
}

bool IOChannel_ITC18_TTL::validate(IOChannelIncompatibility *  incompatibility) {
	
    bool success = IOChannel_ITC18_input::validate(incompatibility); // parent class version
    
    // check digital channels: (not clear we should do anything here, the 
    //      range is implicit in the idea of a digital channel
	
    // resolution 
    if (((this->getRequest())->getRequestedResolution()) != VALID_ITC_DIGITAL_EDGE_IN_RESOLUTION_BITS) {
        (incompatibility)->addIncompatibilityType(M_CAPABILITY_NOT_AVAILABLE);  
        (incompatibility->getModifiedRequest())->
		setRequestedResolution(VALID_ITC_DIGITAL_EDGE_IN_RESOLUTION_BITS);
        success = false;
    }
    
    return (success);
}



// ============================================================================

IOChannel_ITC18_TTL_edge::IOChannel_ITC18_TTL_edge(IOChannelRequest * _request, IOCapability * _capability, shared_ptr<mITC18_IODevice> _device) : IOChannel_ITC18_TTL(_request,_capability,_device) {
	lastDigitalCheck = false;    // for software edge detect
	//linkedWaveformChannels = new ExpandableList<IOChannel_ITC18_ADC_waveform>();
	edgeChannel = true;
}

void IOChannel_ITC18_TTL_edge::setup(shared_ptr<mITC18_IODevice> _device, int chan_index) {
	IOChannel_ITC18_TTL::setup(_device, chan_index);
	lastDigitalCheck = false;    
}

// during setup any waveform channel that uses this digital edge channel, it will use this function 
//   to let the edge channel know that it should be notified when it gets an edge
// (note:  setup is only called once all channels have been created, so the channel creation order does not matter)
// note: more than one wveform channel can be attached to a single TTL edge channel
void IOChannel_ITC18_TTL_edge::linkToWaveformChannel(IOChannel_ITC18_ADC_waveform *_waveformChannel) {
	// TODO: this is dangerous
	shared_ptr<IOChannel_ITC18_ADC_waveform> carrier(_waveformChannel);
	linkedWaveformChannels.addReference(carrier);   
}

void IOChannel_ITC18_TTL_edge::clearAllLinkedChannels() {
	linkedWaveformChannels.clear();
}


// time is supposed to be the time since the ITC started  
void IOChannel_ITC18_TTL_edge::newSample(bool thisDigitalCheck, MonkeyWorksTime time) {
	
	//lock();			// JJD CHAN LOCK
    bool edgeReported = false;   // was anything put in the data stream?
    
    switch(itcDataType) {
        case ITC_TTL_INPUT_EDGE_LOW_TO_HIGH_TYPE:
            if (thisDigitalCheck && (!(lastDigitalCheck))) {
                // rising edge event detected  // this is the absolute time since the ITC started
                edgeReported = true;
                bufferDatum(true,time); 
            }
            break;
            
            
		case ITC_TTL_INPUT_EDGE_HIGH_TO_LOW_TYPE:
            if ((!thisDigitalCheck) && (lastDigitalCheck)) {
                edgeReported = true;
                bufferDatum(false,time);
            }
            break;
            
		case ITC_TTL_INPUT_EDGE_ANY_TYPE:
            if (thisDigitalCheck && (!(lastDigitalCheck))) {
                edgeReported = true;
                bufferDatum(true,time); 
            }
            if ((!thisDigitalCheck) && (lastDigitalCheck)) {
                edgeReported = true;
                bufferDatum(false,time); 	
            }
            break;
		default:
			break;
			
    }
	
	// pay attention
	lock();
    lastDigitalCheck = thisDigitalCheck;
    unlock();
	
    // if any kind of event was issued to the data stream, start a waveform around the time of this event (if there are any linked waveform channels)
    // after this, the job of the edge channel is done (with regard to this waveform)
    if (edgeReported) {
        for (int i=0;i<linkedWaveformChannels.getNElements();i++) {
			// pay attention
            (linkedWaveformChannels.getElement(i))->startNewWaveform(time);  // time of edge (in absolute time since the ITC started)
        }
    }        
	
	//unlock();			// JJD CHAN LOCK 
    
}





// ============================================================================


IOChannel_ITC18_AsychOut::IOChannel_ITC18_AsychOut(IOChannelRequest * _request, IOCapability * _capability, shared_ptr<mITC18_IODevice> _device) : IOChannel_ITC18(_request,_capability,_device) {
	//notification = NULL;
}

IOChannel_ITC18_AsychOut::~IOChannel_ITC18_AsychOut() {
	//if (notification != NULL) delete notification;
}	

// setup is called only when channles are initialized.
void IOChannel_ITC18_AsychOut::setup(shared_ptr<mITC18_IODevice> _device, int chan_index) {
	IOChannel_ITC18::setup(_device, chan_index);       // immediate parent
	setupNotification(_device, chan_index);	
	if(VERBOSE_IO_DEVICE) mprintf(M_IODEVICE_MESSAGE_DOMAIN," Finished setting up an ITC Asych output channel.  Hardware port: %d",getHardwarePort());
}

// create and attach notification object(s) if needed
// this device, channel index is kept
void IOChannel_ITC18_AsychOut::setupNotification(shared_ptr<mITC18_IODevice> _device, int chan_index) {	
	
	
	//if (notification != NULL) delete notification;
	notification = shared_ptr<AsynchronousOutputNotification>(new AsynchronousOutputNotification(_device, chan_index));
	
	// this works by reference, so I need to clear the memory myself at some point
	(this->getVariable())->addNotification(notification);
	if (VERBOSE_IO_DEVICE) mprintf("ITC asych out channel:  Adding notification to variable.");
	
	// this will put a pointer to this object on a list and the notify
	//   method for the object will be run when the variable changes
	//	 effictively, this will make a call the "updateChannel(chan,data)" method in the ITC18 device class
	
	
}

bool IOChannel_ITC18_AsychOut::notify(const Data& data) {
	
	// go update this digital channel (1 bit)
	// TODO -- do some type checking on data at time of channel request checking
	return (setAsychOutputOneBit((bool)data));	// will set this channels hardware port
}

bool IOChannel_ITC18_AsychOutWord::notify(const Data& data) {
	
	// go update this word channel (8 bit)
	// TODO -- do some type checking on data at time of channel request checking
	char desiredWordIn8bitFormat = ((char)((long)data));
	
	return ((this_device)->setAsychOutputWord8(wordPort, desiredWordIn8bitFormat));	 
	// this will only change the bits for the word (the other 8 bits will be left as they are)
}


bool IOChannel_ITC18_AsychOut::setAsychOutputOneBit(bool desiredBit) {
	
    short desiredLinesToSet = pow(2.f,hardwarePort);	// mask revealing only the current port
    if (desiredBit) {
        return(this_device->setAsychLinesHigh(desiredLinesToSet));
    }
    else {
        return( (this_device)->setAsychLinesLow(desiredLinesToSet) );
    }	
}


IOChannel_ITC18_AsychOutWord::IOChannel_ITC18_AsychOutWord(IOChannelRequest * _request, IOCapability * _capability, shared_ptr<mITC18_IODevice> _device) : IOChannel_ITC18_AsychOut(_request,_capability,_device) {
	wordPort = hardwarePort;   // note, hardware port is determined in the mITC18_channel base class from the capability name
}

IOChannel_ITC18_AsychOutWord::~IOChannel_ITC18_AsychOutWord() {
}


// ============================================================================


IOChannel_ITC18_AsychOut_pulsed::IOChannel_ITC18_AsychOut_pulsed(IOChannelRequest * _request, IOCapability * _capability, shared_ptr<mITC18_IODevice> _device, bool _doPulseHigh) : IOChannel_ITC18_AsychOut(_request,_capability,_device) {
	
	// for pulsing
	pulsing = false;
	pulseActiveHigh = false;
	pulseActiveLow = false;
	
	if (_doPulseHigh) {
		pulseActiveHigh = true;
	}
	else {
		pulseActiveLow = true;
	}
	
	if (VERBOSE_IO_DEVICE) mprintf("ITC asych out PULSE channel object was just created.");
	
	
}

IOChannel_ITC18_AsychOut_pulsed::~IOChannel_ITC18_AsychOut_pulsed() {}

void IOChannel_ITC18_AsychOut_pulsed::setup(shared_ptr<mITC18_IODevice> _device, int chan_index) {
	setupPulsing(_device, chan_index);        // scheduling for pulsed channels -- make sure lines are in default (non-active config)
	IOChannel_ITC18_AsychOut::setup(_device, chan_index);      // run stuffin parent setup
	if (VERBOSE_IO_DEVICE) mprintf(M_IODEVICE_MESSAGE_DOMAIN,"Finished setting up PULSING for ITC Asych output channel.  Hardware port: %d",getHardwarePort());
}

void IOChannel_ITC18_AsychOut_pulsed::shutdown() {
	// if any pulse channels are currently "pulsing", then 
	//  we must shut these lines down and cancel/remove their scheduled pulse ends
	//   this is the safest thing to do, otherwise chance line could stay active.
	if (pulsing) {
		forcePulseEnd();      // will also cancel scheduling of pulse end
		if (VERBOSE_IO_DEVICE) mprintf(M_IODEVICE_MESSAGE_DOMAIN,"IOChannel_ITC18_AsychOut_pulsed::shutdown.  Shutting down pulse"); 
	}
}


void IOChannel_ITC18_AsychOut_pulsed::setupPulsing(shared_ptr<IODevice> _device, int chan_index) {	
	
	if (VERBOSE_IO_DEVICE) mprintf(M_IODEVICE_MESSAGE_DOMAIN,"IOChannel_ITC18_AsychOut_pulsed::setupPulsing.  chan_index = %d",chan_index); 
	
	pulsing = false;
	
	// schedule node for pulsing on this channel
	
	
	pulseArgs = shared_ptr<UpdateIOChannelArgs>(new UpdateIOChannelArgs());
	pulseArgs->device = _device;
	pulseArgs->channel_index = chan_index;
	
	// make sure lines are in default position
	pulseEnd();
	
}


bool IOChannel_ITC18_AsychOut_pulsed::notify(const Data& data) {
    
    if (VERBOSE_IO_DEVICE) mprintf(M_IODEVICE_MESSAGE_DOMAIN,"IOChannel_ITC18_AsychOut_pulsed::notify."); 
    // in the following case, the value of the Data has just been changed.
    //  The expected data is the number of usec to "pulse" the out line
	
    // if duration requested = 0, no actual pulse duration requested
    // --> override any existing pulse immediately
    if ((long)data == 0) {  
        return forcePulseEnd();
    }
    return (pulseStart((MonkeyWorksTime)((long)data)));
	
}


// this is how the scheuduled call ultimately stops the pulse (normal pulse end)
int IOChannel_ITC18_AsychOut_pulsed::flushChannel() {
    pulseEnd();
    return 0;
}





// this will also cancel a scheduled pulse end
bool IOChannel_ITC18_AsychOut_pulsed::forcePulseEnd() {
	
    if (VERBOSE_IO_DEVICE_DATA) mprintf(M_IODEVICE_MESSAGE_DOMAIN,"IOChannel_ITC18_AsychOut_pulsed::forcePulseEnd"); 
	
    if (pulseScheduleNode != NULL) {
		boost::mutex::scoped_lock locker(pulseScheduleNodeLock);
        pulseScheduleNode->cancel();
    }
    return (pulseEnd());
}

// this is the method executed by scheduled pulse end
bool IOChannel_ITC18_AsychOut_pulsed::pulseStart(MonkeyWorksTime pulseDurationUS) {
	
    // if anything scheuduled to end (i.e. pulse already going, kill it)
    if (pulseScheduleNode != NULL) {
		boost::mutex::scoped_lock locker(pulseScheduleNodeLock);
        pulseScheduleNode->cancel();
    }
	
    bool noError = false;
    
    // set the line to pulse value now, and schedule set to return later
    
    if (VERBOSE_IO_DEVICE_DATA) {
		shared_ptr<Clock> clock = Clock::instance();
		mprintf(M_IODEVICE_MESSAGE_DOMAIN,"IOChannel_ITC18_AsychOut_pulsed::pulseStart.  Pulse duration = %d us  Current time = %d ",(long)pulseDurationUS, (long)clock->getCurrentTimeMS()); 
	}
	
	shared_ptr<Clock> clock = Clock::instance();
	
    // this takes some time, so we need to compensate to make our pulse the right duration
    long startTimeUS = (long)clock->getCurrentTimeUS();
    if (pulseActiveHigh) {
        noError = setAsychOutputOneBit((bool)1);
    } 
    if (pulseActiveLow) {
        noError = setAsychOutputOneBit((bool)0);
    } 
    pulsing = true;
    long lostTimeUS = (long)(clock->getCurrentTimeUS()) - startTimeUS;
    MonkeyWorksTime scheduledPulseTimeUS = m_max((pulseDurationUS - lostTimeUS),0); 
	
    if (VERBOSE_IO_DEVICE_DATA) mprintf(M_IODEVICE_MESSAGE_DOMAIN,"IOChannel_ITC18_AsychOut_pulsed::pulseStart.  lost time: %d us   Scheduled lag time:%d us  Current time:%d ",lostTimeUS, (long)scheduledPulseTimeUS, (long)clock->getCurrentTimeMS()); 
	
    // note that we come back through the usual update channel function,
    //  but we keep our own local schedule node so that we can add and remove at will
	boost::mutex::scoped_lock locker(pulseScheduleNodeLock);
	
	shared_ptr<Scheduler> scheduler = Scheduler::instance();
    pulseScheduleNode = scheduler->scheduleUS(FILELINE, scheduledPulseTimeUS,		// wait this long
											  0,      // repeat delay (NA in this case)
											  1,      // repeat once
											  boost::bind(update_io_channel, 
														  pulseArgs),
											  M_DEFAULT_IODEVICE_PRIORITY,M_IODEVICE_WARN_SLOP_US,		// here, warning time is not critical (not a repeating call)
											  M_DEFAULT_FAIL_SLOP_US,
											  M_MISSED_EXECUTION_DROP);
    /*
	 ScheduleTask *node = GlobalScheduler->scheduleConstrainedUS(pulseDurationUsec, 
	 0,      // repeat delay (NA in this case)
	 1,      // repeat once
	 &update_io_channel, (void *)pulseArgs,
	 50,M_IODEVICE_WARN_SLOP_US,
	 M_DEFAULT_FAIL_SLOP_US);
	 */
	
    return noError;
    
}

// this is the method executed by scheduled pulse end
bool IOChannel_ITC18_AsychOut_pulsed::pulseEnd() {
    
    if (VERBOSE_IO_DEVICE_DATA) {
		shared_ptr<Clock> clock = Clock::instance();
		mprintf(M_IODEVICE_MESSAGE_DOMAIN,"IOChannel_ITC18_AsychOut_pulsed::pulseEnd Current time = %d",(long)clock->getCurrentTimeMS());
	}
	
    bool noError = false;
    if (pulseActiveHigh) {
        noError = setAsychOutputOneBit((bool)0);
    }
    if (pulseActiveLow) {
        noError = setAsychOutputOneBit((bool)1);
    }
    
	pulsing = false;
    return noError;
}




// ============================================================================

mWaveform::mWaveform(MonkeyWorksTime _spikeTimeUS, long preSpikeWindowTimeUS, long postSpikeWindowTimeUS, long expectedSamplingIntervalUS, int _TTLtriggerPort) {
    
    spikeTimeUS = _spikeTimeUS;
    startTimeUS = _spikeTimeUS - preSpikeWindowTimeUS;
    endTimeUS = _spikeTimeUS + postSpikeWindowTimeUS;
    sampleIntervalUS = 0;   // TBD
    sum = 0; 
    nn = 0;
    nsamples = 0;
    nsamplesInVector = 0;
	timeOfFirstElementInWaveformUS = 0;
	TTLtriggerPort = _TTLtriggerPort;
	
    // for buffering of samples
    long maxBufferSamplesExpected = (long) ( ((double)(postSpikeWindowTimeUS + preSpikeWindowTimeUS)) / ((double)expectedSamplingIntervalUS) );
    tempBufferSize = maxBufferSamplesExpected*2;        // put extra space here just in case
    waveformVector_temp = std::vector<double>(tempBufferSize,0);       
	
    //waveformPackage = Data(M_DICTIONARY, 5);
	//	waveformPackage.addElement(WAVEFORM_LINKED_HARDWARE_PORT,(long)TTLtriggerPort);    
	//    waveformPackage.addElement(WAVEFORM_SPIKE_TIME_KEY,(Data)spikeTimeUS);    
	// time of first element
	// sampling interval
	// vector
    
    //waveformVector = NULL;
	
}

mWaveform::~mWaveform() {
}

// accumulate the data in the waveform object
void mWaveform::newData(double dataV, MonkeyWorksTime timeUS) {     
	
	lock();
	if (nsamples == 0) timeOfFirstElementInWaveformUS = timeUS;
	
    if (nsamples>=1) {
        // compute estimate of time interval
        long delta = timeUS - timeLastUS;
        sum = sum + delta;
        nn++;
        sampleIntervalUS = (long)sum/nn; 
    }
    timeLastUS = timeUS;
	
    // saveData in temp vector
    if (nsamples<tempBufferSize) {
        waveformVector_temp[nsamples] = dataV;
        nsamplesInVector = nsamples;
    } 
	else {
		mwarning(M_IODEVICE_MESSAGE_DOMAIN,"A waveform buffer is being overrun.  Waveform data lost.");
	}
	
    nsamples++;
    unlock();
}    

Data mWaveform::getWaveformPackage() {
    
	lock();
	if (VERBOSE_IO_DEVICE) {
		mprintf("BUILDING WAVEFORM PACKAGE  n data points vector: %d  Sampling interval: %d us",nsamplesInVector,sampleIntervalUS);
		mprintf("BUILDING WAVEFORM PACKAGE  timeOfFirstElement: %d ms   spikeTime: %d ms", (long)(timeOfFirstElementInWaveformUS/1000),(long)(spikeTimeUS/1000));
		
		/*
		 for (int i=0; i<nsamplesInVector;i++) {
		 mprintf("Waveform: %d %f",i,waveformVector_temp[i]);
		 }
		 */
	}
	
	Data waveformPackage(M_DICTIONARY, 5);
	waveformPackage.addElement(WAVEFORM_LINKED_HARDWARE_PORT,(long)TTLtriggerPort);    
    waveformPackage.addElement(WAVEFORM_SPIKE_TIME_KEY,(Data)spikeTimeUS);
	
    // assemble final package
	waveformPackage.addElement(WAVEFORM_TIME_OF_FIRST_ELEMENT_KEY,(Data)timeOfFirstElementInWaveformUS);
    waveformPackage.addElement(WAVEFORM_SAMPLING_INTERVAL_KEY,sampleIntervalUS);
    
	Data waveformVector(M_LIST, nsamplesInVector);
    for (int i=0; i<nsamplesInVector;i++) {
        waveformVector.setElement(i,waveformVector_temp[i]);
    }
	waveformPackage.addElement(WAVEFORM_VECTOR_KEY,waveformVector);
	
	unlock();
	
    return waveformPackage;
}




IOChannelRequest_TriggeredAnalogSnippetITC18::IOChannelRequest_TriggeredAnalogSnippetITC18(
												 std::string channel_name,
												 shared_ptr<Variable> _param, 
												 std::string _requested_capability_name,  
												 IODataDirection _requested_data_direction, 
												 IODataType _requested_data_type, 
												 IODataSynchronyType _requested_synchrony_type,
												 long _requested_data_interval_usec, 
												 long _requested_update_interval_usec, 
												 double _requested_range_min, 
												 double _requested_range_max, 
												 int _requested_resolution_bits,
												 int _preSpikeWindowTimeUS,
												 int _postSpikeWindowTimeUS,
												 int _linkedTTLhardwarePort): 
IOChannelRequest(
				  channel_name,
				  _param, 
				  _requested_capability_name,  
				  _requested_data_direction, 
				  _requested_data_type, 
				  _requested_synchrony_type,
				  _requested_data_interval_usec, 
				  _requested_update_interval_usec, 
				  _requested_range_min, 
				  _requested_range_max, 
				  _requested_resolution_bits) {
	
	requested_preSpikeWindowTimeUS = _preSpikeWindowTimeUS;     
	requested_postSpikeWindowTimeUS = _postSpikeWindowTimeUS;
	requested_linkedTTLhardwarePort = _linkedTTLhardwarePort;
	if (VERBOSE_IO_DEVICE) mprintf("ITC18 subclassed channel request object was successfully instantiated. pre: %d  post us: %d Requested TTL port = %d",requested_preSpikeWindowTimeUS, requested_postSpikeWindowTimeUS, requested_linkedTTLhardwarePort);                    
	
}

/*
 B::B(const B& tocopy) : A((const A&)tocopy {
 // do nothing
 }
 */

//IOChannelRequest_ITC18::IOChannelRequest_ITC18(IOChannelRequest_ITC18& copy) : IOChannelRequest( (IOChannelRequest&)copy ) { }











