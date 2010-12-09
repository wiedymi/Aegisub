// Copyright (c) 2005-2010, Niels Martin Hansen
// Copyright (c) 2005-2010, Rodrigo Braz Monteiro
// Copyright (c) 2010, Amar Takhar
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//	 this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//	 this list of conditions and the following disclaimer in the documentation
//	 and/or other materials provided with the distribution.
//   * Neither the name of the Aegisub Group nor the names of its contributors
//	 may be used to endorse or promote products derived from this software
//	 without specific prior written permission.
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

/// @file time.cpp
/// @brief time/ commands.
/// @ingroup command
///

#include "config.h"

#ifndef AGI_PRE
#endif

#include "aegisub/context.h"
#include "subs_grid.h"
#include "video_context.h"
#include "ass_dialogue.h"
#include "dialog_shift_times.h"
#include "ass_file.h"

namespace cmd {

void time_continous_end(agi::Context *c) {
//XXX: subs_grid.cpp
}


void time_continous_start(agi::Context *c) {
//XXX: subs_grid.cpp
}


void time_frame_current(agi::Context *c) {
	if (!VideoContext::Get()->IsLoaded()) return;

	wxArrayInt sels = c->SubsGrid->GetSelection();
	size_t n=sels.Count();
	if (n == 0) return; 

	// Get shifting in ms
	AssDialogue *cur = c->SubsGrid->GetDialogue(sels[0]);
	if (!cur) return;
	int shiftBy = VideoContext::Get()->TimeAtFrame(VideoContext::Get()->GetFrameN(),agi::vfr::START) - cur->Start.GetMS();

	// Update
	for (size_t i=0;i<n;i++) {
		cur = c->SubsGrid->GetDialogue(sels[i]);
		if (cur) {
			cur->Start.SetMS(cur->Start.GetMS()+shiftBy);
			cur->End.SetMS(cur->End.GetMS()+shiftBy);
		}
	}

	// Commit
	c->SubsGrid->ass->Commit(_("shift to frame"), AssFile::COMMIT_TIMES);
}


void time_shift(agi::Context *c) {
	VideoContext::Get()->Stop();
	DialogShiftTimes Shift(c->parent, c->SubsGrid);
	Shift.ShowModal();
}


void time_snap_end_video(agi::Context *c) {
	c->SubsGrid->SetSubsToVideo(false);
}


void time_snap_frame(agi::Context *c) {

}


void time_snap_scene(agi::Context *c) {
	VideoContext *con = VideoContext::Get();
	if (!con->IsLoaded() || !con->KeyFramesLoaded()) return;

	// Get frames
	wxArrayInt sel = c->SubsGrid->GetSelection();
	int curFrame = con->GetFrameN();
	int prev = 0;
	int next = 0;

	const std::vector<int> &keyframes = con->GetKeyFrames();
	if (curFrame < keyframes.front()) {
		next = keyframes.front();
	}
	else if (curFrame >= keyframes.back()) {
		prev = keyframes.back();
		next = con->GetLength();
	}
	else {
		std::vector<int>::const_iterator kf = std::lower_bound(keyframes.begin(), keyframes.end(), curFrame);
		if (*kf == curFrame) {
			prev = *kf;
			next = *(kf + 1);
		}
		else {
			prev = *(kf - 1);
			next = *kf;
		}
	}

	// Get times
	int start_ms = con->TimeAtFrame(prev,agi::vfr::START);
	int end_ms = con->TimeAtFrame(next-1,agi::vfr::END);
	AssDialogue *cur;

	// Update rows
	for (size_t i=0;i<sel.Count();i++) {
		cur = c->SubsGrid->GetDialogue(sel[i]);
		cur->Start.SetMS(start_ms);
		cur->End.SetMS(end_ms);
	}

	// Commit
	c->SubsGrid->ass->Commit(_("snap to scene"), AssFile::COMMIT_TIMES);
}


void time_snap_start_video(agi::Context *c) {
	c->SubsGrid->SetSubsToVideo(false);
}


void time_sort_end(agi::Context *c) {
	c->ass->Sort(AssFile::CompEnd);
	c->ass->Commit(_("sort"));
}


void time_sort_start(agi::Context *c) {
	c->ass->Sort();
	c->ass->Commit(_("sort"));
}


void time_sort_style(agi::Context *c) {
	c->ass->Sort(AssFile::CompStyle);
	c->ass->Commit(_("sort"));
}


} // namespace cmd
