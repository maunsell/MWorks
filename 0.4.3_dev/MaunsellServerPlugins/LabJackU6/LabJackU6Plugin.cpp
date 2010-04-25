/*
 *  LabJack U6 Plugin for MWorks
 *
 *  Created by Mark Histed on 4/21/2010
 *    (based on Nidaq plugin code by Jon Hendry and John Maunsell)
 *
 */


#include "LabJackU6Plugin.h"
#include "LabJackU6Device.h"

using namespace mw;

Plugin *getPlugin(){
    return new LabJackU6Plugin();
}


void LabJackU6Plugin::registerComponents(shared_ptr<mwComponentRegistry> registry) {
	
	registry->registerFactory(std::string("iodevice/labjacku6"),
							  (ComponentFactory *)(new LabJackU6DeviceFactory()));
							                              
}
