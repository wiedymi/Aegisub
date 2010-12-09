// Copyright (c) 2005-2010, Niels Martin Hansen
// Copyright (c) 2005-2010, Rodrigo Braz Monteiro
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

/// @file 
/// @brief 
/// @ingroup command
///

#include "config.h"

#ifndef AGI_PRE
#include <wx/window.h>
#include <wx/filedlg.h>
#endif

#include "aegisub/context.h"
#include "compat.h"
#include "main.h"

namespace cmd {

void audio_close(agi::Context *c) {
	c->audioController->CloseAudio();
}

void audio_open(agi::Context *c) {
	wxString path = lagi_wxString(OPT_GET("Path/Last/Audio")->GetString());  
	wxString str = wxString(_("Audio Formats")) + _T(" (*.wav,*.mp3,*.ogg,*.flac,*.mp4,*.ac3,*.aac,*.mka,*.m4a,*.w64)|*.wav;*.mp3;*.ogg;*.flac;*.mp4;*.ac3;*.aac;*.mka;*.m4a;*.w64|")
				+ _("Video Formats") + _T(" (*.avi,*.mkv,*.ogm,*.mpg,*.mpeg)|*.avi;*.mkv;*.ogm;*.mp4;*.mpeg;*.mpg|")
				+ _("All files") + _T(" (*.*)|*.*");
	wxString filename = wxFileSelector(_("Open audio file"),path,_T(""),_T(""),str,wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (!filename.empty()) {
		c->audioController->OpenAudio(filename);
		OPT_SET("Path/Last/Audio")->SetString(STD_STR(filename));
	}
}


void audio_open_blank(agi::Context *c) {
	c->audioController->OpenAudio(_T("dummy-audio:silence?sr=44100&bd=16&ch=1&ln=396900000"));
}


void audio_open_noise(agi::Context *c) {
	c->audioController->OpenAudio(_T("dummy-audio:noise?sr=44100&bd=16&ch=1&ln=396900000"));
}


void audio_open_video(agi::Context *c) {
	c->audioController->OpenAudio(_T("audio-video:cache"));
}

void audio_view_spectrum(agi::Context *c) {
	printf("XXX: fixme\n");
}

void audio_view_waveform(agi::Context *c) {
	printf("XXX: fixme\n");
}


} // namespace cmd
