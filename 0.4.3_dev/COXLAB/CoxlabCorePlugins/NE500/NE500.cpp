/*
 *  NE500.cpp
 *  MonkeyWorksCore
 *
 *  Created by David Cox on 2/1/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "NE500.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
	
	
void mNE500PumpNetworkDevice::addChild(std::map<std::string, std::string> parameters,
										mComponentRegistry *reg,
										shared_ptr<mComponent> _child){

	shared_ptr<mNE500DeviceChannel> channel = dynamic_pointer_cast<mNE500DeviceChannel, mComponent>(_child);
	if(channel == NULL){
		throw mSimpleException("Attempt to access an invalid NE500 channel object");
	}

	string pump_id = channel->getPumpID();
	
	if(pump_id.empty()){
		throw mSimpleException("Attempt to access invalid (empty) NE500 pump id");
	}
	pump_ids.push_back(pump_id);
	
	initializePump(pump_id, channel->getRate(), channel->getSyringeDiameter());
	
	shared_ptr<mVariable> var = channel->getVariable();
	
	weak_ptr<mNE500PumpNetworkDevice> weak_self_ref(getSelfPtr<mNE500PumpNetworkDevice>());
	shared_ptr<mVariableNotification> notif(new mNE500DeviceOutputNotification(weak_self_ref, weak_ptr<mNE500DeviceChannel>(channel)));
	var->addNotification(notif);
}


shared_ptr<mComponent> mNE500DeviceFactory::createObject(std::map<std::string, std::string> parameters,
												 mComponentRegistry *reg) {
												 
	REQUIRE_ATTRIBUTES(parameters, "address", "port");
	
	shared_ptr<mVariable> address_variable = reg->getVariable(parameters["address"]);
	string address = (string)(address_variable->getValue());
	//string address = parameters["address"];
	int port = reg->getNumber(parameters["port"]);
    shared_ptr <mComponent> newDevice;
    

    newDevice = shared_ptr<mComponent>(new mNE500PumpNetworkDevice(address, port));
	
    return newDevice;
}


void mNE500PumpNetworkDevice::connectToDevice() {

	struct sockaddr_in addr;
	struct hostent *hostp;
	
	mprintf(M_IODEVICE_MESSAGE_DOMAIN,
			"Connecting to NE500 Pump Network...");
	
	if(NULL == (hostp = gethostbyname(address.c_str()))) {
		throw mSimpleException("Error looking up host", address);
		
	}

	memset(&addr,0,sizeof(addr));
	memcpy((char *)&addr.sin_addr,hostp->h_addr,hostp->h_length);
	addr.sin_family = hostp->h_addrtype;
	addr.sin_port = htons((u_short)port);


	if((s=socket(PF_INET, SOCK_STREAM, 6)) < 0) {
		throw mSimpleException("Error creating socket.");
	}


	int flags; 

	int rc;

	/* Set socket to non-blocking */ 

	if ((flags = fcntl(s, F_GETFL, 0)) < 0) 
	{ 
		/* Handle error */ 
	} 


	if (fcntl(s, F_SETFL, flags | O_NONBLOCK) < 0) 
	{ 
		/* Handle error */ 
	} 


	while(true){
		rc = connect(s, (struct sockaddr *) &addr, sizeof(addr));
		
		int connect_errno = errno;
		
		if(rc >= 0 || connect_errno == EISCONN){
			break;
		}
		
		if(connect_errno != EALREADY && connect_errno != EINPROGRESS && 
										connect_errno != EWOULDBLOCK){
			
			string err;
			
			
			switch(connect_errno){
				
				case EBADF:
					err = "descriptor not valid"; 
					break;
				case EIO:
					err = "io error"; 
					break;
				case ECONNREFUSED:
					err = "connection refused";
					break;
				case EFAULT:
					err = "bad address";
					break;
				case EHOSTUNREACH:
					err="host unreachable";
					break;
				case ENAMETOOLONG:
					err="name too long";
					break;
				case ENETDOWN:
					err="network down";
					break;
				case EINTR:
					err ="interrupted";
					break;
				case EINVAL:
					err = "invalid parameter";
					break;
				case ETIMEDOUT:
					err = "timed out";
					break;
				case ENOTSOCK:
					err = "not socket";
					break;
						
				default:
					err = "unknown error";
			}
			
			throw mSimpleException( (boost::format("Error (%s, %d) connecting to %s") % err % errno % address).str());
		}
		
		
		if(connect_errno == EINPROGRESS){
			timeval timeout;
			timeout.tv_sec = 5;
			timeout.tv_usec = 0;
			fd_set fd_w;
			FD_ZERO(&fd_w);
			FD_SET(s,&fd_w);
			int select_errno;
			int select_err=select(FD_SETSIZE,NULL,&fd_w,NULL,&timeout);
			
			/* 0 means it timed out out & no fds changed */
			if(select_err==0) {
				close(s);
				connected = false;
				throw mSimpleException((boost::format("Connection to NE500 Device (%s) timed out") % address).str());
			}
		}
	}

	
	connected = true;
}

void mNE500PumpNetworkDevice::disconnectFromDevice() {
	if(connected){
		close(s);
		connected = false;
	}
}

string mNE500PumpNetworkDevice::sendMessage(string message){
			
	#define RECV_BUFFER_SIZE	512
	char recv_buffer[RECV_BUFFER_SIZE];
	int rc;
	bool broken = false;
	
	string result;
	
	message = message + "\r";
	
	if(connected){
		
		// Send converted line back to client.
		if (send(s, message.c_str(), message.size(), 0) < 0){
			cerr << "Error: cannot send data";
		} else {
			mprintf("SENT: %s", message.c_str());
		}
		
		// Clear the buffer
		memset(recv_buffer, 0x0, RECV_BUFFER_SIZE);
		
		// give it a moment
		shared_ptr<mClock> clock = mClock::sharedClock();
		clock->sleepMS(1);
		
		while(true){
			rc = recv(s, recv_buffer, RECV_BUFFER_SIZE, 0);
			
			if(rc < 0){

				if(errno != EWOULDBLOCK){
					broken = true;
				}
				break;
			}
			
			result += string(recv_buffer);
		}
		
		if(broken){
			broken = false;
			disconnectFromDevice();
			
			mwarning(M_SYSTEM_MESSAGE_DOMAIN,
					 "Connection lost, reconnecting...");
			connectToDevice();
		
			// try again
			while(true){
				rc = recv(s, recv_buffer, RECV_BUFFER_SIZE, 0);
				
				if(rc < 0){

					if(errno != EWOULDBLOCK){
						broken = true;
					}
					break;
				}
				
				result += string(recv_buffer);
			}
		}
		
		
		if(broken){
			merror(M_SYSTEM_MESSAGE_DOMAIN,
					"Connection to device lost; message not sent");
		}
			
		
	mprintf("RETURNED: %s", result.c_str());
	}
	
	
	return result;
}
