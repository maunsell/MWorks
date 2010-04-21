/*
 *  DLabTemplate.h
 *  MonkeyWorksCore
 *
 *  Created by bkennedy on 8/26/08.
 *  Copyright 2008 MIT. All rights reserved.
 *
 */

#ifndef DLAB_TEMPLATE__H
#define DLAB_TEMPLATE__H

// this is the class defninition of the actual object.  The only requirement is that is descends from mw::Component.
// This object is responsible for freeing any memory that it allocates.

#include "MonkeyWorksCore/Component.h"
namespace mw {
	class DLabTemplate : public mw::Component {
	public:
		DLabTemplate(const std::string &_tag);
		virtual ~DLabTemplate();
	};
}
#endif 
