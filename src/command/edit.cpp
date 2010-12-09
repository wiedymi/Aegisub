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
#endif

#include "aegisub/context.h"
#include "aegisub/context.h"
#include "subs_edit_box.h"
#include "subs_edit_ctrl.h"
#include "dialog_search_replace.h"
#include "video_context.h"

namespace cmd {

void edit_line_copy(agi::Context *c) {
	if (c->parent->FindFocus() == c->EditBox->TextEdit) {
	c->EditBox->TextEdit->Copy();
		return;
	}
	c->SubsGrid->CopyLines(c->SubsGrid->GetSelection());
}


void edit_line_cut(agi::Context *c) {
	if (c->parent->FindFocus() == c->EditBox->TextEdit) {
		c->EditBox->TextEdit->Cut();
		return;
	}
	c->SubsGrid->CutLines(c->SubsGrid->GetSelection());
}


void edit_line_delete(agi::Context *c) {
//XXX: subs_grid.cpp
}


void edit_line_duplicate(agi::Context *c) {
//XXX: subs_grid.cpp
}


void edit_line_duplicate_shift(agi::Context *c) {
//XXX: subs_grid.cpp
}


void edit_line_join_as_karaoke(agi::Context *c) {
//XXX: subs_grid.cpp
}


void edit_line_join_concatenate(agi::Context *c) {
//XXX: subs_grid.cpp
}


void edit_line_join_keep_first(agi::Context *c) {
//XXX: subs_grid.cpp
}


void edit_line_paste(agi::Context *c) {
	if (c->parent->FindFocus() == c->EditBox->TextEdit) {
		c->EditBox->TextEdit->Paste();
		return;
	}
	c->SubsGrid->PasteLines(c->SubsGrid->GetFirstSelRow());
}


void edit_line_paste_over(agi::Context *c) {
	c->SubsGrid->PasteLines(c->SubsGrid->GetFirstSelRow(),true);
}


void edit_line_recombine(agi::Context *c) {
//XXX: subs_grid.cpp
}


void edit_line_split_by_karaoke(agi::Context *c) {
//XXX: subs_grid.cpp
}


void edit_line_swap(agi::Context *c) {
//XXX: subs_grid.cpp
}


void edit_redo(agi::Context *c) {
	VideoContext::Get()->Stop();
	c->ass->Redo();
}


void edit_search_replace(agi::Context *c) {
	VideoContext::Get()->Stop();
	Search.OpenDialog(true);
}


void edit_undo(agi::Context *c) {
	VideoContext::Get()->Stop();
	c->ass->Undo();
}


} // namespace cmd
