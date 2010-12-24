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

/// @file hotkey.cpp
/// @brief Hotkey handler
/// @ingroup hotkey menu event window

#include "config.h"

#ifndef AGI_PRE
#include <math.h>

#include <memory>
#endif

#include <libaegisub/io.h>
#include <libaegisub/json.h>
#include <libaegisub/log.h>

#include "aegisub/hotkey.h"

#include "aegisub/toolbar.h"
#include "libresrc/libresrc.h"
#include "command/command.h"


namespace hotkey {

Hotkey *hotkey;

std::string Combo::Str() {
	std::string str(key_map[0]);
	for (unsigned int i=1; i < key_map.size(); i++) {
		str.append("-" + key_map[i]);
	}
	return str;
}

std::string Combo::StrMenu() {
	return Str();
}

void Hotkey::ComboInsert(Combo *combo) {
    map.insert(HotkeyMapPair(combo->Str(), combo));
}

Hotkey::~Hotkey() {}

Hotkey::Hotkey() {

	LOG_D("hotkey/init") << "Generating hotkeys.";

    std::istringstream *stream = new std::istringstream(GET_DEFAULT_CONFIG(default_hotkey));
    json::UnknownElement hotkey_root = agi::json::parse(stream);

	json::Object object = hotkey_root;

	for (json::Object::const_iterator index(object.Begin()); index != object.End(); index++) {
		const json::Object::Member& member = *index;
		const json::Object& obj = member.element;
		BuildHotkey(member.name, obj);

    }
}


void Hotkey::BuildHotkey(std::string context, const json::Object& object) {

	for (json::Object::const_iterator index(object.Begin()); index != object.End(); index++) {
		const json::Object::Member& member = *index;


		const json::Array& array = member.element;
		for (json::Array::const_iterator arr_index(array.Begin()); arr_index != array.End(); arr_index++) {

			Combo *combo = new Combo(context, member.name);

	        const json::Object& obj = *arr_index;

			const json::Array& arr_mod = obj["modifiers"];

			if (arr_mod.Size() >  0) {
				for (json::Array::const_iterator arr_mod_index(arr_mod.Begin()); arr_mod_index != arr_mod.End(); arr_mod_index++) {
					const json::String& key_mod = *arr_mod_index;
					combo->KeyInsert(key_mod.Value());
				} // for arr_mod_index

			}
			const json::String& key = obj["key"];
			combo->KeyInsert(key.Value());
			ComboInsert(combo);
		} // for arr_index
	} // for index
}


void Hotkey::Scan(std::string context, std::string str) {
	HotkeyMap::iterator index;
	std::pair<HotkeyMap::iterator, HotkeyMap::iterator> range;

	range = map.equal_range(str);

	for (index = range.first; index != range.second; ++index) {
		std::cout << "Found: " << (*index).first << "  Context: "<< (*index).second->Context() << "  Command: " << (*index).second->CmdName() << std::endl;
	}

}

void check(std::string context, int key_code, wchar_t key_char, int modifier) {
	std::string combo;
	if ((modifier != wxMOD_NONE)) {
		if ((modifier & wxMOD_CMD) != 0) combo.append("Ctrl-");
		if ((modifier & wxMOD_ALT) != 0) combo.append("Alt-");
		if ((modifier & wxMOD_SHIFT) != 0) combo.append("Shift-");
	}

	if ((key_char != 0)
		&& (key_code != WXK_BACK)
		&& (key_code != WXK_RETURN)
		&& (key_code != WXK_ESCAPE)
		&& (key_code != WXK_SPACE)
		&& (key_code != WXK_DELETE)) {
		combo.append(wxString::Format("%c", key_char));
	} else if (hotkey::keycode_name(key_code, combo) == 1) {
		std::stringstream ss;
		ss << key_code;
		combo.append(ss.str());
	}

	std::cout << combo << std::endl;
	hotkey::hotkey->Scan(context, combo);

}

} // namespace toolbar

