/*
 *  FakeMonkeyFactory.h
 *  FakeMonkeyPlugin
 *
 *  Created by labuser on 8/18/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#ifndef FAKE_MONKEY_FACTORY_H
#define FAKE_MONKEY_FACTORY_H

#include "MonkeyWorksCore/ComponentFactory.h"

class mFakeMonkeyFactory : public mComponentFactory {
	virtual shared_ptr<mComponent> createObject(std::map<std::string, std::string> parameters,
												mComponentRegistry *reg);
};

#endif

