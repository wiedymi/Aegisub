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

/// @file app.cpp
/// @brief app/ commands.
/// @ingroup command
///

#include "config.h"

#ifndef AGI_PRE
#endif

#include "aegisub/context.h"
#include "main.h"

#include "dialog_about.h"
#include "audio_controller.h"
#include "frame_main.h"
#include "video_context.h"
#include "utils.h"
#include "dialog_log.h"
#include "preferences.h"
#include "dialog_version_check.h"


namespace cmd {

void app_about(agi::Context *c) {
	AboutScreen About(c->parent);
	About.ShowModal();
}


void app_display_audio_subs(agi::Context *c) {
	if (!c->audioController->IsAudioOpen()) return;
	wxGetApp().frame->SetDisplayMode(0,1);

}


void app_display_full(agi::Context *c) {
	if (!c->audioController->IsAudioOpen() || !VideoContext::Get()->IsLoaded()) return;
	wxGetApp().frame->SetDisplayMode(1,1);
}


void app_display_subs(agi::Context *c) {
	wxGetApp().frame->SetDisplayMode(0,0);
}


void app_display_video_subs(agi::Context *c) {
	wxGetApp().frame->SetDisplayMode(1,0);
}


void app_exit(agi::Context *c) {
	printf("XXX: not working yet\n");
}


void app_language(agi::Context *c) {
	// Get language
	AegisubApp *app = (AegisubApp*) wxTheApp;
	int old = app->locale.curCode;
	int newCode = app->locale.PickLanguage();
	// Is OK?
	if (newCode != -1) {
		// Set code
		OPT_SET("App/Locale")->SetInt(newCode);

		// Language actually changed?
		if (newCode != old) {
			// Ask to restart program
			int result = wxMessageBox(_T("Aegisub needs to be restarted so that the new language can be applied. Restart now?"),_T("Restart Aegisub?"),wxICON_QUESTION | wxYES_NO);
			if (result == wxYES) {
				// Restart Aegisub
				if (wxGetApp().frame->Close()) {
					RestartAegisub();
					//wxStandardPaths stand;
					//wxExecute(_T("\"") + stand.GetExecutablePath() + _T("\""));
				}
			}
		}
	}
}

void app_log(agi::Context *c) {
	LogWindow *log = new LogWindow(c->parent);
	log->Show(1);
}


void app_new_window(agi::Context *c) {
	RestartAegisub();
}


void app_options(agi::Context *c) {
	try {
		Preferences pref(c->parent);
		pref.ShowModal();
	} catch (agi::Exception& e) {
		wxPrintf("Caught agi::Exception: %s -> %s\n", e.GetName(), e.GetMessage());
	}
}


void app_updates(agi::Context *c) {
	PerformVersionCheck(true);
}


} // namespace cmd
