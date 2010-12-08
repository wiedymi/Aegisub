#pragma once // sigh.

#include <wx/window.h>

#include "ass_file.h"
#include "subs_grid.h"
#include "audio_box.h"
#include "video_box.h"
#include "dialog_detached_video.h"
#include "auto4_base.h"

namespace agi {

struct Context {
	wxWindow *parent;

	AudioBox *audioBox;
	DialogDetachedVideo *detachedVideo;
	AssFile *ass;
	Automation4::ScriptManager *local_scripts;
	SubsEditBox *EditBox;
	SubtitlesGrid *SubsGrid;
	VideoBox *videoBox;
};

}
