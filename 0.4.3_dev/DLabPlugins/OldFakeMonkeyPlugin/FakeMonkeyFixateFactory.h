/*
 *  FakeMonkeyFixateFactory.h
 *  FakeMonkey
 *
 *  Created by bkennedy on 10/8/08.
 *  Copyright 2008 mit. All rights reserved.
 *
 */

#ifndef FAKE_MONKEY_FIXATE_FACTORY_H
#define FAKE_MONKEY_FIXATE_FACTORY_H

#include "MonkeyWorksCore/ComponentFactory.h"

class mFakeMonkeyFixateFactory : public mComponentFactory{
	virtual shared_ptr<mComponent> createObject(std::map<std::string, std::string> parameters,
												mComponentRegistry *reg);
};

#endif
