// Copyright (c) 2009, Niels Martin Hansen
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

/// @file audio_controller.h
/// @see audio_controller.cpp
/// @ingroup audio_ui


#ifndef AGI_PRE
#include <stdint.h>
#include <assert.h>
#include <wx/string.h>

#include "include/aegisub/exception.h"
#endif

#include <set>

#define AGI_AUDIO_CONTROLLER_INCLUDED 1


class AudioDisplay;
class AudioPlayer;
class AudioProvider;

// Declared below
class AudioControllerEventListener;


/// @class AudioController
/// @brief Manage an open audio stream and UI state for it
///
/// Keeps track of the UI interaction state of the open audio for a project, ie. what the current
/// selection is, what moveable markers are on the audio, and any secondary non-moveable markers
/// that are present.
///
/// Changes in interaction are broadcast to all managed audio displays so they can redraw, and
/// the audio displays report all interactions back to the controller. There is a one to many
/// relationship between controller and audio displays. There is at most one audio controller
/// for an open subtitling project.
///
/// Creates and destroys audio providers and players. This behaviour should at some point be moved
/// to a separate class, as it adds too many responsibilities to this class, but at the time of
/// writing, it would extend the scope of reworking components too much.
///
/// There is not supposed to be a way to get direct access to the audio providers or players owned
/// by a controller. If some operation that isn't possible in the existing design is needed, the
/// controller should be extended in some way to allow it.
class AudioController {
public:

	/// @class SampleRange
	/// @brief Represents an immutable range of audio samples
	class SampleRange {
		int64_t _begin;
		int64_t _end;

	public:
		/// @brief Constructor
		/// @param begin Index of the first sample to include in the range
		/// @param end   Index of one past the last sample to include in the range
		SampleRange(int64_t begin, int64_t end)
			: _begin(begin)
			, _end(end)
		{
			assert(end >= begin);
		}

		/// @brief Copy constructor, optionally adjusting the range
		/// @param src          The range to duplicate
		/// @param begin_adjust Number of samples to add to the start of the range
		/// @param end_adjust   Number of samples to add to the end of the range
		SampleRange(const SampleRange &src, int64_t begin_adjust = 0, int64_t end_adjust = 0)
		{
			_begin = src._begin + begin_adjust;
			_end = src._end + end_adjust;
			assert(_end >= _begin);
		}

		/// Get the number of samples in the range
		int64_t length() const { return _end - _begin; }
		/// Get the index of the first sample in the range
		int64_t begin() const { return _begin; }
		/// Get the index of one past the last sample in the range
		int64_t end() const { return _end; }
	};


private:

	/// A list of audio displays managed by this controller
	std::set<AudioControllerEventListener *> listeners;

	/// The audio output object
	AudioPlayer *player;

	/// The audio provider
	AudioProvider *provider;


	enum PlaybackMode {
		PM_NotPlaying,
		PM_Range,
		PM_ToEnd
	};
	/// The current playback mode
	PlaybackMode playback_mode;

	/// The current audio selection
	SampleRange selection;


public:

	/// @brief Constructor
	AudioController();

	/// @brief Destructor
	~AudioController();


	/// @brief Open an audio stream
	/// @param url URL of the stream to open
	///
	/// The URL can either be a plain filename (with no qualifiers) or one
	/// recognised by various providers.
	void OpenAudio(const wxString &url);

	/// @brief Closes the current audio stream
	void CloseAudio();

	/// @brief Determine whether audio is currently open
	/// @return True if an audio stream is open and can be played back
	bool IsAudioOpen() const;

	/// @brief Get the URL for the current open audio stream
	/// @return The URL for the audio stream
	///
	/// The returned URL can be passed into OpenAudio() later to open the same stream again.
	wxString GetAudioURL() const;


	/// @brief Add an audio event listener
	/// @param display The listener to add
	void AddListener(AudioControllerEventListener *listener);

	/// @brief Remove an audio event listener
	/// @param display The listener to remove
	void RemoveListener(AudioControllerEventListener *listener);


	/// @brief Start or restart audio playback, playing a range
	/// @param range The range of audio to play back
	///
	/// Calling various other functions during playback may change the end of the range,
	/// causing playback to end sooner or later than originally requested.
	void PlayRange(const SampleRange &range);

	/// @brief Start or restart audio playback, playing from a point to the end of stream
	/// @param start_sample Index of the sample to start playback at
	///
	/// Playback to end cannot be converted to a range playback like range playback can,
	/// it will continue until the end is reached, it is stopped, or restarted.
	void PlayToEnd(int64_t start_sample);

	/// @brief Stop all audio playback
	void Stop();

	/// @brief Determine whether playback is ongoing
	/// @return True if audio is being played back
	bool IsPlaying();

	/// @brief Change the playback end point
	/// @param end_sample Sample index to stop playback at
	///
	/// Only has an effect if audio is being played and is not in "play to end" mode.
	///
	/// If the specified end sample is earlier than the current playback position, playback
	/// stops immediately.
	void ChangePlaybackEnd(int64_t end_sample);

	/// @brief Get the current playback position
	/// @return Approximate current sample index being heard by the user
	///
	/// Returns 0 if playback is stopped. The return value is only approximate.
	int64_t GetPlaybackPosition();

	/// @brief If playing, restart playback from the specified position
	/// @param new_position Sample index to restart playback from
	///
	/// This function can be used to re-synchronise audio playback to another source that
	/// might not be able to keep up with the full speed, such as video playback in high
	/// resolution or with complex subtitles.
	///
	/// This function only does something if audio is already playing.
	void ResyncPlaybackPosition(int64_t new_position);


	/// @brief Get the current audio selection
	/// @return An immutable SampleRange object
	SampleRange GetSelection() const { return selection; }

	/// @brief Change the current audio selection
	/// @param newsel The new selection to use
	void SetSelection(const SampleRange &newsel);


	/// @brief Get the playback audio volume
	/// @return The amplification factor for the audio
	double GetVolume() const;

	/// @brief Set the playback audio volume
	/// @param volume The new amplification factor for the audio
	void SetVolume(double volume);


	/// @brief Return the current audio provider
	/// @return A const pointer to the current audio provider
	const AudioProvider * GetAudioProvider() const { return provider; }


	/// @brief Convert a count of audio samples to a time in milliseconds
	/// @param samples Sample count to convert
	/// @return The number of milliseconds equivalent to the sample-count, rounded down
	int64_t MillisecondsFromSamples(int64_t samples) const;

	/// @brief Convert a time in milliseconds to a count of audio samples
	/// @param ms Time in milliseconds to convert
	/// @return The index of the first sample that is wholly inside the millisecond
	int64_t SamplesFromMilliseconds(int64_t ms) const;
};



/// @class AudioControllerEventListener
/// @brief Abstract interface for 
class AudioControllerEventListener {
public:
	/// A new audio stream was opened (and any previously open was closed)
	virtual void OnAudioOpen(AudioProvider *) = 0;

	/// The current audio stream was closed
	virtual void OnAudioClose() = 0;

	/// One or more moveable markers were moved
	virtual void OnMarkersMoved() = 0;

	/// The selection was changed
	virtual void OnSelectionChanged() = 0;

	/// Playback is in progress and ths current position was updated
	virtual void OnPlaybackPosition(int64_t sample_position) = 0;

	/// Playback has stopped
	virtual void OnPlaybackStop() = 0;
};


namespace Aegisub {
	DEFINE_BASE_EXCEPTION(AudioControllerError, Exception);
	DEFINE_SIMPLE_EXCEPTION(AudioOpenError, AudioControllerError, "audio_controller/open_failed");
};
