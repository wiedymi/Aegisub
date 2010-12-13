// Copyright (c) 2010, Amar Takhar
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of the Aegisub Group nor the names of its contributors
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Aegisub Project http://www.aegisub.org/
//
// $Id$

/// @file recent.cpp
/// @brief recent/ commands, rebuild MRU-based lists.
/// @ingroup command
///

#include "config.h"

#ifndef AGI_PRE
#include <wx/event.h>
#endif

#include "aegisub/context.h"
#include "command.h"
#include "main.h"
#include "frame_main.h"
#include "compat.h"
#include "video_context.h"

namespace cmd {

void recent_audio(agi::Context *c) {
	c->audioController->OpenAudio(lagi_wxString(config::mru->GetEntry("Audio", event.GetId()-cmd::id("recent/audio"))));
}

void recent_keyframe(agi::Context *c) {
	VideoContext::Get()->LoadKeyframes(lagi_wxString(config::mru->GetEntry("Keyframes", event.GetId()-cmd::id("recent/keyframe"))));
}

void recent_subtitle(agi::Context *c) {
	int number = event.GetId()-cmd::id("recent/subtitle");
	wxGetApp().frame->LoadSubtitles(lagi_wxString(config::mru->GetEntry("Subtitle", number)));
}

void recent_timecode(agi::Context *c) {
	int number = event.GetId()-cmd::id("recent/timecode");
	wxGetApp().frame->LoadVFR(lagi_wxString(config::mru->GetEntry("Timecodes", number)));
}

void recent_video(agi::Context *c) {
	int number = event.GetId()-cmd::id("recent/video");
	wxGetApp().frame->LoadVideo(lagi_wxString(config::mru->GetEntry("Video", number)));
}


} // namespace cmd
