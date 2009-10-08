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


	/// Do a commit
	// audio_karaoke.cpp 931
	// audio_box.cpp 501
	void CommitChanges(bool) { }
	void CommitChanges() { }

	/// Set amplitude scaling
	// audio_box.cpp 346
	void SetScale(float) { }

	/// Switch dialogue line
	// audio_box.cpp 588 596
	void SetDialogue() { }

	/// Ensure the current dialogue line is visible to the user
	// audio_box.cpp 673
	void MakeDialogueVisible(bool) { }

	/// Add lead in and/or lead out
	// audio_box.cpp 741 750
	void AddLead(bool, bool) { }

	/// Set horizontal zoom level in percent of the total audio length
	// audio_box.cpp 333
	void SetSamplesPercent(int) { }


	/// Karaoke control and controller
	AudioKaraoke *karaoke;

	/// The containing audio box
	AudioBox *box;


public:

	AudioDisplay(wxWindow *parent);
	~AudioDisplay();

	DECLARE_EVENT_TABLE()
};


