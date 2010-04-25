/*
 *  main.c
 *  Exodriver example
 *
 *  Created by Mark Histed on 4/20/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#include <stdlib.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>
#include "u6.h"
#include <sys/time.h>
#include <assert.h>

//decls
bool ljU6WriteDI(HANDLE Handle, long Channel, long State);
bool ljU6ConfigPorts(HANDLE Handle);
bool ljU6ReadDI(HANDLE Handle, long Channel, long* State);
#define LJU6_REWARD_FIO 1
#define LJU6_LEVERPRESS_FIO 0
long timeUS();

//main
int main (int argc, const char * argv[]) {
    bool retval;
    HANDLE ljHandle;

	printf("Main\n");
    
    if( (ljHandle = openUSBConnection(-1)) == NULL) {    
        printf("Error\n");
        return -1;
    }
    
    retval = ljU6ConfigPorts(ljHandle);
    
    int i;
    long t1,t2;
    
    //eDO(ljHandle, LJU6_REWARD_FIO, 0);
    for (i=0; i<1000; i++) {
        t1 = timeUS();
        //eDO(ljHandle, LJU6_REWARD_FIO, 1);
        retval = ljU6WriteDI(ljHandle, LJU6_REWARD_FIO, 1);
        t2 = timeUS();
        printf("Elapsed %d us \n", t2-t1);
        
        usleep(500000);

        retval = ljU6WriteDI(ljHandle, LJU6_REWARD_FIO, 0);
        //eDO(ljHandle, LJU6_REWARD_FIO, 0);
        usleep(500000);
    }
    
    closeUSBConnection(ljHandle);
    return 0;
}


bool ljU6ConfigPorts(HANDLE Handle) {
    /// set up IO ports
    uint8 sendDataBuff[6]; // recDataBuff[1];
    uint8 Errorcode, ErrorFrame;
    
    // setup one to be output, one input, and set output to zero
    sendDataBuff[0] = 13;       //IOType is BitDirWrite
    sendDataBuff[1] = (LJU6_LEVERPRESS_FIO & 0x0f) | (0 << 7);  //IONumber(bits 0-4) + Direction (bit 7; 1 is output)
    sendDataBuff[2] = 13;       //IOType is BitDirWrite
    sendDataBuff[3] = LJU6_REWARD_FIO & 0x0f | (1 << 7);  //IONumber(bits 0-4) + Direction (bit 7; 1 is output)
    sendDataBuff[4] = 11;             //IOType is BitStateWrite
    sendDataBuff[5] = (LJU6_REWARD_FIO & 0x0f) | (0 << 7);  //IONumber(bits 0-4) + State (bit 7)
    
    
    //printf("*****************Output %x %x %x %x\n", sendDataBuff[0], sendDataBuff[1], sendDataBuff[2], sendDataBuff[3]);
    
    if(ehFeedback(Handle, sendDataBuff, 6, &Errorcode, &ErrorFrame, NULL, 0) < 0) {
        printf("bug: ehFeedback error, see stdout");  // note we will get a more informative error on stdout
        goto cleanup;
    }
    if(Errorcode) {
        printf("ehFeedback: error with command, errorcode was %d");
        goto cleanup;
    }
    
    return true;
    
cleanup:
    closeUSBConnection(Handle);
    return false;
}


bool ljU6ReadDI(HANDLE Handle, long Channel, long* State) {
    // returns: 1 on success, 0 on error
    // output written to State
    
    uint8 sendDataBuff[4], recDataBuff[1];
    uint8 Errorcode, ErrorFrame;
    
    sendDataBuff[0] = 10;       //IOType is BitStateRead
    sendDataBuff[1] = Channel;  //IONumber
    
    printf("entering ljU6ReadDI\n"); fflush(stdout);
    if(ehFeedback(Handle, sendDataBuff, 2, &Errorcode, &ErrorFrame, recDataBuff, 1) < 0) {
        printf("bug: ehFeedback error, see stdout");  // note we will get a more informative error on stdout
        return 0;
    }
    if(Errorcode) {
        printf("ehFeedback: error with command, errorcode was %d");
        return 0;
    }
    
    *State = (long int)recDataBuff[0];
    return 1;
    
}

bool ljU6WriteDI(HANDLE Handle, long Channel, long State) {
    
    uint8 sendDataBuff[2]; // recDataBuff[1];
    uint8 Errorcode, ErrorFrame;
    
    sendDataBuff[0] = 11;             //IOType is BitStateWrite
    sendDataBuff[1] = Channel + 128*((State > 0) ? 1 : 0);  //IONumber(bits 0-4) + State (bit 7)
    
    if(ehFeedback(Handle, sendDataBuff, 2, &Errorcode, &ErrorFrame, NULL, 0) < 0) {
        printf("bug: ehFeedback error, see stdout");  // note we will get a more informative error on stdout
        goto cleanup;
    }
    if(Errorcode) {
        printf("ehFeedback: error with command, errorcode was %d");
        goto cleanup;
    }
    
    return true;
    
cleanup:
    // Should do this in destructor?
    //closeUSBConnection(ljHandle);  
    return false;    
}



long timeUS() {
    struct timeval t1;
    int retval;
    long outUS;
    static int isStarted=0;
    static long startSec;
    
    retval = gettimeofday(&t1, NULL);
    if (retval != 0) {printf("gettimeofday() error\n"); exit -1; }
    
    if (~isStarted) {
        isStarted = 1;
        startSec = t1.tv_sec;
    }
    
    outUS = (t1.tv_sec-startSec)*10000000 + t1.tv_usec;
    assert(outUS < LONG_MAX/4);  // ensure not near overflow

    return outUS;
};
