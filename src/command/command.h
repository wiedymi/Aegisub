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

/// @file command.h
/// @brief Command base class and main header.
/// @ingroup command

#include "aegisub/context.h"

#define CMD_NAME(a) const char* name() { return a; }
#define STR_MENU(a) wxString StrMenu() const { return a; }
#define STR_DISP(a) wxString StrDisplay() const { return a; }
#define STR_HELP(a) wxString StrHelp() const { return a; }

namespace cmd {


	class Command {
	public:
		virtual const char* name()=0;
		virtual wxString StrMenu() const=0;
		virtual wxString StrDisplay() const=0;
			virtual wxString StrHelp() const=0;

		virtual void operator()(agi::Context *c)=0;
		virtual ~Command();
	};


	class CommandManager {
		typedef std::map<std::string, Command*> cmdMap;
		typedef std::pair<std::string, Command*> cmdPair;
		cmdMap map;
	public:
		void reg(Command *cmd);
		int id(std::string name);
		void call(agi::Context *c, const int id);
		int count() { return map.size(); }
	};

	extern CommandManager *cm;

	int id(std::string name);
	void call(agi::Context *c, const int id);
	int count();

	void init_command(CommandManager *cm);
} // namespace cmd
