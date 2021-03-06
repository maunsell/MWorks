/*
 *  FakeMonkeyJuiceChannelFactory.h
 *  FakeMonkey
 *
 *  Created by bkennedy on 10/24/08.
 *  Copyright 2008 mit. All rights reserved.
 *
 */

#ifndef FAKE_MONKEY_JUICE_CHANNEL_FACTORY_H
#define FAKE_MONKEY_JUICE_CHANNEL_FACTORY_H

#include "MonkeyWorksCore/ComponentFactory.h"
#include "MonkeyWorksCore/ComponentRegistry_new.h"
using namespace mw;

class mFakeMonkeyJuiceChannelFactory : public ComponentFactory {
	virtual shared_ptr<mw::Component> createObject(std::map<std::string, std::string> parameters,
												mwComponentRegistry *reg);
};

#endif


