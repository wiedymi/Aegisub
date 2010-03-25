#include "compat.h"

inline wxColour lagi_wxColour(agi::Colour &colour) {
	return wxColour(colour);
}
