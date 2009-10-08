// Copyright (c) 2005, Rodrigo Braz Monteiro
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

/// @file audio_display.h
/// @see audio_display.cpp
/// @ingroup audio_ui
///


///////////
// Headers
#ifndef AGI_PRE
#include <stdint.h>

#include <wx/bitmap.h>
#include <wx/scrolbar.h>
#include <wx/window.h>
#endif


#ifndef AGI_AUDIO_CONTROLLER_INCLUDED
#error You must include "audio_controller.h" before "audio_display.h"
#endif


//////////////
// Prototypes
class AudioRenderer;
class AudioSpectrumRenderer;
class AudioKaraoke;
class AudioProvider;
class AudioPlayer;

class AudioBox;
class SubtitlesGrid;
class AssDialogue;
class wxScrollBar;




/// @class AudioDisplay
/// @brief Control that displays audio and lets the user create selections
///
/// Everyone hates this class.
class AudioDisplay: public wxWindow {
private:

	/// The audio renderer manager
	AudioRenderer *audio_renderer;

	/// The renderer for audio spectrums
	AudioSpectrumRenderer *audio_spectrum_renderer;


	/// Leftmost pixel in the vitual audio image being displayed
	int scroll_left;

	/// Horizontal zoom measured in audio samples per pixel
	int pixel_samples;

	/// Amplitude scaling ("vertical zoom") as a factor, 1.0 is neutral
	float scale_amplitude;


	void OnPaint(wxPaintEvent &event);
	void OnMouseEvent(wxMouseEvent &event);
	void OnSize(wxSizeEvent &event);
	void OnKeyDown(wxKeyEvent &event);


public:
	// Here's all kinds of stuff other parts depend on
	// Annotating where each field or function is referenced


	/// Things have changed and a commit is required
	// audio_karaoke.cpp 667 715 927
	bool NeedCommit;

	/// Is the audio provider a temporary one (linked to the video provider)?
	// video_context.cpp 178 183
	bool temporary;

	/// Current start something in milliseconds
	// frame_main_events.cpp  2035 2045
	int curStartMS;

	/// Current end something in milliseconds
	// frame_main_events.cpp 2055 2065
	int curEndMS;


	/// Do a commit
	// audio_karaoke.cpp 928
	// audio_box.cpp 568
	// frame_main_events.cpp 2107
	void CommitChanges(bool) { }
	void CommitChanges() { }

	/// Set amplitude scaling
	// audio_box.cpp 346
	void SetScale(float) { }

	/// Get the selection start and end times
	// audio_box.cpp 445 502 514 526 540 554
	// frame_main_events.cpp 2009 2026 2076 2085
	void GetTimesSelection(int &, int &) { }

	/// Start playback
	// audio_box.cpp 446 459 503 515 529 543 555
	// video_context.cpp 689 711
	// dialog_translation.cpp 413 445
	// frame_main_events.cpp 2010 2027 2077 2086
	// dialog_styling_assistant.cpp 387 457
	void Play(int, int) { }

	/// Get the stored start and end times for the current dialogue line
	// audio_box.cpp 457
	void GetTimesDialogue(int &, int &) { }

	/// Set the selection start and end
	// audio_box.cpp 458
	void SetSelection(int, int) { }

	/// Stop playback
	// audio_box.cpp 469 479 490
	// video_context.cpp 738
	// frame_main_events.cpp 1605 2019
	void Stop() { }

	/// Switch to the next line (or syllable if in karaoke mode)
	// audio_box.cpp 480
	// frame_main_events.cpp 2093
	void Next(bool) { }
	void Next() { }

	/// Switch to the previous line (or syllable if in karaoke mode)
	// audio_box.cpp 491
	// frame_main_events.cpp 2100
	void Prev(bool) { }
	void Prev() { }

	/// Switch dialogue line
	// subs_edit_box.cpp 298 798 1051
	// audio_box.cpp 588 596
	void SetDialogue(SubtitlesGrid *, AssDialogue *, int) { }
	void SetDialogue() { }

	/// Update image back buffer
	// video_context.cpp 384
	// audio_box.cpp 729
	void UpdateImage(bool) { }

	/// Ensure the current dialogue line is visible to the user
	// audio_box.cpp 673
	void MakeDialogueVisible(bool) { }

	/// Add lead in and/or lead out
	// audio_box.cpp 741 750
	void AddLead(bool, bool) { }

	/// Get the sample number for the time in milliseconds
	// video_context.cpp 780 789
	// subs_grid.cpp 736 738
	int GetSampleAtMS(int) { return 0; }

	/// Push selection times to time edit controls
	// frame_main_events.cpp 2038 2048 2058 2068
	void UpdateTimeEditCtrls() { }

	/// Reloads the audio provider
	// dialog_options.cpp 1002
	void Reload() { }

	/// Recreates the back buffer image
	// dialog_options.cpp 1006
	void RecreateImage() { }

	/// Scrolls or something to the position given in some measure
	// audio_box.cpp 324
	void SetPosition(int) { }

	/// Attempts to load the current video as an audio file
	// audio_box.cpp 264
	void SetFromVideo() { }

	/// Loads audio from the given file
	// audio_box.cpp 270
	void SetFile(const wxString &) { }

	/// Set horizontal zoom level in percent of the total audio length
	// audio_box.cpp 333
	void SetSamplesPercent(int) { }


	/// Karaoke control and controller
	AudioKaraoke *karaoke;

	/// Audio provider, owned unless temporary
	// video_context.cpp 179 180 788 793
	// subs_grid.cpp 727
	AudioProvider *provider;

	/// The containing audio box
	// dialog_styling_assistant.cpp 75
	AudioBox *box;


public:

	AudioDisplay(wxWindow *parent);
	~AudioDisplay();

	DECLARE_EVENT_TABLE()
};


