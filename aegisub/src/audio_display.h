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

// Helper classes used in implementation of the audio display
class AudioDisplayScrollbar;
class AudioDisplayTimeline;
class AudioDisplaySelection;



/// @class AudioDisplayInteractionObject
/// @brief Interface for objects on the audio display that can respond to mouse events
class AudioDisplayInteractionObject {
public:
	/// @brief The user is interacting with the object using the mouse
	/// @param event Mouse event data
	/// @return True to take mouse capture, false to release mouse capture
	///
	/// Assuming no object has the mouse capture, the audio display uses other methods
	/// in the object implementing this interface to deterine whether a mouse event
	/// should go to the object. If the mouse event goes to the object, this method
	/// is called.
	///
	/// If this method returns true, the audio display takes the mouse capture and
	/// stores a pointer to the AudioDisplayInteractionObject interface for the object
	/// and redirects the next mouse event to that object.
	///
	/// If the object that has the mouse capture returns false from this method, the
	/// capture is released and regular processing is done for the next event.
	///
	/// If the object does not have mouse capture and returns false from this method,
	/// no capture is taken or released and regular processing is done for the next
	/// mouse event.
	virtual bool OnMouseEvent(wxMouseEvent &event) = 0;
};



/// @class AudioDisplay
/// @brief Control that displays audio and lets the user create selections
///
/// Everyone hates this class.
class AudioDisplay: public wxWindow, private AudioControllerEventListener {
private:

	/// The audio renderer manager
	AudioRenderer *audio_renderer;

	/// The renderer for audio spectrums
	AudioSpectrumRenderer *audio_spectrum_renderer;

	/// Our current audio provider
	AudioProvider *provider;

	/// The controller managing us
	AudioController *controller;


	/// Scrollbar helper object
	AudioDisplayScrollbar *scrollbar;

	/// Timeline helper object
	AudioDisplayTimeline *timeline;


	/// Current object on display being dragged, if any
	AudioDisplayInteractionObject *dragged_object;
	/// Change the dragged object and update mouse capture
	void SetDraggedObject(AudioDisplayInteractionObject *new_obj);


	/// Leftmost pixel in the vitual audio image being displayed
	int scroll_left;

	/// Total width of the audio in pixels
	int pixel_audio_width;

	/// Horizontal zoom measured in audio samples per pixel
	int pixel_samples;

	/// Amplitude scaling ("vertical zoom") as a factor, 1.0 is neutral
	float scale_amplitude;

	// Height of main audio area in pixels
	int audio_height;


	/// Zoom level given as a number, see SetZoomLevel for details
	int zoom_level;


	/// Absolute pixel position of the playback position marker
	int playback_pos;


	struct {
		/// True if the selection has changed since the last redraw;
		bool changed;

		/// Absolute pixel position of the selection start
		int64_t start;

		/// Length of selection in pixels, <=0 and there's no selection
		int64_t length;
	}
	selection;


	void OnPaint(wxPaintEvent &event);
	void OnMouseEvent(wxMouseEvent &event);
	void OnSize(wxSizeEvent &event);
	void OnKeyDown(wxKeyEvent &event);
	void OnFocus(wxFocusEvent &event);


private:
	// AudioControllerEventListener implementation
	virtual void OnAudioOpen(AudioProvider *provider);
	virtual void OnAudioClose();
	virtual void OnMarkersMoved();
	virtual void OnSelectionChanged();
	virtual void OnPlaybackPosition(int64_t sample_position);
	virtual void OnPlaybackStop();


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

	AudioDisplay(wxWindow *parent, AudioController *controller);
	~AudioDisplay();


	/// @brief Scroll the audio display
	/// @param pixel_amount Number of pixels to scroll the view
	///
	/// A positive amount moves the display to the right, making later parts of the audio visible.
	void ScrollBy(int pixel_amount);

	/// @brief Scroll the audio display
	/// @param pixel_position Absolute pixel to put at left edge of the audio display
	///
	/// This is the principal scrolling function. All other scrolling functions eventually
	/// call this function to perform the actual scrolling.
	void ScrollPixelToLeft(int pixel_position);

	/// @brief Scroll the audio display
	/// @param pixel_position Absolute pixel to put in center of the audio display
	void ScrollPixelToCenter(int pixel_position);

	/// @brief Scroll the audio display
	/// @param sample_position Audio sample to put at left edge of the audio display
	void ScrollSampleToLeft(int64_t sample_position);

	/// @brief Scroll the audio display
	/// @param sample_position Audio sample to put in center of the audio display
	void ScrollSampleToCenter(int64_t sample_position);

	/// @brief Scroll the audio display
	/// @param range Range of audio samples to ensure is in view
	///
	/// If the entire range is already visible inside the display, nothing is scrolled. If
	/// just one of the two endpoints is visible, the display is scrolled such that the
	/// visible endpoint stays in view but more of the rest of the range becomes visible.
	///
	/// If the entire range fits inside the display, the display is centered over the range.
	/// For this calculation, the display is considered smaller by some margins, see below.
	///
	/// If the range does not fit within the display with margins subtracted, the start of 
	/// the range is ensured visible and as much of the rest of the range is brought into
	/// view.
	///
	/// For the purpose of this function, a 5 percent margin is assumed at each end of the
	/// audio display such that a range endpoint that is ensured to be in view never gets
	/// closer to the edge of the display than the margin. The edge that is not ensured to
	/// be in view might be outside of view or might be closer to the display edge than the
	/// margin.
	void ScrollSampleRangeInView(const AudioController::SampleRange &range);


	/// @brief Change the zoom level
	/// @param new_zoom_level The new zoom level to use
	///
	/// A zoom level of 0 is the default zoom level, all other levels are based on this.
	/// Negative zoom levels zoom out, positive zoom in.
	///
	/// The zoom levels generally go from +30 to -30. It is possible to zoom in more than
	/// +30 
	void SetZoomLevel(int new_zoom_level);

	/// @brief Get the zoom level
	/// @return The zoom level
	///
	/// See SetZoomLevel for a description of zoom levels.
	int GetZoomLevel() const;

	/// @brief Get a textual description of a zoom level
	/// @param level The zoom level to describe
	/// @return A translated string describing a zoom level
	///
	/// The zoom level description can tell the user details about how much audio is
	/// actually displayed.
	wxString GetZoomLevelDescription(int level) const;

	/// @brief Get the zoom factor in percent for a zoom level
	/// @param level The zoom level to get the factor of
	/// @return The zoom factor in percent
	///
	/// Positive: 125, 150, 175, 200, 225, ...
	///
	/// Negative: 90, 80, 70, 60, 50, 45, 40, 35, 30, 25, 20, 19, 18, 17, ..., 1
	///
	/// Too negative numbers get clamped.
	static int GetZoomLevelFactor(int level);


	/// @brief Set amplitude scale factor
	/// @param scale New amplitude scale factor, 1.0 is no scaling
	void SetAmplitudeScale(float scale);

	/// @brief Get amplitude scale factor
	/// @return The amplitude scaling factor
	float GetAmplitudeScale() const;


	/// @brief Reload all rendering settings from Options and reset caches
	///
	/// This can be called if some rendering quality settings have been changed in Options
	/// and need to be reloaded to take effect.
	void ReloadRenderingSettings();


	DECLARE_EVENT_TABLE()
};


