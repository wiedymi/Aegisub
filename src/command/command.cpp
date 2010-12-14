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

/// @file command.cpp
/// @brief Command system base file.
/// @ingroup command

#include "command.h"
#include "proto.h"
#include <libaegisub/log.h>

namespace cmd {

typedef std::map<std::string, Command*> cmdMap;
typedef std::pair<std::string, Command*> cmdPair;

cmdMap map;

int id(std::string name) {

	cmdMap::iterator index;

	if ((index = map.find(name)) != map.end()) {
		int id = std::distance(map.begin(), index);
		return id;
	}
	// XXX: throw
	printf("cmd::id NOT FOUND (%s)\n", name.c_str());
	return 60003;
}

void call(agi::Context *c, const int id) {

	cmdMap::iterator index(map.begin());
	std::advance(index, id);

	if (index != map.end()) {
		LOG_D("event/command") << index->first << " " << "(Id: " << id << ")";
		(*index->second)(c);
	} else {
		LOG_W("event/command/not_found") << "EVENT ID NOT FOUND: " << id;
		// XXX: throw
	}
}

int count() {
	return map.size();
}

#define NAME(a) static const char* name() { return a; }
#define STR_MENU(a) wxString StrMenu() const { return a; }
#define STR_DISP(a) wxString StrDisplay() const { return a; }
#define STR_HELP(a) wxString StrHelp() const { return a; }

#define CMD_INSERT(a) \
	Command *a##_obj = new a(); \
	map.insert(cmdPair(a::name(), a##_obj));


void command_init() {
	LOG_D("command/init") << "Populating command map";


}

} // namespace cmd
