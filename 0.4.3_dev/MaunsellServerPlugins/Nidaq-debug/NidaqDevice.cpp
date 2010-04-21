/*
 *  NidaqDevice.cpp
 *  NidaqPlugin
 *
 *  Created by Jonathan Hendry on 1/1/10.
 *  Copyright 2010 hhmi. All rights reserved.
 *
 */

#include "NidaqDevice.h"
#include "boost/bind.hpp"
#include <MonkeyWorksCore/Component.h>

#define kBufferLength	2048
#define kDIDeadtimeUS	5000			
	
using namespace mw;

DebugLockable *NidaqDevice::NidaqDriverLock;

// NIDAQ functions

void NidaqDevice::createDigitalInTask() {

	if (VERBOSE_IO_DEVICE) {
		mprintf("NidaqDevice: createDigitalInTask");
	}
	NidaqDriverLock->lock();
	if( DAQmxFailed(niError=(DAQmxBaseCreateTask("",&diTask))))
	{
		mprintf("NidaqDevice: Error creating diTask: %d\n",niError);
	}
	NidaqDriverLock->unlock();
	
}

void NidaqDevice::createDigitalInChannel() {

	char        errBuff[kBufferLength];

	if (VERBOSE_IO_DEVICE) {
		mprintf("NidaqDevice: createDigitalInChannel");
	}
	NidaqDriverLock->lock();
	if( DAQmxFailed(niError=( DAQmxBaseCreateDIChan(diTask, "Dev1/port0", NULL, DAQmx_Val_ChanForAllLines) )))
	{
		DAQmxBaseGetExtendedErrorInfo (errBuff, kBufferLength);
		mprintf("NidaqDevice: Error creating from diTask channel: %d %s\n",niError,errBuff);
	}
	NidaqDriverLock->unlock();
	
}

void NidaqDevice::startDigitalInTask() {

	shared_ptr <Clock> clock = Clock::instance();

	if (VERBOSE_IO_DEVICE) {
		mprintf("NidaqDevice: startDigitalInTask");
	}
	lastDITransitionTimeUS = clock->getCurrentTimeUS();
	NidaqDriverLock->lock();	
	if(DAQmxFailed(niError=(DAQmxBaseStartTask(diTask))))
	{
		mprintf("NidaqDevice: Error starting diTask channel: %d\n",niError);
	}
	NidaqDriverLock->unlock();
}

void NidaqDevice::stopDigitalInTask() {

	char   errBuff[kBufferLength];

	if (VERBOSE_IO_DEVICE) {
		mprintf("NidaqDevice: stopDigitalInTask");
	}
	NidaqDriverLock->lock();
	if( DAQmxFailed(niError=(DAQmxBaseStopTask(diTask))))
	{
		DAQmxBaseGetExtendedErrorInfo (errBuff, kBufferLength);
		mprintf("NidaqDevice: Error stopping diTask: %d %s\n",niError, errBuff);
	}
	if( DAQmxFailed(niError=(DAQmxBaseClearTask(diTask))))
	{
		DAQmxBaseGetExtendedErrorInfo (errBuff, kBufferLength);
		mprintf("NidaqDevice: Error clearing diTask channel: %d\n",niError);
	}
	NidaqDriverLock->unlock();
}

void NidaqDevice::createDigitalOutTask() {
	
	if (VERBOSE_IO_DEVICE) {
		mprintf("NidaqDevice: createDigitalOutTask");
	}
	NidaqDriverLock->lock();
	if( DAQmxFailed(niError=(DAQmxBaseCreateTask("", &doTask))))
	{
		mprintf("NidaqDevice: Error creating doTask: %d\n",niError);
	}
	NidaqDriverLock->unlock();
}

void NidaqDevice::createDigitalOutChannel() {
	
	char errBuff[kBufferLength];
	
	if (VERBOSE_IO_DEVICE) {
		mprintf("NidaqDevice: createDigitalOutChannel");
	}
	NidaqDriverLock->lock();
	if( DAQmxFailed(niError=(DAQmxBaseCreateDOChan(doTask, "Dev1/port1/line1:1", NULL, DAQmx_Val_ChanForAllLines) ))) // **pin 7**
	{
		DAQmxBaseGetExtendedErrorInfo (errBuff, kBufferLength);
		mprintf("NidaqDevice: Error creating doTask channel: %d %s\n",niError,errBuff);
	}
	NidaqDriverLock->unlock();
}

void NidaqDevice::startDigitalOutTask() {
	
	char        errBuff[kBufferLength];
	
	if (VERBOSE_IO_DEVICE) {
		mprintf("NidaqDevice: startDigitalOutTask");
	}
	NidaqDriverLock->lock();
	if( DAQmxFailed(niError=(DAQmxBaseStartTask(doTask))))
	{
		DAQmxBaseGetExtendedErrorInfo (errBuff, kBufferLength);
		mprintf("NidaqDevice: Error starting doTask channel: %d %s\n",niError, errBuff);
	}
	NidaqDriverLock->unlock();
}	

void NidaqDevice::stopDigitalOutTask() {
	
	char        errBuff[kBufferLength];
	
	NidaqDriverLock->lock();
	if( DAQmxFailed(niError=(DAQmxBaseStopTask(doTask))))
	{
		DAQmxBaseGetExtendedErrorInfo (errBuff, kBufferLength);
		mprintf("NidaqDevice: Error stopping doTask channel: %d %s\n",niError, errBuff);
	}
/*
 if( DAQmxFailed(niError=(DAQmxBaseClearTask(doTask))))
	{
		DAQmxBaseGetExtendedErrorInfo (errBuff, kBufferLength);
		mprintf("NidaqDevice: Error clearing doTask channel: %d %s\n",niError, errBuff);
	}
*/
	NidaqDriverLock->unlock();
}

/*
void NidaqDevice::(){
	setActive(false);
	stopDigitalInTask();

	stopDigitalOutTask();
	char        errBuff[kBufferLength];

	NidaqDriverLock->lock();
	if( DAQmxFailed(niError=( DAQmxBaseResetDevice("Dev1") )))
	{
		DAQmxBaseGetExtendedErrorInfo (errBuff, kBufferLength);
		mprintf("Error resetting device: %d %s\n",niError,errBuff);
	}
	NidaqDriverLock->unlock();
	
}
*/
// External function for scheduling

void *endPulse(const shared_ptr<NidaqDevice> &gp){
	shared_ptr <Clock> clock = Clock::instance();
	MonkeyWorksTime current = clock->getCurrentTimeUS();
	mprintf("NidaqDevice: endPulse callback %lld",current);
	gp->pulseDOLow(0);            
	return NULL;
}

void *dummy(const shared_ptr<NidaqDevice> &gp){
	return NULL;
}

static MonkeyWorksTime highTimeUS;

void NidaqDevice::pulseDOHigh(int pulseLengthUS) {
	uInt32      w_data [1];
	int32       written;
	w_data[0] = 0xFF;
	shared_ptr <Clock> clock = Clock::instance();

// The Nidaq DAQmxBaseWriteDigitalU32 function takes a millisecond to return, presumably
// because it is waiting for USB servicing.  We schedule the DOLow function before we
// make the call so that we don't add a millisecond to the total time.
	boost::mutex::scoped_lock(testLock);	
	
	boost::mutex::scoped_lock lock(pollScheduleNodeLock);
	shared_ptr<NidaqDevice> this_one = shared_from_this();
	pulseScheduleNode = scheduler->scheduleMS(std::string(FILELINE ": ") + tag,
											  pulseLengthUS / 1000.0, 
											  0, 
											  1, 
											  boost::bind(endPulse, this_one),
											  M_DEFAULT_IODEVICE_PRIORITY,
											  M_DEFAULT_IODEVICE_WARN_SLOP_US,
											  M_DEFAULT_IODEVICE_FAIL_SLOP_US,
											  M_MISSED_EXECUTION_DROP);
	
	scheduler->scheduleMS(std::string(FILELINE ": bd") + tag,
											  pulseLengthUS / 1000.0, 
											  0, 
											  1, 
											  boost::bind(dummy, this_one),
											  M_DEFAULT_IODEVICE_PRIORITY,
											  M_DEFAULT_IODEVICE_WARN_SLOP_US,
											  M_DEFAULT_IODEVICE_FAIL_SLOP_US,
											  M_MISSED_EXECUTION_DROP);
	
	MonkeyWorksTime current = clock->getCurrentTimeUS();
	mprintf("NidaqDevice:  schedule endPulse callback at %lld us (%lld)", current, clock->getCurrentTimeUS());
	highTimeUS = current;

	
	// Set the DO highh
	printf("Locking at %lld us\n", clock->getCurrentTimeUS());
	NidaqDriverLock->lock();
	printf("Got lock at %lld us\n", clock->getCurrentTimeUS());

	mprintf("NidaqDevice: setting pulse high %d ms (%lld)", pulseLengthUS / 1000, clock->getCurrentTimeUS());
	if (DAQmxFailed(niError=(DAQmxBaseWriteDigitalU32(doTask, 1, 1, 0.25, DAQmx_Val_GroupByChannel, w_data, &written, NULL))))
	{
		mprintf("NidaqDevice: Error writing to doTask channel: %d\n", niError);
	}
//	else {
//		mprintf("NidaqDevice: Wrote: %d (%lld)", written, clock->getCurrentTimeUS());
//	}
	NidaqDriverLock->unlock();
}

void NidaqDevice::pulseDOLow(int pulseLengthUS) {
	uInt32      w_data [1];
	int32       written;

	boost::mutex::scoped_lock(testLock);	
	
	shared_ptr <Clock> clock = Clock::instance();
    MonkeyWorksTime current = clock->getCurrentTimeUS();
	mprintf("NidaqDevice:  pulseDOLow at %lld us (pulse %lld us long)", current, current - highTimeUS);
	
	NidaqDriverLock->lock();
	w_data[0] = 0x00;
	if ( DAQmxFailed(niError=(DAQmxBaseWriteDigitalU32(doTask,1,1,0.25F,DAQmx_Val_GroupByChannel,w_data,&written,NULL))))
	{
		mprintf("NidaqDevice: Error writing to doTask channel: %d\n", niError);
	}
//	else mprintf("NidaqDevice: Wrote: %d",w_data[0]); 
	NidaqDriverLock->unlock();
}

bool NidaqDevice::readDI()
{
	char    errBuff[kBufferLength+1000];
	bool	state;
	uInt8	r_data[100];
	int32   read;
	shared_ptr <Clock> clock = Clock::instance();

	boost::mutex::scoped_lock(testLock);	
	
	//read = (int32 *)malloc(sizeof(int32));
	
	static bool lastState = 0xff;
	
	if (!this->getActive()) {
		return false;
	}
	NidaqDriverLock->lock();

	printf("read is %p\n", &read);
	if (DAQmxFailed(niError=(DAQmxBaseReadDigitalU8(diTask,1,1.0F,DAQmx_Val_GroupByChannel,r_data,1,NULL,NULL))))
	{
		DAQmxBaseGetExtendedErrorInfo (errBuff, kBufferLength);
		mprintf("NidaqDevice: Error reading from diTask channel: %d %s\n", niError, errBuff);
	}

	state = r_data[0] & 0x01;
	if (state != lastState) {
		if (clock->getCurrentTimeUS() - lastDITransitionTimeUS < kDIDeadtimeUS) {
			state = lastState;				// discard changes during deadtime
			mprintf("NidaqDevice: readDI, rejecting new read (last %lld now %lld, diff %lld)", lastDITransitionTimeUS, clock->getCurrentTimeUS(),
					clock->getCurrentTimeUS() - lastDITransitionTimeUS);
		}
		lastState = state;					// record and report the transition
		lastDITransitionTimeUS = clock->getCurrentTimeUS();
	}
	NidaqDriverLock->unlock();
	return state;
}

// External function for scheduling

void *update_lever(const shared_ptr<NidaqDevice> &gp){
	gp->updateSwitch();                 
	return NULL;
}

// Constructor for NidaqDevice.  MW will later call attachToDevice, which is where we finish our initialization

NidaqDevice::NidaqDevice(const boost::shared_ptr <Scheduler> &a_scheduler,
			const boost::shared_ptr <Variable> _pulseDurationMS,
			const boost::shared_ptr <Variable> _pulseOn,
			const boost::shared_ptr <Variable> _leverPress,
			const MonkeyWorksTime update_time)
{
	if (VERBOSE_IO_DEVICE) {
		mprintf("NidaqDevice: constructor");
	}
	scheduler = a_scheduler;
	pulseDurationMS = _pulseDurationMS;
	pulseOn = _pulseOn;
	leverPress = _leverPress;
	update_period = update_time;
	deviceIOrunning = false;
	NidaqDriverLock = new DebugLockable();
}

NidaqDevice::~NidaqDevice(){ 

	if (VERBOSE_IO_DEVICE) {
		mprintf("NidaqDevice: destructor\n");
	}
	if (pulseScheduleNode != NULL) {
		boost::mutex::scoped_lock lock(pollScheduleNodeLock);
        pulseScheduleNode->cancel();
		//pulseScheduleNode->kill();
    }
	/* MH 100413 Cannot do this in destructor, causes crash -- if card not initialized? */
	//pulseDOLow(0);
	//stopDigitalOutTask();
	//disconnectFromDevice();
}

// initialize the device (pre- channel creation/initialization

bool NidaqDevice::startup(){

	if (VERBOSE_IO_DEVICE) {
		mprintf("NidaqDevice: startup");
	}
	
	// Create and start the digital out task and channel that are needed for asynchronous digital output	
	
	createDigitalOutTask();
	createDigitalOutChannel();
	startDigitalOutTask();
	return true;
}

// For testing, always read a lever press.
bool NidaqDevice::updateSwitch() {	
	
	boost::mutex::scoped_lock(testLock);
	
	bool switchValue = readDI();
	//bool switchValue2 = readDI();	
	// //tell channel to read switch
	switchValue = 0;
	
	int i;
	for (i=0; i<10; i++) { 
		switchValue=readDI();
	}

	//leverPress->setValue(Data(switchValue));

	return true;
}

bool NidaqDevice::shutdown(){
	// no special action required
	if (VERBOSE_IO_DEVICE) {
		mprintf("NidaqDevice: shutdown");
	}
	return true;
}

// attachPhysicalDevice attempts a connection with hardware.  If it reports failure, MW should 
// give up on using this device and move onto the alternative device

bool NidaqDevice::attachPhysicalDevice(){                                      // attach next avaialble device to this object
	
	char errBuff[kBufferLength];
	bool success;
	
	attached_device = new IOPhysicalDeviceReference(0, "nidaq");
	
	this->connectToDevice();
	
	if (VERBOSE_IO_DEVICE) {
		mprintf("NidaqDevice: attachPhysicalDevice; start DAQmxBaseResetDevice");
	}
	NidaqDriverLock->lock();

	success = !DAQmxFailed(niError=(DAQmxBaseResetDevice("Dev1")));
	if (!success) {
		DAQmxBaseGetExtendedErrorInfo(errBuff, kBufferLength);
		mprintf("NidaqDevice: attachPhysicalDevice; Failed to find device: %d %s", niError, errBuff);
		if (niError == -200220) {
			mprintf("NidaqDevice: retrying open");
			success = !DAQmxFailed(niError=(DAQmxBaseResetDevice("Dev1")));
			if (!success) {
				DAQmxBaseGetExtendedErrorInfo(errBuff, kBufferLength);	
				mprintf("NidaqDevice: attachPhysicalDevice; Failed to find device: %d %s", niError, errBuff);			
			}
		}
		if (!success) return false;
	}
	NidaqDriverLock->unlock();
	if (VERBOSE_IO_DEVICE) {
		if (success == true) {
			mprintf("NidaqDevice: attachPhysicalDevice; Found Nidaq");
		}
	}
	return success;
}

// Start the scheduled IO on the Nidaq.  Note that the asynchronous DO is running continuously from the time the
// device is initialized.

bool NidaqDevice::startDeviceIO(){
	boost::mutex::scoped_lock(testLock);	
	if (VERBOSE_IO_DEVICE) {
		mprintf("NidaqDevice: startDeviceIO");
	}
	if (deviceIOrunning) {
		mwarning(M_IODEVICE_MESSAGE_DOMAIN,
				 "NidaqDevice startDeviceIO:  startDeviceIO request was made without first stopping IO.  Attempting to stop and restart now.");
		if (!this->stopDeviceIO()) {					// try to stop
			merror(M_IODEVICE_MESSAGE_DOMAIN, "ITC18 IO could not be stopped.  Stop and restart has failed.");
			return false;
		}
	}
	schedule_nodes_lock.lock();
	
	createDigitalInTask();
	createDigitalInChannel();
	startDigitalInTask();

	setActive(true);
	deviceIOrunning = true;
	
	int i;
	bool out;	
	for (i=0; i<5; i++) {
	
		shared_ptr<NidaqDevice>	this_one = shared_from_this();
		pollScheduleNode = scheduler->scheduleUS(std::string(FILELINE ": ") + tag,
														  (MonkeyWorksTime)(0+i*1000), 
														  30000+i*1000, 
														  M_REPEAT_INDEFINITELY, 
														  boost::bind(update_lever, 
																	  this_one),
														  M_DEFAULT_IODEVICE_PRIORITY,
														  M_DEFAULT_IODEVICE_WARN_SLOP_US,
														  M_DEFAULT_IODEVICE_FAIL_SLOP_US,
														  M_MISSED_EXECUTION_DROP);
		schedule_nodes.push_back(pollScheduleNode);       
	}

	schedule_nodes_lock.unlock();
//	this->startIO();
	return true;
}

// Stop the Nidaq collecting data.  This is typically called at the end of each trial.

bool NidaqDevice::stopDeviceIO(){
	boost::mutex::scoped_lock(testLock);
	printf("Running stopDeviceIO()\n");
    	if (VERBOSE_IO_DEVICE) {
    		mprintf("NidaqDevice: stopDeviceIO");
    	}
    	if (!deviceIOrunning) {
    		return false;
    	}
	
    	// stop all the scheduled channel checking (i.e. stop calls to "updateChannel")
    	// this will stop both flushing of the FIFO and posting of data in the software buffers
	//mprintf("NidaqDevice::stopDeviceIO: before stopAllScheduleNodes pollScheduleNode = %x", (long)pollScheduleNode);
   	stopAllScheduleNodes();								// IO device base class method -- this is thread safe
 	
	//mprintf("NidaqDevice::stopDeviceIO: after stopAllScheduleNodes pollScheduleNode = %x", pollScheduleNode);
   	if (pollScheduleNode != NULL) {
		printf("pollScheduleNode != NULL\n");
    		boost::mutex::scoped_lock lock(pollScheduleNodeLock);
			pollScheduleNode->cancel();
    		//pollScheduleNode->kill();
		//mprintf("NidaqDevice::stopDeviceIO: after kill pollScheduleNode = %x", pollScheduleNode);
    }
    	
	setActive(false);
	stopDigitalInTask();
	deviceIOrunning = false;
	return true;
}

// HACK To Get shared_from_this to work. Hold a shared second pointer to the device. Not sure why this is necessary.
//static boost::shared_ptr<mw::Component> *device;
// END HACK

boost::shared_ptr<mw::Component> NidaqDeviceFactory::createObject(std::map<std::string, std::string> parameters,
																   mw::mwComponentRegistry *reg) {
	
	const char *PULSE_DURATION = "pulse_duration";
	const char *PULSE_ON = "pulse_on";
	const char *LEVER_PRESS = "lever_press";
	const char *UPDATE_PERIOD = "data_interval";
	
	REQUIRE_ATTRIBUTES(parameters, PULSE_DURATION, UPDATE_PERIOD);
	
	MonkeyWorksTime update_period = reg->getNumber(parameters.find(UPDATE_PERIOD)->second);	
	
	boost::shared_ptr<mw::Variable> pulse_duration = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Data(M_INTEGER, 0)));	
	if(parameters.find(PULSE_DURATION) != parameters.end()) {
		pulse_duration = reg->getVariable(parameters.find(PULSE_DURATION)->second);	
		checkAttribute(pulse_duration, 
					   parameters.find("reference_id")->second, 
					   PULSE_DURATION, 
					   parameters.find(PULSE_DURATION)->second);
	}
	
	boost::shared_ptr<mw::Variable> pulse_on = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Data(M_BOOLEAN, 0)));	
	if(parameters.find(PULSE_ON) != parameters.end()) {
		pulse_on = reg->getVariable(parameters.find(PULSE_ON)->second);	
		checkAttribute(pulse_on, 
					   parameters.find("reference_id")->second, 
					   PULSE_ON, 
					   parameters.find(PULSE_ON)->second);
	}
	
	boost::shared_ptr<mw::Variable> lever_press = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Data(M_INTEGER, 0)));	
	if(parameters.find(LEVER_PRESS) != parameters.end()) {
		lever_press = reg->getVariable(parameters.find(LEVER_PRESS)->second);	
		checkAttribute(lever_press, 
					   parameters.find("reference_id")->second, 
					   LEVER_PRESS, 
					   parameters.find(LEVER_PRESS)->second);
	}
	
	boost::shared_ptr <mw::Scheduler> scheduler = mw::Scheduler::instance(true);
	
	boost::shared_ptr <mw::Component> new_daq = boost::shared_ptr<mw::Component>(new NidaqDevice(scheduler,
																								  pulse_duration, 
																								  pulse_on, 
																								  lever_press,
																								  update_period));
																								  
	// HACK: hold a second shared_ptr to the Device instance, in order to make weak_ptr work in connectToDevice().  :: JWH
	
//	boost::shared_ptr <mw::Component> t = boost::shared_ptr<mw::Component>(new_daq);
//	device =&t;
	// END HACK
	
	return new_daq;
}	

void NidaqDevice::connectToDevice() {
	
	shared_ptr<Variable> doReward = this->pulseOn;

	if (VERBOSE_IO_DEVICE) {
		mprintf("NidaqDevice: connectToDevice");
	}
	weak_ptr<NidaqDevice> weak_self_ref(getSelfPtr<NidaqDevice>());
	shared_ptr<VariableNotification> notif(new NidaqDeviceOutputNotification(weak_self_ref));
	doReward->addNotification(notif);
	connected = true;
}

void NidaqDevice::disconnectFromDevice() {

	if (VERBOSE_IO_DEVICE) {
		mprintf("NidaqDevice: disconnectFromDevice");
	}
	if (connected) {
		connected = false;
		char        errBuff[kBufferLength];
		
		NidaqDriverLock->lock();
		DAQmxBaseClearTask(diTask);
		DAQmxBaseClearTask(doTask);
		
		if( DAQmxFailed(niError=( DAQmxBaseResetDevice("Dev1") )))
		{
			DAQmxBaseGetExtendedErrorInfo (errBuff, kBufferLength);
			mprintf("NidaqDevice: Error resetting device: %d %s",niError,errBuff);
		}

		NidaqDriverLock->unlock();
	} else {
		mprintf("NidaqDevice: Was not connected");
	}
}

