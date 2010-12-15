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
#include <libaegisub/log.h>

namespace cmd {

CommandManager *cm;

int id(std::string name) { return cm->id(name); }
void call(agi::Context *c, const int id) { return cm->call(c, id); }
int count() { return cm->count(); }


int CommandManager::id(std::string name) {

	cmdMap::iterator index;

	if ((index = map.find(name)) != map.end()) {
		int id = std::distance(map.begin(), index);
		return id;
	}
	// XXX: throw
	printf("cmd::id NOT FOUND (%s)\n", name.c_str());
	return 60003;
}

void CommandManager::call(agi::Context *c, const int id) {
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

void CommandManager::reg(Command *cmd) {
	map.insert(cmdPair(cmd->name(), cmd));
}


void init_automation(CommandManager *cm);

void init_command(CommandManager *cm) {
	LOG_D("command/init") << "Populating command map";
	init_automation(cm);
}

} // namespace cmd
