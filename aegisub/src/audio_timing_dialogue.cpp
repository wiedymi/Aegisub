// Copyright (c) 2010, Niels Martin Hansen
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

/// @file audio_timing_dialogue.cpp
/// @brief Default timing mode for dialogue subtitles
/// @ingroup audio_ui


#ifndef AGI_PRE
#include <stdint.h>
#include <wx/pen.h>
#endif

#include "audio_timing.h"
#include "audio_controller.h"



class AudioMarkerDialogueTiming : public AudioMarker {
	int64_t position;
	wxPen style;
	FeetStyle feet;

public:
	// AudioMarker interface
	virtual int64_t GetPosition() const { return position; }
	virtual wxPen GetStyle() const { return style; }
	virtual FeetStyle GetFeet() const { return feet; }
	virtual bool CanSnap() const { return true; }

public:
	/// @brief Change the marker to have "start marker" style
	void SetAsStart();
	/// @brief Change the marker to have "end marker" style
	void SetAsEnd();
};


/// @class AudioTimingControllerDialogue
/// @brief Default timing mode for dialogue subtitles
///
/// Displays a start and end marker for an active subtitle line, and allows
/// for those markers to be dragged. Dragging the start/end markers changes
/// the audio selection.
///
/// When the audio rendering code is expanded to support it, inactive lines
/// will also be shown as shaded lines that cannot be changed.
///
/// Another later expansion will be to affect the timing of multiple selected
/// lines at the same time, if they e.g. have end1==start2.
class AudioTimingControllerDialogue : public AudioTimingController {
	/// Start marker of the active line
	AudioMarkerDialogueTiming *marker_start;
	/// End marker of the active line
	AudioMarkerDialogueTiming *marker_end;

public:
	// AudioTimingController interface
	virtual void GetMarkers(const AudioController::SampleRange &range, AudioMarkerVector &markers) const;
	virtual wxString GetWarningMessage() const;
	virtual bool HasLabels() const;
	virtual void Next();
	virtual void Prev();
	virtual void Commit();
	virtual void Revert();
	virtual bool IsNearbyMarker(int64_t sample, int sensitivity) const;
	virtual AudioMarker * OnLeftClick(int64_t sample, int sensitivity);
	virtual AudioMarker * OnRightClick(int64_t sample, int sensitivity);
	virtual void OnMarkerDrag(AudioMarker *marker, int64_t new_position);
};



void AudioTimingControllerDialogue::GetMarkers(const AudioController::SampleRange &range, AudioMarkerVector &markers) const
{
}



wxString AudioTimingControllerDialogue::GetWarningMessage() const
{
	return wxString();
}



bool AudioTimingControllerDialogue::HasLabels() const
{
	return false;
}



void AudioTimingControllerDialogue::Next()
{
}



void AudioTimingControllerDialogue::Prev()
{
}



void AudioTimingControllerDialogue::Commit()
{
}



void AudioTimingControllerDialogue::Revert()
{
}



bool AudioTimingControllerDialogue::IsNearbyMarker(int64_t sample, int sensitivity) const
{
	return false;
}



AudioMarker * AudioTimingControllerDialogue::OnLeftClick(int64_t sample, int sensitivity)
{
	return 0;
}



AudioMarker * AudioTimingControllerDialogue::OnRightClick(int64_t sample, int sensitivity)
{
	return 0;
}



void AudioTimingControllerDialogue::OnMarkerDrag(AudioMarker *marker, int64_t new_position)
{
}
