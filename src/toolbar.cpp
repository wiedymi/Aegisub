// Copyright (c) 2010, Amar Takhar <verm@aegisub.org>
//
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
// $Id$

/// @file menutool.cpp
/// @brief Dynamic menu toolbar generator.
/// @ingroup toolbar menu

#include "config.h"

#ifndef AGI_PRE
#include <math.h>

#include <memory>
#endif

#include <libaegisub/io.h>
#include <libaegisub/log.h>

#include "aegisub/toolbar.h"
#include "libresrc/libresrc.h"


namespace toolbar {

Toolbar *toolbar;

Toolbar::Toolbar() {

	json::UnknownElement toolbar_root;
	std::istringstream stream(GET_DEFAULT_CONFIG(default_toolbar));

	LOG_D("toolbar/init") << "Generating toolbars";

	try {
		json::Reader::Read(toolbar_root, stream);
	} catch (json::Reader::ParseException& e) {
		std::cout << "json::ParseException: " << e.what() << ", Line/offset: " << e.m_locTokenBegin.m_nLine + 1 << '/' << e.m_locTokenBegin.m_nLineOffset + 1 << std::endl << std::endl;
	} catch (json::Exception& e) {
		/// @todo Do something better here, maybe print the exact error
		std::cout << "json::Exception: " << e.what() << std::endl;
 	}

	json::Object object = toolbar_root;

	for (json::Object::const_iterator index(object.Begin()); index != object.End(); index++) {
		const json::Object::Member& member = *index;

		const json::Array& array = member.element;
		BuildToolbar(member.name, array);
	}
}


Toolbar::~Toolbar() {}


wxToolBar* Toolbar::GetToolbar(std::string name) {

	TbMap::iterator index;

	if ((index = map.find(name)) != map.end()) {
		return index->second;
	}

	throw ToolbarInvalidName("Unknown index");
}


void Toolbar::BuildToolbar(std::string name, const json::Array& array) {
	wxToolBar *toolbar = new wxToolBar();

	for (json::Array::const_iterator index(array.Begin()); index != array.End(); index++) {
		std::string name_sub = name;

		const json::Object& obj = *index;
		const json::Number& type_number = obj["type"];
		int type = type_number.Value();

		const json::String& command = obj["command"];

		switch (type) {
			case Toolbar::Standard: {
//				toolbar->AddTool(1, command.Value(), wxNullBitmap, "somehelp", wxITEM_NORMAL);
			}
			break;
		}

	} // for index

	map.insert(TbPair(name, toolbar));
}

} // namespace toolbar
