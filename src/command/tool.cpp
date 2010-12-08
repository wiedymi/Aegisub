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
#include <wx/utils.h>
#include <wx/window.h>
#endif

#include "aegisub/context.h"

#include "dialog_fonts_collector.h"
#include "standard_paths.h" // tool_assdraw
#include "video_context.h" // tool_font_collector
#include "dialog_resample.h"
#include "dialog_selection.h"

namespace cmd {

void tool_assdraw(agi::Context *c) {
	wxExecute(_T("\"") + StandardPaths::DecodePath(_T("?data/ASSDraw3.exe")) + _T("\""));
}


void tool_export(agi::Context *c) {
	VideoContext::Get()->Stop();
	DialogResample diag(c->parent, c->SubsGrid);
	diag.ShowModal();
}


void tool_font_collector(agi::Context *c) {
	VideoContext::Get()->Stop();
	DialogFontsCollector Collector(c->parent, c->ass);
	Collector.ShowModal();
}


void tool_line_select(agi::Context *c) {
	VideoContext::Get()->Stop();
	DialogSelection select(c->parent, c->SubsGrid);
	select.ShowModal();
}


void tool_resampleres(agi::Context *c) {

}


void tool_style_assistant(agi::Context *c) {

}


void tool_style_manager(agi::Context *c) {

}


void tool_time_kanji(agi::Context *c) {

}


void tool_time_postprocess(agi::Context *c) {

}


void tool_translation_assistant(agi::Context *c) {

}


} // namespace cmd
