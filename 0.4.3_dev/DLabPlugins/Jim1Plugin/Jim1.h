/*
 *  Jim1.h
 *  MonkeyWorksCore
 *
 *  Created by bkennedy on 8/26/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#ifndef Jim1_H
#define Jim1_H

// this is the class defninition of the actual object.  The only requirement is that is descends from mw::Component.
// This object is responsible for freeing any memory that it allocates.

#include "MonkeyWorksCore/Component.h"
namespace mw {
	class Jim1 : public mw::Component {
	public:
		Jim1(const std::string &_tag);
		virtual ~Jim1();
	};
}
#endif 
