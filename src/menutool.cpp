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
/// @brief Dynamic menu and toolbar generator.
/// @ingroup menu

#include "config.h"

#ifndef AGI_PRE
#include <math.h>

#include <memory>

#include <wx/menuitem.h>
#endif

#include <libaegisub/io.h>

#include "aegisub/menutool.h"

namespace menu {

menu::MenuTool *menutool;

MenuTool::MenuTool() {

	main_menu = new wxMenuBar();

	json::UnknownElement menu_root;
	std::auto_ptr<std::istream> stream;

	stream.reset(agi::io::Open("./default_menu.json"));

	try {
		json::Reader::Read(menu_root, *stream);
	} catch (json::Reader::ParseException& e) {
		std::cout << "json::ParseException: " << e.what() << ", Line/offset: " << e.m_locTokenBegin.m_nLine + 1 << '/' << e.m_locTokenBegin.m_nLineOffset + 1 << std::endl << std::endl;
	} catch (json::Exception& e) {
		/// @todo Do something better here, maybe print the exact error
		std::cout << "json::Exception: " << e.what() << std::endl;
 	}

	json::Object object = menu_root;

	for (json::Object::const_iterator index(object.Begin()); index != object.End(); index++) {
		const json::Object::Member& member = *index;
//		const json::UnknownElement& element = member.element;

		std::cout << "PRIMARY: "<< member.name << "\n" << std::endl;

		const json::Array& array = member.element;
		BuildMenu(member.name, array);

	}
}

MenuTool::~MenuTool() {}


wxMenu* MenuTool::GetMenu(std::string name) {

	MTMap::iterator index;

	if ((index = map.find(name)) != map.end()) {
		return index->second;
	}

	throw MenuInvalidName("Unknown index");
}




wxMenu* MenuTool::BuildMenu(std::string name, const json::Array& array, int submenu) {
	wxMenu *menu = new wxMenu();


	for (json::Array::const_iterator index(array.Begin()); index != array.End(); index++) {
		std::string name_sub = name;

		const json::Object& obj = *index;
		const json::Number& type_number = obj["type"];
		int type = type_number.Value();

		if (type == MenuTool::Spacer) {
			menu->AppendSeparator();
			continue;
		}

		const json::String& display = obj["display"];
		const json::String& descr = obj["description"];
		const json::String& command = obj["command"];

		switch (type) {
			case MenuTool::Option: {
				wxMenuItem *menu_item = new wxMenuItem(menu, 0, wxString(display.Value()), wxString(descr.Value()), wxITEM_NORMAL);
				menu->Append(menu_item);
			}
			break;

			case MenuTool::Check: {
				menu->AppendCheckItem(0, wxString(display.Value()), wxString(descr.Value()));
			}
			break;

			case MenuTool::Radio: {
				menu->AppendRadioItem(0, wxString(display.Value()), wxString(descr.Value()));
			}
			break;

			case MenuTool::Recent: {
			}
			break;

			case MenuTool::Menu: {

				const json::Array& arr = obj["contents"];

				std::string n = name_sub + "/" + command.Value();

	 			wxMenu *menu_new = BuildMenu(name_sub.append("/").append(command), arr, 1);
				map.insert(MTPair(n, menu_new));

				if (submenu) {
					wxMenuItem *menu_item = new wxMenuItem(menu, 0, wxString(display.Value()), wxString(descr.Value()), wxITEM_NORMAL, menu_new);
					menu->Append(menu_item);
				} else {
					main_menu->Append(menu_new, wxString(display.Value()));
				}

				std::cout << "Generating Menu: " << n << " " << menu_new << std::endl;
			}
			break;
		}

	} // for index

	return menu;
}

} // namespace menu
