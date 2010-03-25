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

/// @file option_dump.cpp
/// @brief Debug helper to dump option values.
/// @ingroup libaegisub debug

#ifndef LAGI_PRE
#include <map>

#endif

#include <libaegisub/colour.h>
#include "option_visit.h"

namespace agi {

void Options::DumpAll() {

try {
	for (OptionValueMap::const_iterator i = values.begin(); i != values.end(); ++i) {

		int type = i->second->GetType();

		switch (type) {

			case OptionValue::Type_String:
				wxPrintf("string  ->%s: %s\n", i->first, i->second->GetString());
			break;

			case OptionValue::Type_Int:
				wxPrintf("int     ->%s: %ld\n", i->first, i->second->GetInt());
			break;

			case OptionValue::Type_Double:
				wxPrintf("double  ->%s: %f\n", i->first, i->second->GetDouble());
			break;

			case OptionValue::Type_Colour:
				wxPrintf("%s: (THIS IS BROKEN)\n", i->first, i->second->GetColour().GetAsString(wxC2S_CSS_SYNTAX));
			break;

			case OptionValue::Type_Bool:
				wxPrintf("bool    ->%s: %d\n", i->first, i->second->GetBool());
			break;

			case OptionValue::Type_List_String: {
				std::vector<std::string> array_string;
				i->second->GetListString(array_string);

				wxPrintf("lstring ->%s\n", i->first);
				for (std::vector<std::string>::const_iterator i_str = array_string.begin(); i_str != array_string.end(); ++i_str) {
					wxPrintf("      %s\n", *i_str);
				}
			}
			break;

			case OptionValue::Type_List_Int: {
				std::vector<int64_t> array_int;
				i->second->GetListInt(array_int);

				wxPrintf("lint    ->%s\n", i->first);
				for (std::vector<int64_t>::const_iterator i_int = array_int.begin(); i_int != array_int.end(); ++i_int) {
					wxPrintf("      %d\n", *i_int);
				}
			}
			break;

			case OptionValue::Type_List_Double: {
				std::vector<double> array_double;
				i->second->GetListDouble(array_double);

				wxPrintf("ldouble ->%s\n", i->first);
				for (std::vector<double>::const_iterator i_double = array_double.begin(); i_double != array_double.end(); ++i_double) {
					wxPrintf("      %f\n", *i_double);
				}
			}
			break;

			case OptionValue::Type_List_Colour: {
				std::vector<Colour> array_colour;
				i->second->GetListColour(array_colour);

				wxPrintf("lcolour ->%s\n", i->first);
				for (std::vector<Colour>::const_iterator i_colour = array_colour.begin(); i_colour != array_colour.end(); ++i_colour) {
					wxPrintf("      %s\n", i_colour->GetAsString(wxC2S_CSS_SYNTAX));
				}
			}
			break;

			case OptionValue::Type_List_Bool: {
				std::vector<bool> array_bool;

				wxPrintf("lbool   ->%s\n", i->first);
				i->second->GetListBool(array_bool);
				for (std::vector<bool>::const_iterator i_bool = array_bool.begin(); i_bool != array_bool.end(); ++i_bool) {
					wxPrintf("      %d\n", *i_bool);
				}
			}
			break;
		}
	}

	} catch (Aegisub::Exception& e) {
		wxPrintf("Caught Aegisub::Exception: %s -> %s\n", e.GetName(), e.GetMessage());
	}

}

} // namespace agi
