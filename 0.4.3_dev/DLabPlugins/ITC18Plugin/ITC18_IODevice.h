#ifndef _ITC18_IODEVICE_H
#define _ITC18_IODEVICE_H

//#ifdef __ppc__



#include "MonkeyWorksCore/IODevice.h"				
#include "ITC/ITC18.h"		// Instrutech header
#include "MonkeyWorksCore/ExpandableList.h"					
#include "MonkeyWorksCore/Buffers.h"				
#include "MonkeyWorksCore/ComponentFactory.h"
#include <fstream>
using namespace mw;

/**
 * ITC18_IODevice.h
 *
 * Desscription:
 *
 * History:
 * James DiCarlo created June 2005
 *
 * Copyright (c) 2002 MIT. All rights reserved.
 */


// Jan 2007
#define DO_NEW_SCHEDULER_APPROACH 0

// warning time for late scheduler calls will equal the update interval plus this amount
#define WARN_SCHEDULER_ADD_US 200

// leave this defined only if debugging without an actual itc
//#define ITC18_DEBUG 

// use  to check on how weel you are clearing the software buffers
// this is redundant with scheduler checking and can be dropped
#define STALE_DATA_WARN 1
#define STALE_DATA_WARN_NDATA	100		

#define noErr       0       //John code for ITC

// asych output
#define JUICE_BIT				0x0001			// Digital Aux. bit for juice
#define TONE_DURATION			30

// Number of AD channels (defined in intrutech header)
#define AD_CHANNELS             ITC18_AD_CHANNELS    
#define FLUSH_FIFO_CHANNEL      -1   // dummy channel for controlling FIFO flush

// Invalid entries at the start of sequence
#define GARBAGE_LENGTH			3                              
#define	ITC18_TICKS_PER_MS		800L        // Time base for ITC18
#define	ITC18_TICK_DURATION_US  1.25		// 1.25 usec for each ITC18 tick

// mask to detect the presence of a digital input being high
#define DIGITAL_BIT             0x0001                     

// size of the software buffers in ms (actual size varies with requested sampling rate)
// ** lack of scheduler servicing longer than this will cause loss of data
#define MAX_EXPECTED_BUFFER_TIME_MS			500		
#define MIN_BUFFER_SIZE_SAMPLES				50

// the slowest that we will drain the FIFO (20 ms)
#define DEFAULT_FIFO_UPDATE_INTERVAL_US	 200000			

// go no faster than this to FIFO
#define PCI_MIN_FIFO_FLUSH_INTERVAL_US  100
#define USB_MIN_FIFO_FLUSH_INTERVAL_US  1000

// check for clock drift this often (right now, just issues warning)
#define INTERVAL_TO_CHECK_CLOCK_DRIFT_MS    1000 
// threshold for ITC clock drift to issue warning        
#define MAX_ALLOWABLE_DRIFT_FRACTION_OF_SAMPLE_DURATION  2.00	// +/- two sample period
  
// a warning is also issued if the drift is more than the duration
//		of the instruction sequence
//
//	JJD notes on clock.  The clock testing shows that we can maintain within 
//	  the length of one instruciton sequence.  The is essentially the duration of 
//	  shortest requested sampling interval.  For most people, this is <1ms
//	  Thus, we are currently as good or better than the old system



// define max
#define m_max(a,b)	(((a) >= (b)) ? (a) : (b))
#define min(a,b)	(((a) <= (b)) ? (a) : (b))

// valid range values (volts) -- these are the values that are allowed in the 
//  channel request for the analog input channels -- must be exact
//  (but modified request will help out the caller)
#define VALID_ITC_ANALOG_IN_RANGE_VALUE_1V    1
#define VALID_ITC_ANALOG_IN_RANGE_VALUE_2V    2
#define VALID_ITC_ANALOG_IN_RANGE_VALUE_5V    5
#define VALID_ITC_ANALOG_IN_RANGE_VALUE_10V   10

#define VALID_ITC_ANALOG_IN_RESOLUTION_BITS			16
#define VALID_ITC_DIGITAL_EDGE_IN_RESOLUTION_BITS	1

// for channles to be validated, the actual device sampling rate must be within 
// this percent of the requested sampling rate (for each channel)
#define SAMPLING_INTERVAL_TOLERANCE_PERCENT    2    

// here is the SETUP stuff from the old DLablib.h
// ============================================================================
//Variables used in Setup of ITC18 for the experiment: ========================

#define MAX_INSTRUCTS_PER_INSTRUCT_TYPE 20

enum {	ANALOG_INPUT_CHANNEL_0=0, 	ANALOG_INPUT_CHANNEL_1, 	
        ANALOG_INPUT_CHANNEL_2, 	ANALOG_INPUT_CHANNEL_3,
		ANALOG_INPUT_CHANNEL_4, 	ANALOG_INPUT_CHANNEL_5, 	
        ANALOG_INPUT_CHANNEL_6, 	ANALOG_INPUT_CHANNEL_7, 
		N_ANALOG_INPUT_CHANNELS};

enum {	DIGITAL_INPUT_CHANNEL_0=0, DIGITAL_INPUT_CHANNEL_1, 	
        DIGITAL_INPUT_CHANNEL_2, 	DIGITAL_INPUT_CHANNEL_3,
		DIGITAL_INPUT_CHANNEL_4, 	DIGITAL_INPUT_CHANNEL_5, 	
        DIGITAL_INPUT_CHANNEL_6, 	DIGITAL_INPUT_CHANNEL_7,
		DIGITAL_INPUT_CHANNEL_8, 	DIGITAL_INPUT_CHANNEL_9, 	
        DIGITAL_INPUT_CHANNEL_10, 	DIGITAL_INPUT_CHANNEL_11,
		DIGITAL_INPUT_CHANNEL_12, 	DIGITAL_INPUT_CHANNEL_13, 	
        DIGITAL_INPUT_CHANNEL_14, 	DIGITAL_INPUT_CHANNEL_15, 
		N_DIGITAL_INPUT_CHANNELS};

#define NO_CHANNEL -1


enum {	ANALOG_OUTPUT_CHANNEL_0=0, 	ANALOG_OUTPUT_CHANNEL_1, 	
        ANALOG_OUTPUT_CHANNEL_2, 	ANALOG_OUTPUT_CHANNEL_3,
		ANALOG_OUTPUT_CHANNEL_4, 	ANALOG_OUTPUT_CHANNEL_5, 	
        ANALOG_OUTPUT_CHANNEL_6, 	ANALOG_OUTPUT_CHANNEL_7, 
		N_ANALOG_OUTPUT_CHANNELS};


// ALL possible types of ITC data type (only one type per channel)
// (note, this is both input and output types)
enum ITC18DataType 
	{	ITC_UNKNOWN_TYPE = 0,
        ITC_ADC_INPUT_CONTINUOUS_TYPE,	
        ITC_ADC_INPUT_WAVEFORM_TYPE,	
        ITC_TTL_INPUT_CONTINUOUS_TYPE,
		ITC_TTL_INPUT_EDGE_LOW_TO_HIGH_TYPE,
        ITC_TTL_INPUT_EDGE_HIGH_TO_LOW_TYPE,
        ITC_TTL_INPUT_EDGE_ANY_TYPE,
		ITC_AUXPORT_ASYCH_OUT_TYPE,
        ITC_AUXPORT_ASYCH_OUT_PULSE_ACTIVE_HIGH,
        ITC_AUXPORT_ASYCH_OUT_PULSE_ACTIVE_LOW,
		ITC_AUXPORT_ASYCH_OUT_WORD_TYPE				// JJD added Nov 7, 2007
    };
    

enum {WORD_PORT_0=0, WORD_PORT_1};

	
// DDC SETUP FIELD INDEX NUMBERS (added by JJD)
enum { 
    SETUP_AD_INDEX=0,       // analogue or digital input request?
    SETUP_CHAN_INDEX,       // which hardware port to read from?
    SETUP_TYPE_INDEX,       // what type of event? (e.g. eye, lfp, spike)
    SETUP_ASSOC_CHAN_INDEX, // what channel is associated with this input? (e.g. waveform channel for spike input)
    SETUP_RATE_INDEX,       // tells requested rate of acquisition in HZ
    MIODEVICE_CHAN_INDEX,
    NUM_ELEMENTS_ON_EACH_SETUP_LINE
    };
    
// specify the "kind" of input (input/output? analogue/digital?)
#define input_AD  		1		// analogue input
#define input_digital  	2		// digital  input
#define output_digital  3		// digital 	output
#define output_AD  		4		// analogue output




//Analog In Variables:
/*
#define input_update  	0
#define Waveform_input  1
#define lfp_input  		2
#define Eyex_input  	3
#define Eyey_input  	4
*/
// user will have to keep track of what this is by channel number
//#define analog_input_regular  5				
//#define waveform_input  6


//Digital Variables:
/*
#define response_lever  0
#define spike_input  	1
*/
// ITC will only keep track of times of rising edges, 
//      user will have to keep track of what this is by channel number	
//#define digital_input_edges		  2
//#define digital_input_continuous  3

//Analog Output Variables:
//#define stimulus  0

// waveforms
// 2 times the most number of samples needed for each waveform
//#define WAVE_DATA_BUFFER_SIZE (WAVE_WINDOW_N_SAMPLES * 2)	


// this is an old c structure for help in setting up the ITC instruction seq
typedef struct {
	short	count;		// current count						
	short	mod;		// how many ot do before resetting count									
} SKIP_DESC;


// =============================================================================

class mITC18_IODevice;

// Subclass the IOChannelRequest class so that we can add ITC-specific stuff
class IOChannelRequest_TriggeredAnalogSnippetITC18 : public IOChannelRequest {

    protected:
        int requested_preSpikeWindowTimeUS;         // parameter -- include data from up to this many US BEFORE the digital edge
        int requested_postSpikeWindowTimeUS;        // after the digital edge
        int requested_linkedTTLhardwarePort;        // makey this should be the entire request -- starts to interact with how this looks in the XML
	
    public:
        IOChannelRequest_TriggeredAnalogSnippetITC18(
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
                            int _linkedTTLhardwarePort);
		
		//IOChannelRequest_ITC18(IOChannelRequest_ITC18& copy); 
		~IOChannelRequest_TriggeredAnalogSnippetITC18() {};
        int getRequestedPreSpikeWindow(){ return requested_preSpikeWindowTimeUS;}          
        int getRequestedPostSpikeWindow() { return requested_postSpikeWindowTimeUS;}       
        int getRequestedLinkedDigitalPort() { return requested_linkedTTLhardwarePort;}	
        
        virtual IOChannelRequest *clone(){
            return (IOChannelRequest *)(new IOChannelRequest_TriggeredAnalogSnippetITC18(*this));
        }    
                	
};



// this class is used to keep track of things about each established ITC channel
//  and handle all the different needs for each itc channel type.  Subclasses are used for different types.
class IOChannel_ITC18 : public IOChannel {

	protected:
		shared_ptr<mITC18_IODevice> this_device;
		ITC18DataType	itcDataType;
		short	hardwarePort;
		bool	schedulable;
		bool	instructionSeqNeeded;
        //virtual int computeBufferSize();
        bool reportNow;
        bool edgeChannel;
    
    public:
        int		ad_index;
		int		type_index;
		//int		assoc_chan_index;
    
        IOChannel_ITC18(IOChannelRequest * _request, 
						 IOCapability * _capability, 
						 shared_ptr<mITC18_IODevice> _device);
		~IOChannel_ITC18();
    
        inline ITC18DataType    getITCDataType() { return itcDataType;}	// return hardware port for this channel
        virtual void    setup(shared_ptr<mITC18_IODevice> _device, 
							  int chan_index) {};
        virtual void    shutdown() {};
        virtual void    stopChannelIO();
        virtual int     flushChannel() {return (0);};			  // move all data into the event stream  
        virtual void    setSkippingMod(int numberOfReadsPerSavedRead) {};
        virtual void    clearAllLinkedChannels() {};     
		inline short    getHardwarePort() { return hardwarePort;}	// return hardware port for this channel
		inline bool     isInstructionSeqNeeded() {return instructionSeqNeeded;}
		inline bool     isSchedulable() {return schedulable;}
        virtual int     getITCrangeTag() { return -1; } // <0 will use default ranges -- override below   
		bool            isEdgeChannel() {return edgeChannel;}
        virtual bool    validate(IOChannelIncompatibility *incompatibility);
    
};
  
class IOChannel_ITC18_input : public IOChannel_ITC18 {        
   
    protected:
        shared_ptr<ShortDataTimeStampedBufferReader	> bufferReader;
        ShortDataTimeStampedRingBuffer		*buffer; 
  		void            bufferDatum(short data, MonkeyWorksTime timeUS); // accept a piece of data into the buffer
		void            clearBuffer();			// remove all unserviced data from buffer
       	int				computeBufferSize();
        
		
		SKIP_DESC skipInput; 
		MonkeyWorksTime timeOfDataElementAtBufferWriteHeadUS;
		
		virtual void    postDataElement(short stemp, MonkeyWorksTime timeUS);
          
    public:
        IOChannel_ITC18_input(IOChannelRequest *_request, 
							   IOCapability *_capability,
							   shared_ptr<mITC18_IODevice> _device);
        ~IOChannel_ITC18_input(); 
        virtual void    setup(shared_ptr<mITC18_IODevice> _device, int chan_index);
        virtual void    newSample(bool digitalCheck, MonkeyWorksTime time);
        virtual void    newSample(short analogSample, MonkeyWorksTime time);
        void            setSkippingMod(int numberOfReadsPerSavedRead);
        virtual int     flushChannel();			// move all data into the event stream
        
		
};


// input on ADC port
class IOChannel_ITC18_ADC : public IOChannel_ITC18_input {      
        
    protected:
        double	multiplierToGetMV;
		int		itc_range_tag;
        MonkeyWorksTime lastClippingWarnTimeMS;
		virtual void    postDataElement(short stemp, MonkeyWorksTime timeUS);

    public:
        
        IOChannel_ITC18_ADC(IOChannelRequest * _request, 
							 IOCapability * _capability, 
							 shared_ptr<mITC18_IODevice> _device);
        int             getITCrangeTag() { return itc_range_tag;};
        virtual bool    validate(IOChannelIncompatibility *incompatibility);
       

};


class mWaveform;

class IOChannel_ITC18_ADC_waveform : public IOChannel_ITC18_ADC {    
    
    protected:
        LinkedList<mWaveform> activeWaveforms;
        long preSpikeWindowTimeUS, postSpikeWindowTimeUS;   // from wave channel request
        int linkedTTLport;      // from wave channel request
        void flushWaveforms();
		
    public:
        IOChannel_ITC18_ADC_waveform(
								IOChannelRequest_TriggeredAnalogSnippetITC18 * request, 
								IOCapability * _capability, 
								shared_ptr<mITC18_IODevice> _device);
        ~IOChannel_ITC18_ADC_waveform();
        // waveform channel needs extra stuff, override posting
        virtual void    setup(shared_ptr<mITC18_IODevice> _device, int chan_index);
        void            startNewWaveform(MonkeyWorksTime timeUS);
        virtual int     flushChannel();	 // override -- do NOT move all data into the event stream -- just completed waveforms and move THEM into data steam (if done)
        virtual void    stopChannelIO();
        
};


// input on TTL port
class IOChannel_ITC18_TTL : public IOChannel_ITC18_input {      
    protected:
		virtual void    postDataElement(short stemp, MonkeyWorksTime timeUS);
	public:
        IOChannel_ITC18_TTL(IOChannelRequest* _request, 
							 IOCapability* _capability, 
							 shared_ptr<mITC18_IODevice> _device);
        virtual bool validate(IOChannelIncompatibility* incompatibility);

};


// edge detect input on TTL port
class IOChannel_ITC18_TTL_edge : public IOChannel_ITC18_TTL {      
        
    protected:
        bool lastDigitalCheck;
        ExpandableList<IOChannel_ITC18_ADC_waveform>  linkedWaveformChannels;
        
    public:
        IOChannel_ITC18_TTL_edge(IOChannelRequest* _request, 
								  IOCapability* _capability, 
								  shared_ptr<mITC18_IODevice> _device);
        virtual void setup(shared_ptr<mITC18_IODevice> _device, int chan_index);
        void    newSample(bool digitalValue, 
						  MonkeyWorksTime absoluteTimeUSbasedOnITCclock);
		virtual void clearAllLinkedChannels();      // override
        void linkToWaveformChannel(
					IOChannel_ITC18_ADC_waveform* _waveformChannel);
};


// asych outputs (aux ports)
class IOChannel_ITC18_AsychOut : public IOChannel_ITC18 {      

    protected:
		shared_ptr<AsynchronousOutputNotification> notification;
        bool  setAsychOutputOneBit(bool desiredBit);
        void	setupNotification(shared_ptr<mITC18_IODevice> _device, int chan_index);
        
	public:
        IOChannel_ITC18_AsychOut(IOChannelRequest* _request, 
								  IOCapability* _capability, 
								  shared_ptr<mITC18_IODevice> _device);
        ~IOChannel_ITC18_AsychOut(); 
        virtual void setup(shared_ptr<mITC18_IODevice> _device, int chan_index);
        virtual bool notify(const Data& data);		// e.g. ascyh outs
       
        
};


// JJD added Nov 7, 2007
class IOChannel_ITC18_AsychOutWord : public IOChannel_ITC18_AsychOut  {      
	
	protected:
		short wordPort;
	
	public:
		IOChannel_ITC18_AsychOutWord(IOChannelRequest* _request, 
								  IOCapability* _capability, 
								  shared_ptr<mITC18_IODevice> _device);
		~IOChannel_ITC18_AsychOutWord(); 
		virtual bool notify(const Data& data);		// override
};



class IOChannel_ITC18_AsychOut_pulsed : public IOChannel_ITC18_AsychOut {      

    protected:
        bool    pulseActiveHigh, pulseActiveLow;
        bool    pulsing;
        void    setupPulsing(shared_ptr<IODevice> _device, int chan_index);
        bool    forcePulseEnd();
        bool    pulseStart(MonkeyWorksTime durationUsec);
        bool    pulseEnd();
		shared_ptr <UpdateIOChannelArgs>	pulseArgs;
		shared_ptr<ScheduleTask>		pulseScheduleNode;
		boost::mutex				pulseScheduleNodeLock;
       
    public:    
        IOChannel_ITC18_AsychOut_pulsed(IOChannelRequest* _request, 
										 IOCapability* _capability, 
										 shared_ptr<mITC18_IODevice> _device, 
										 bool doPulseHigh);
		~IOChannel_ITC18_AsychOut_pulsed();
		virtual void setup(shared_ptr<mITC18_IODevice> _device, int chan_index);
        virtual bool notify(const Data& data);	// called to start the pulse (data contains duration in US)
		virtual int	 flushChannel();		// called to STOP the pulse (effectively scheduled at time of start)
        virtual void shutdown();            // use this to cause the pulse to stop 

};


// here we define an derived class of the IODevice class.
// it contains data and functions that are specific to the ITC 18.
//  This header and associated c code will ultimately be compiled as a plugin 
//  that will be linked at run time. 
// JJD July 2004
class mITC18_IODevice;

class mITC18_IODevice : public IODevice {

           // need to keep track of who is out there 
            //  (common to all ITC18IODevice objects that are created)
			static  ExpandableList<IOPhysicalDeviceReference> *visible_ITC18_devices;
            static  bool visible_ITC18_devices_initialized;
			static  long numITC18objects; 
            static  Lockable *ITC18DriverLock;

    protected:
	
			shared_ptr<Scheduler> scheduler;
    
            void *itc;						// handle to this itc -- memory it points to is allocated when object created
			int	 attachedDeviceElementNumber;  // index to the device on the ITC device list (visible_ITC18_devices)
			bool justStartedITC18;
			bool fullyInitialized;          // need to successfully run "initializeChannels" method in the base class to make this true 
											// (this will make sequence, load sequence, and init each channel before this is true)
											// the device will not start IO until this flag is true
			bool deviceIOrunning;
			bool instruction_seq_running;	// true when sequence is running in itc
			bool instructionSequenceIsNeeded;	// true if we need to build/load/run an instruciton seq
			long num_FIFO_clears_since_itc_started;
			MonkeyWorksTime time_since_inst_seq_started_US;  // time in US since ITC sequence last started running (reset to 0 at each start of instr seq)
			MonkeyWorksTime globalClockTimeAtStartUS;      // clock time when the instruction sequence starts (usec)
			MonkeyWorksTime absoluteTimeUSbasedOnITCclock; // the time that the ITC thinks it is (resynched with global clock at each start of instruction seq)
            MonkeyWorksTime nextCheckTimeUS;               // used for checking clock drift 
			MonkeyWorksTime fudgeTime;
			double clockDriftAdjustFactor;		// multiply ITC time by this to get true time
			short asychLinesStatus;         
			bool  alreadyWarnedAboutDrift;
			bool usingUSB;      // true if device is connect via USB (false for PCI)
            long flushFIFOintervalUS;
            
			// will have to establish device capabilities at some point
			bool capabilitiesSet;
			
			// setup of the ITC (defined by the user)
			int 		**setup;			// handle for the user specs for the ITC setup -- this is now dynamically allocated when "setup" is built from list of matched channels
			short		setupSize;			// number of setup lines actually needed
			//bool		userSetupDone;
			
			// instruction sequence info
			int		*pInstructions;		// pointer to the current instruction sequence for this itc  
			short   *pSamples;			// pointer to the buffer used to hold the ITC contents brought down off the FIFO
			short   *pSamplesLarge;     // buffer big enough to pull all sets of data expected in a single FIFO call all at once
            int     pSampleLargeNelements;      // current size of pSamplesLarge
            int		numInputAD;
			int		numInputDig;
            int     numOutputAD;
            int     numOutputDig;
			int		numInstr;
			float 	fast_time_slice_us;		// time between each instruction
			double 	full_time_slice_us; 	// time between the start of each full instruction sequence	
            int     *inputADs[MAX_INSTRUCTS_PER_INSTRUCT_TYPE];
            int     *outputADs[MAX_INSTRUCTS_PER_INSTRUCT_TYPE];
            int     *inputDigs[MAX_INSTRUCTS_PER_INSTRUCT_TYPE];
            int     *outputDigs[MAX_INSTRUCTS_PER_INSTRUCT_TYPE];
			int		inputRangesOnADhardwarePorts[ITC18_AD_CHANNELS];
			int		ITCTicksPerInstruction;
			
            // list to keep track of proposed new requests
            ExpandableList<IOChannelIncompatibility> *temporaryIncompatabilityList;
            bool AllChannelRequestsMetWithinSamplingTolerance;
             
			 
			// protected methods -- these are methods that are needed by the ITC, but not seen by the core
			void *openITC18();
			void closeITC18();					
			bool createInstructionSequence(
				ExpandableList<IOChannel> *channels_for_instruction_seq, 
				bool checkForIncompatabilities);  
			void makeSetup(ExpandableList<IOChannel> *channels_for_instruction_seq);  
			bool computeNumInstrLines();
			bool loadInstructionSequence();
			bool startRunningITC18instructSequence();
			bool stopRunningITC18instructSequence();	
			bool flushITC18FIFO();	
            bool flushITC18FIFO_all();
            void fullFlushITC18FIFO();	
            void parseDigital(short value, int *Setup, int index);	
            void parseAD(short value, int *Setup, short digital, 
						 float actualSamplingIntervalUS);
            void parseDigitalInputRegular(short value,int *Setup);    
            void parseAnalogInputRegular(short value, int *Setup);
            void deleteInstructionSeqArrays();			
			double getClockDriftFactor(
						IOPhysicalDeviceReference* deviceRef);
			bool setupADRanges();
			bool checkIfInstructionSequenceIsNeeded();

			
    public:
            
            mITC18_IODevice(const shared_ptr<Scheduler> &a_scheduler);  
            ~mITC18_IODevice();
	
			// methods from base class that are overridden by the ITC18_IODevice class  
			virtual IOChannel* makeNewChannel(
								IOChannelRequest* _request, 
								IOCapability* _capability);
            virtual bool attachPhysicalDevice();
			virtual ExpandableList<IOCapability> *getCapabilities();		// allow core to see what you have available
			virtual bool initializeChannels();								// this is where all of the work needed to setup the ITC is done
            virtual bool validateIndividualChannel(IOChannel *tentative_channel, 
								IOChannelIncompatibility * incomp);
            virtual bool validateMultipleChannels(ExpandableList<IOChannel> *first_pass_validated_channels);		
            virtual bool startDeviceIO();
			virtual bool stopDeviceIO(); 
			virtual bool startDeviceIOchannels();
            virtual bool updateChannel(int channel_index);					// input channels 
			virtual bool updateChannel(int channel_index, Data data);		// output channels
			bool		setAsychLinesHigh(short asychLinesToSetHigh);
			bool		setAsychLinesLow(short asychLinesToSetLow);
			bool		setAsychLinesToSpecifiedPattern(short desiredAsychLinePattern);
			bool		setAsychOutputWord8(short wordPort, char desiredWordIn8bitFormat);
			float		getMultiplierToGetMV(int itc_range_tag);
			short				setupHardwarePort(IOChannel* chan);
			ITC18DataType       computeITCDataType(IOChannel* chan);
			int					getAnalogInputRangeTag(double requestedAnalogInRangeV);
            IOChannel_ITC18_TTL_edge  *findEdgeChannelUsingTTLport(ExpandableList<IOChannel> *channelListToCheck, int TTLhardwarePortNumber);

	shared_ptr<mITC18_IODevice> shared_from_this() { return static_pointer_cast<mITC18_IODevice>(IODevice::shared_from_this()); }
};


// names of fields in waveform package
#define WAVEFORM_LINKED_HARDWARE_PORT			"TTLtriggerPort"
#define WAVEFORM_SPIKE_TIME_KEY					"spikeTimeUS"			
#define WAVEFORM_TIME_OF_FIRST_ELEMENT_KEY      "startTimeUS"
#define WAVEFORM_SAMPLING_INTERVAL_KEY			"samplingIntervalUS"
#define WAVEFORM_VECTOR_KEY						"waveformVolts"


class mWaveform : public LinkedListNode<mWaveform>, public Lockable {

    protected:
        MonkeyWorksTime spikeTimeUS;
        MonkeyWorksTime startTimeUS;
        MonkeyWorksTime endTimeUS;
		MonkeyWorksTime timeOfFirstElementInWaveformUS;
        long sampleIntervalUS;
        int sum, n, nn, nsamples, nsamplesInVector;
        MonkeyWorksTime timeLastUS;
        //Data *waveformVector;
        //Data waveformPackage;
        
        long tempBufferSize;
		std::vector<double> waveformVector_temp;
		
		int TTLtriggerPort;
    
        
    public:
        mWaveform(MonkeyWorksTime _spikeTimeUS, 
				  long preSpikeWindowTimeUS, 
				  long postSpikeWindowTimeUS, 
				  long expectedSamplingIntervalUS, 
				  int TTLtriggerPort); 
        ~mWaveform();
        void            newData(double dataV, MonkeyWorksTime timeUS);
        Data           getWaveformPackage();
        MonkeyWorksTime getStartTime(){return (startTimeUS);}
        MonkeyWorksTime getEndTime(){return (endTimeUS);}
};






// old user header stuff follows
// ===================================================================================================================

// This header allows you to setup your ITC in many differenct ways.
// It is not completely intuitive, and it is NOT tested in all configurations.
// if you change this, it is HIGHLY recommended that you run a waveform generator into the cahnnels you think 
//	you have specified, and verify behavior (e.g. sampling rates, max spike rates, etc.) over a range of input frequencies 
// PLEASE SEE JJD WITH YOUR SETUP FILE.


// intructions are stored as an array of integers donating their io type, their channel, 
// channel that they are related to(such as the case for waveform), and their speed
// -1 indicates not applicable
//  [type, io, channel in, related to, speed]
//	int type;  	//(0 update, 1 waveform, 2 lfp, 3 eye, 4 digital, 5 skip)
//	int io; 	//(0 for out, 1 for in)
//	int channel; 	//(0-7 Input, 0-3 Output)
//	int related_channel; //(0-7 Input)
//	int speed; (in Hertz)


// This is setup:
//
// Note: These instructions can be changed,
// it is parsed later in makeInstruction Sequence to make sure the order is correct
// The number of instructions, minus the number of digital instructions, cannot be greater than 19
// Also, all the sampling rates must be integer multiples of the fastest speed (e.g. if the fastest speed is 10,
//    then all the others must be either (1,2,5, or 10)  If they are not, Lablib will show an error.

// NOTE: you will not get any waveform data unless you specify at least one digital read in the setup (spike read)

// LFP is the most general user input (can be analogue values at any sampling rate)

/*
#define MY_FAST_RATE  	15000		// Hz 
#define MY_EYE_RATE  	1000		// Hz
#define MY_LFP_RATE  	1000		// Hz
#define MY_LEVER_RATE  	1000		// Hz -- this is the rate that the lever channel is checked
*/


// lever (or any TTL device)
//int lever_setup[] = {input_digital, DIGITAL_INPUT_CHANNEL_0, response_lever, NO_CHANNEL, MY_LEVER_RATE};

// eyes (same as LFP except that code will group x and y)
//int eyex_setup[] = {input_AD, ANALOG_INPUT_CHANNEL_0, Eyex_input, NO_CHANNEL, MY_EYE_RATE};  				// Eye 1 Input
//int eyey_setup[] = {input_AD, ANALOG_INPUT_CHANNEL_1, Eyey_input, NO_CHANNEL, MY_EYE_RATE}; 				// Eye 2 Input

// input "signal" 1 (i.e. input from electrode1)  --> (spike, waveform, lfp)
//int signal1_spike_setup[] = {input_digital, DIGITAL_INPUT_CHANNEL_1, spike_input, NO_CHANNEL, MY_FAST_RATE};
//int signal1_wave_setup[] = {input_AD, ANALOG_INPUT_CHANNEL_2, Waveform_input, DIGITAL_INPUT_CHANNEL_1, MY_FAST_RATE};  		// Waveform 1 Input, tied to digital in 1
//int signal1_lfp_setup[] = {input_AD, ANALOG_INPUT_CHANNEL_3, lfp_input, NO_CHANNEL, MY_LFP_RATE};  									// LFP 1 Input

// a non-specified digital edge input (e.g. MR pulse)
//int MRdigital_edge_input[] =  {input_digital, DIGITAL_INPUT_CHANNEL_2, unspecified_digital_edge_input, NO_CHANNEL, MY_FAST_RATE};

// JJD standard setup (one electrode)
//int *userSetup[] = {eyex_setup, eyey_setup, signal1_spike_setup,  signal1_wave_setup, signal1_lfp_setup, MRdigital_edge_input};


// defineMyITC18setup(userSetup,(sizeof(userSetup)/sizeof(int)));




// end user header stuff follows
// ===================================================================================================================

#endif


//#endif
