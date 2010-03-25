#ifndef AGI_PRE
#include <wx/colour.h>

#include <string>
#endif

#include <libaegisub/colour.h>

#define STD_STR(x) std::string(x.mb_str())

inline wxColour lagi_wxColour(const agi::Colour &colour) { return wxColour(colour); }
