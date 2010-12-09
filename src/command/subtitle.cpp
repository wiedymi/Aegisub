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

/// @file subtitle.cpp
/// @brief subtitle/ commands.
/// @ingroup command
///

#include "config.h"

#ifndef AGI_PRE
#include <wx/filedlg.h>
#include <wx/choicdlg.h>
#endif

#include <libaegisub/charset_conv.h>

#include "aegisub/context.h"
#include "dialog_search_replace.h"
#include "dialog_attachments.h"
#include "video_context.h"
#include "main.h"
#include "frame_main.h"
#include "compat.h"
#include "dialog_properties.h"
#include "dialog_spellchecker.h"


namespace cmd {

void subtitle_attachment(agi::Context *c) {
	VideoContext::Get()->Stop();
	DialogAttachments attachments(c->parent, c->ass);
	attachments.ShowModal();
}


void subtitle_find(agi::Context *c) {
	VideoContext::Get()->Stop();
	Search.OpenDialog(false);
}


void subtitle_find_next(agi::Context *c) {
	VideoContext::Get()->Stop();
	Search.FindNext();
}


void subtitle_insert_after(agi::Context *c) {
//XXX: subs_grid.cpp
}


void subtitle_insert_after_videotime(agi::Context *c) {
//XXX: subs_grid.cpp
}


void subtitle_insert_before(agi::Context *c) {
//XXX: subs_grid.cpp
}


void subtitle_insert_before_videotime(agi::Context *c) {
//XXX: subs_grid.cpp
}


void subtitle_new(agi::Context *c) {
	wxGetApp().frame->LoadSubtitles(_T(""));
}


void subtitle_open(agi::Context *c) {
	wxString path = lagi_wxString(OPT_GET("Path/Last/Subtitles")->GetString()); 
	wxString filename = wxFileSelector(_("Open subtitles file"),path,_T(""),_T(""),AssFile::GetWildcardList(0),wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (!filename.empty()) {
		wxGetApp().frame->LoadSubtitles(filename);
		wxFileName filepath(filename);
		OPT_SET("Path/Last/Subtitles")->SetString(STD_STR(filepath.GetPath()));
	}
}


void subtitle_open_charset(agi::Context *c) {
	// Initialize charsets
	wxString path = lagi_wxString(OPT_GET("Path/Last/Subtitles")->GetString());

	// Get options and load
	wxString filename = wxFileSelector(_("Open subtitles file"),path,_T(""),_T(""),AssFile::GetWildcardList(0),wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (!filename.empty()) {
		wxString charset = wxGetSingleChoice(_("Choose charset code:"), _("Charset"), agi::charset::GetEncodingsList<wxArrayString>(), c->parent, -1, -1, true, 250, 200);
		if (!charset.empty()) {
			wxGetApp().frame->LoadSubtitles(filename,charset);
		}
		OPT_SET("Path/Last/Subtitles")->SetString(STD_STR(filename));
	}
}


void subtitle_open_video(agi::Context *c) {
	wxGetApp().frame->LoadSubtitles(VideoContext::Get()->videoName, "binary");
}


void subtitle_properties(agi::Context *c) {
	VideoContext::Get()->Stop();
	DialogProperties Properties(c->parent, c->ass);
	Properties.ShowModal();
}


void subtitle_save(agi::Context *c) {
	wxGetApp().frame->SaveSubtitles(false);
}


void subtitle_save_as(agi::Context *c) {
	wxGetApp().frame->SaveSubtitles(true);
}


void subtitle_select_visible(agi::Context *c) {
	VideoContext::Get()->Stop();
	c->SubsGrid->SelectVisible();
}


void subtitle_spellcheck(agi::Context *c) {
//XXX: This is obscene, requires refactoring the spellchecker.
//	VideoContext::Get()->Stop();
//	new DialogSpellChecker;
}


void subtitle_tags_show(agi::Context *c) {
//XXX: see grid.cpp:grid_tags_hide()
}


} // namespace cmd
