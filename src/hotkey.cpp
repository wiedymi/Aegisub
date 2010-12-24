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
#include <libaegisub/hotkey.h>

#include "aegisub/hotkey.h"

#include "aegisub/toolbar.h"
#include "libresrc/libresrc.h"
#include "command/command.h"


namespace hotkey {

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
	agi::hotkey::hotkey->Scan(context, combo);

}

} // namespace toolbar

