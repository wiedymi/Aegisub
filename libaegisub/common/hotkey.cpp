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

#ifndef LAGI_PRE
#include <math.h>

#include <memory>
#endif

#include "libaegisub/io.h"
#include "libaegisub/json.h"
#include "libaegisub/log.h"

#include "libaegisub/hotkey.h"

namespace agi {
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

Hotkey::Hotkey(const std::string &default_config) {

	LOG_D("hotkey/init") << "Generating hotkeys.";

	std::istringstream *stream = new std::istringstream(default_config);
	json::UnknownElement hotkey_root = agi::json_util::parse(stream);

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

			const json::Boolean& enable = obj["enable"];
			combo->Enable(enable);

			ComboInsert(combo);
		} // for arr_index
	} // for index
}


void Hotkey::Scan(std::string context, std::string str) {
	HotkeyMap::iterator index;
	std::pair<HotkeyMap::iterator, HotkeyMap::iterator> range;

	range = map.equal_range(str);

	for (index = range.first; index != range.second; ++index) {
		LOG_D("hotkey/found") << "Found: " << (*index).first << "  Context: "<< (*index).second->Context() << "  Command: " << (*index).second->CmdName();
	}

}
	} // namespace toolbar
} // namespace agi
