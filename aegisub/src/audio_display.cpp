// Copyright (c) 2005, Rodrigo Braz Monteiro
// Copyright (c) 2009-2010, Niels Martin Hansen
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

/// @file audio_display.cpp
/// @brief Display audio in the main UI
/// @ingroup audio_ui
///


///////////
// Headers
#include "config.h"

#ifndef AGI_PRE
#include <algorithm>

#include <wx/dcclient.h>
#endif

#include "ass_time.h"
#include "audio_controller.h"
#include "audio_display.h"
#include "block_cache.h"
#include "audio_renderer.h"
#include "audio_renderer_spectrum.h"
#include "include/aegisub/audio_provider.h"
#include "options.h"


#undef min
#undef max


class AudioDisplayScrollbar : public AudioDisplayInteractionObject {
	static const int height = 10;

	wxRect bounds;
	wxRect thumb;

	bool dragging;   // user is dragging with the primary mouse button

	int data_length; // total amount of data in control
	int page_length; // amount of data in one page
	int position;    // first item displayed

	int sel_start;   // first data item in selection
	int sel_length;  // number of data items in selection

	AudioDisplay *display;

	// Recalculate thumb bounds from position and length data
	void RecalculateThumb()
	{
		thumb.width = std::max((height+1)/2, bounds.width * page_length / data_length);
		thumb.height = height;
		thumb.x = bounds.width * position / data_length;
		thumb.y = bounds.y;
	}

public:

	AudioDisplayScrollbar(AudioDisplay *_display)
		: dragging(false)
		, data_length(1)
		, page_length(1)
		, position(0)
		, sel_start(-1)
		, sel_length(0)
		, display(_display)
	{
	}

	virtual ~AudioDisplayScrollbar()
	{
	}

	// The audio display has changed size
	void SetDisplaySize(const wxSize &display_size)
	{
		bounds.x = 0;
		bounds.y = display_size.y - height;
		bounds.width = display_size.x;
		bounds.height = height;
		page_length = display_size.x;

		RecalculateThumb();
	}


	const wxRect & GetBounds() const
	{
		return bounds;
	}

	int GetPosition() const
	{
		return position;
	}

	int SetPosition(int new_position)
	{
		// These two conditionals can't be swapped, otherwise the position can become
		// negative if the entire data is shorter than one page.
		if (new_position + page_length >= data_length)
			new_position = data_length - page_length - 1;
		if (new_position < 0)
			new_position = 0;

		// This check is required to avoid mutual recursion with the display
		if (new_position != position)
		{
			position = new_position;
			RecalculateThumb();
			display->ScrollPixelToLeft(position);
		}

		return position;
	}

	void SetSelection(int new_start, int new_length)
	{
		sel_start = new_start;
		sel_length = new_length;
	}

	void ChangeLengths(int new_data_length, int new_page_length)
	{
		data_length = new_data_length;
		page_length = new_page_length;

		RecalculateThumb();
	}

	bool OnMouseEvent(wxMouseEvent &event)
	{
		if (event.LeftIsDown())
		{
			const int thumb_left = event.GetPosition().x - thumb.width/2;
			const int data_length_less_page = data_length - page_length;
			const int shaft_length_less_thumb = bounds.width - thumb.width;

			SetPosition(data_length_less_page * thumb_left / shaft_length_less_thumb);

			dragging = true;
		}
		else if (event.LeftUp())
		{
			dragging = false;
		}

		return dragging;
	}

	void Paint(wxDC &dc, bool has_focus)
	{
		wxColour light(89, 145, 220);
		wxColour dark(8, 4, 13);
		wxColour sel(65, 34, 103);

		if (has_focus)
		{
			light.Set(205, 240, 226);
			sel.Set(82, 107, 213);
		}

		dc.SetPen(wxPen(light));
		dc.SetBrush(wxBrush(dark));
		dc.DrawRectangle(bounds);

		if (sel_length > 0 && sel_start >= 0)
		{
			wxRect r;
			r.x = sel_start * bounds.width / data_length;
			r.y = bounds.y;
			r.width = sel_length * bounds.width / data_length;
			r.height = bounds.height;

			dc.SetPen(wxPen(sel));
			dc.SetBrush(wxBrush(sel));
			dc.DrawRectangle(r);
		}

		dc.SetPen(wxPen(light));
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.DrawRectangle(bounds);

		dc.SetPen(wxPen(light));
		dc.SetBrush(wxBrush(light));
		dc.DrawRectangle(thumb);
	}
};



class AudioDisplayTimeline : public AudioDisplayInteractionObject {
	int64_t num_samples;
	int samplerate;
	int samples_per_pixel;
	int pixel_left;

	wxRect bounds;

	wxPoint drag_lastpos;
	bool dragging;

	enum Scale {
		Sc_Millisecond,
		Sc_Centisecond,
		Sc_Decisecond,
		Sc_Second,
		Sc_Decasecond,
		Sc_Minute,
		Sc_Decaminute,
		Sc_Hour,
		Sc_Decahour, // If anyone needs this they should reconsider their project
		Sc_MAX = Sc_Decahour
	};
	Scale scale_minor;
	int scale_major_modulo; // If minor_scale_mark_index % scale_major_modulo == 0 the mark is a major mark
	double scale_minor_divisor; // Absolute scale-mark index multiplied by this number gives sample index for scale mark

	AudioDisplay *display;

public:

	AudioDisplayTimeline(AudioDisplay *_display)
		: num_samples(0)
		, samplerate(44100)
		, samples_per_pixel(1)
		, pixel_left(0)
		, display(_display)
		, dragging(false)
	{
	}

	virtual ~AudioDisplayTimeline()
	{
	}

	int GetHeight() const
	{
		int width, height;
		display->GetTextExtent(_T("0123456789:."), &width, &height);
		return height + 4;
	}

	void SetDisplaySize(const wxSize &display_size)
	{
		// The size is without anything that goes below the timeline (like scrollbar)
		bounds.width = display_size.x;
		bounds.height = GetHeight();
		bounds.x = 0;
		bounds.y = 0;
	}

	const wxRect & GetBounds() const
	{
		return bounds;
	}

	void ChangeAudio(int64_t new_length, int new_samplerate)
	{
		num_samples = new_length;
		samplerate = new_samplerate;
	}

	void ChangeZoom(int new_pixel_samples)
	{
		samples_per_pixel = new_pixel_samples;

		// Pixels per second
		double px_sec = (double)samplerate / (double)samples_per_pixel;

		if (px_sec > 3000) {
			scale_minor = Sc_Millisecond;
			scale_minor_divisor = (double)samplerate / 1000;
			scale_major_modulo = 10;
		} else if (px_sec > 300) {
			scale_minor = Sc_Centisecond;
			scale_minor_divisor = (double)samplerate / 100;
			scale_major_modulo = 10;
		} else if (px_sec > 30) {
			scale_minor = Sc_Decisecond;
			scale_minor_divisor = (double)samplerate / 10;
			scale_major_modulo = 10;
		} else if (px_sec > 3) {
			scale_minor = Sc_Second;
			scale_minor_divisor = (double)samplerate;
			scale_major_modulo = 10;
		} else if (px_sec > 1.0/3.0) {
			scale_minor = Sc_Decasecond;
			scale_minor_divisor = (double)samplerate * 10;
			scale_major_modulo = 6;
		} else if (px_sec > 1.0/9.0) {
			scale_minor = Sc_Minute;
			scale_minor_divisor = (double)samplerate * 60;
			scale_major_modulo = 10;
		} else if (px_sec > 1.0/90.0) {
			scale_minor = Sc_Decaminute;
			scale_minor_divisor = (double)samplerate * 600;
			scale_major_modulo = 6;
		} else {
			scale_minor = Sc_Hour;
			scale_minor_divisor = (double)samplerate * 3600;
			scale_major_modulo = 10;
		}
	}

	void SetPosition(int new_pixel_left)
	{
		if (new_pixel_left < 0)
			new_pixel_left = 0;

		if (new_pixel_left != pixel_left)
		{
			pixel_left = new_pixel_left;
			display->ScrollPixelToLeft(pixel_left);
		}

	}

	bool OnMouseEvent(wxMouseEvent &event)
	{
		if (event.LeftDown())
		{
			drag_lastpos = event.GetPosition();
			dragging = true;
		}
		else if (event.LeftIsDown())
		{
			SetPosition(pixel_left - event.GetPosition().x + drag_lastpos.x);

			drag_lastpos = event.GetPosition();
			dragging = true;
		}
		else if (event.LeftUp())
		{
			dragging = false;
		}

		return dragging;
	}

	void Paint(wxDC &dc)
	{
		wxColour light(89, 145, 220);
		wxColour dark(8, 4, 13);

		int bottom = bounds.y + bounds.height;

		// Background
		dc.SetPen(wxPen(dark));
		dc.SetBrush(wxBrush(dark));
		dc.DrawRectangle(bounds);

		// Top line
		dc.SetPen(wxPen(light));
		dc.DrawLine(bounds.x, bottom-1, bounds.x+bounds.width, bottom-1);

		// Prepare for writing text
		dc.SetTextBackground(dark);
		dc.SetTextForeground(light);

		// Figure out the first scale mark to show
		int64_t sample_left = pixel_left * samples_per_pixel;
		int next_scale_mark = (int)(sample_left / scale_minor_divisor);
		if (next_scale_mark * scale_minor_divisor < sample_left)
			next_scale_mark += 1;
		assert(next_scale_mark * scale_minor_divisor >= sample_left);

		// Draw scale marks
		int next_scale_mark_pos;
		int last_text_right = -1;
		int last_hour = -1, last_minute = -1;
		if (num_samples / samplerate < 3600) last_hour = 0; // Trick to only show hours if audio is longer than 1 hour
		do {
			next_scale_mark_pos = (int)(next_scale_mark * scale_minor_divisor / samples_per_pixel) - pixel_left;
			bool mark_is_major = next_scale_mark % scale_major_modulo == 0;

			if (mark_is_major)
				dc.DrawLine(next_scale_mark_pos, bottom-6, next_scale_mark_pos, bottom);
			else
				dc.DrawLine(next_scale_mark_pos, bottom-4, next_scale_mark_pos, bottom);

			// Print time labels on major scale marks
			if (mark_is_major && next_scale_mark_pos > last_text_right)
			{
				double mark_time = next_scale_mark * scale_minor_divisor / samplerate;
				int mark_hour = (int)(mark_time / 3600);
				int mark_minute = (int)(mark_time / 60) % 60;
				double mark_second = mark_time - mark_hour*3600 - mark_minute*60;

				wxString time_string;
				bool changed_hour = mark_hour != last_hour;
				bool changed_minute = mark_minute != last_minute;

				if (changed_hour)
				{
					time_string = wxString::Format(_T("%d:%02d:"), mark_hour, mark_minute);
					last_hour = mark_hour;
					last_minute = mark_minute;
				}
				else if (changed_minute)
				{
					time_string = wxString::Format(_T("%d:"), mark_minute);
					last_minute = mark_minute;
				}
				if (scale_minor >= Sc_Decisecond)
					time_string += wxString::Format(_T("%02d"), (int)mark_second);
				else if (scale_minor == Sc_Centisecond)
					time_string += wxString::Format(_T("%02.1f"), mark_second);
				else
					time_string += wxString::Format(_T("%02.2f"), mark_second);

				int tw, th;
				dc.GetTextExtent(time_string, &tw, &th);
				last_text_right = next_scale_mark_pos + tw;

				dc.DrawText(time_string, next_scale_mark_pos, 0);
			}

			next_scale_mark += 1;

		} while (next_scale_mark_pos < bounds.width);
	}
};



class AudioMarkerInteractionObject : public AudioDisplayInteractionObject {
	// Object-pair being intracted with
	AudioMarker *marker;
	AudioTimingController *timing_controller;
	// From the audio display
	int scroll_left;
	int pixel_samples;
	// Mouse button used to initiate the drag
	wxMouseButton button_used;

public:
	AudioMarkerInteractionObject(AudioMarker *marker, AudioTimingController *timing_controller, int scroll_left, int pixel_samples, wxMouseButton button_used)
		: marker(marker)
		, timing_controller(timing_controller)
		, scroll_left(scroll_left)
		, pixel_samples(pixel_samples)
		, button_used(button_used)
	{
	}

	virtual ~AudioMarkerInteractionObject()
	{
	}

	virtual bool OnMouseEvent(wxMouseEvent &event)
	{
		if (event.Dragging())
		{
			int64_t sample_pos = (scroll_left + event.GetPosition().x) * pixel_samples;
			timing_controller->OnMarkerDrag(marker, sample_pos);
		}

		// We lose the marker drag if the button used to initiate it goes up
		return !event.ButtonUp(button_used);
	}
};




/// @brief Constructor 
/// @param parent 
///
AudioDisplay::AudioDisplay(wxWindow *parent, AudioController *_controller)
: wxWindow(parent, -1, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxBORDER_SIMPLE)
, controller(_controller)
, provider(0)
, dragged_object(0)
{
	scrollbar = new AudioDisplayScrollbar(this);
	timeline = new AudioDisplayTimeline(this);

	audio_renderer = new AudioRenderer;
	audio_spectrum_renderer = new AudioSpectrumRenderer;

	scroll_left = 0;
	pixel_audio_width = 0;
	scale_amplitude = 1.0;

	track_cursor_pos = -1;

	audio_renderer->SetRenderer(audio_spectrum_renderer);
	audio_renderer->SetAmplitudeScale(scale_amplitude);

	controller->AddAudioListener(this);
	controller->AddTimingListener(this);

	SetZoomLevel(0);

	ReloadRenderingSettings();

	SetMinClientSize(wxSize(-1, 70));
	SetBackgroundStyle(wxBG_STYLE_CUSTOM); // intended to be wxBG_STYLE_PAINT but that doesn't exist for me
	SetThemeEnabled(false);
}



/// @brief Destructor 
///
AudioDisplay::~AudioDisplay()
{
	delete audio_renderer;
	delete audio_spectrum_renderer;

	delete timeline;
	delete scrollbar;

	controller->RemoveAudioListener(this);
	controller->RemoveTimingListener(this);
}


void AudioDisplay::ScrollBy(int pixel_amount)
{
	ScrollPixelToLeft(scroll_left + pixel_amount);
}


void AudioDisplay::ScrollPixelToLeft(int pixel_position)
{
	const int client_width = GetClientRect().GetWidth();

	if (pixel_position + client_width >= pixel_audio_width)
		pixel_position = pixel_audio_width - client_width;
	if (pixel_position < 0)
		pixel_position = 0;

	// This check is required to avoid needless redraws, but more importantly to
	// avoid mutual recursion with the scrollbar and timeline.
	if (pixel_position != scroll_left)
	{
		scroll_left = pixel_position;
		scrollbar->SetPosition(scroll_left);
		timeline->SetPosition(scroll_left);
		Refresh();
	}
}


void AudioDisplay::SetZoomLevel(int new_zoom_level)
{
	zoom_level = new_zoom_level;

	if (!provider)
		return;

	const int samples_per_second = provider ? provider->GetSampleRate() : 48000;
	const int base_pixels_per_second = 50; /// @todo Make this customisable
	const int base_samples_per_pixel = samples_per_second / base_pixels_per_second;

	const int factor = GetZoomLevelFactor(zoom_level);

	const int new_samples_per_pixel = std::max(1, 100 * base_samples_per_pixel / factor);

	if (pixel_samples != new_samples_per_pixel)
	{
		pixel_samples = new_samples_per_pixel;
		audio_renderer->SetSamplesPerPixel(pixel_samples);

		if (provider)
			pixel_audio_width = provider->GetNumSamples() / pixel_samples + 1;
		else
			pixel_audio_width = 1;

		scrollbar->ChangeLengths(pixel_audio_width, GetClientSize().GetWidth());
		timeline->ChangeZoom(pixel_samples);

		Refresh();
	}
}


int AudioDisplay::GetZoomLevel() const
{
	return zoom_level;
}


wxString AudioDisplay::GetZoomLevelDescription(int level) const
{
	const int factor = GetZoomLevelFactor(level);
	const int base_pixels_per_second = 50; /// @todo Make this customisable along with the above
	const int second_pixels = 100 * base_pixels_per_second / factor;

	return wxString::Format(_("%d%%, %d pixel/second"), factor, second_pixels);
}


int AudioDisplay::GetZoomLevelFactor(int level)
{
	int factor = 100;

	if (level > 0)
	{
		factor += 25 * level;
	}
	else if (level < 0)
	{
		if (level >= -5)
			factor += 10 * level;
		else if (level >= -11)
			factor = 50 + (level+5) * 5;
		else
			factor = 20 + level + 11;
		if (factor <= 0)
			factor = 1;
	}

	return factor;
}


void AudioDisplay::SetAmplitudeScale(float scale)
{
	audio_renderer->SetAmplitudeScale(scale);
	Refresh();
}


float AudioDisplay::GetAmplitudeScale() const
{
	return audio_renderer->GetAmplitudeScale();
}


void AudioDisplay::ReloadRenderingSettings()
{
	int spectrum_quality = Options.AsInt(_T("Audio Spectrum Quality"));
#ifdef WITH_FFTW
	// FFTW is so fast we can afford to upgrade quality by two levels
	spectrum_quality += 2;
#endif
	if (spectrum_quality < 0) spectrum_quality = 0;
	if (spectrum_quality > 5) spectrum_quality = 5;

	// Quality indexes:        0  1  2  3   4   5
	int spectrum_width[]    = {8, 9, 9, 9, 10, 11};
	int spectrum_distance[] = {8, 8, 7, 6,  6,  5};

	audio_spectrum_renderer->SetResolution(
		spectrum_width[spectrum_quality],
		spectrum_distance[spectrum_quality]);
	audio_renderer->Invalidate();

	Refresh();
}


#pragma region Old code
//void AudioDisplay::UpdateImage(bool weak)


/// @brief Actually update the image on the display
///
/// This is where most actual drawing of the audio display happens, or other functions
/// to draw specific parts are called from.
/*
void AudioDisplay::DoUpdateImage() {
	// Loaded?
	if (!loaded || !provider) return;

	// Needs updating?
	if (!needImageUpdate) return;
	bool weak = needImageUpdateWeak;

	// Prepare bitmap
	int timelineHeight = Options.AsBool(_T("Audio Draw Timeline")) ? 20 : 0;
	int displayH = h+timelineHeight;
	if (origImage) {
		if (origImage->GetWidth() != w || origImage->GetHeight() != displayH) {
			delete origImage;
			origImage = NULL;
		}
	}

	// Options
	bool draw_boundary_lines = Options.AsBool(_T("Audio Draw Secondary Lines"));
	bool draw_selection_background = Options.AsBool(_T("Audio Draw Selection Background"));
	bool drawKeyframes = Options.AsBool(_T("Audio Draw Keyframes"));

	// Invalid dimensions
	if (w == 0 || displayH == 0) return;

	// New bitmap
	if (!origImage) origImage = new wxBitmap(w,displayH,-1);

	// Is spectrum?
	bool spectrum = false;
	if (provider && Options.AsBool(_T("Audio Spectrum"))) {
		spectrum = true;
	}

	// Draw image to be displayed
	wxMemoryDC dc;
	dc.SelectObject(*origImage);

	// Black background
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush(wxBrush(Options.AsColour(_T("Audio Background"))));
	dc.DrawRectangle(0,0,w,h);

	// Selection position
	hasSel = false;
	hasKaraoke = karaoke->enabled;
	selStart = 0;
	selEnd = 0;
	lineStart = 0;
	lineEnd = 0;
	selStartCap = 0;
	selEndCap = 0;
	int64_t drawSelStart = 0;
	int64_t drawSelEnd = 0;
	if (dialogue) {
		GetDialoguePos(lineStart,lineEnd,false);
		hasSel = true;
		if (hasKaraoke) {
			GetKaraokePos(selStartCap,selEndCap,true);
			GetKaraokePos(drawSelStart,drawSelEnd,false);
			selStart = lineStart;
			selEnd = lineEnd;
		}
		else {
			GetDialoguePos(selStartCap,selEndCap,true);
			selStart = lineStart;
			selEnd = lineEnd;
			drawSelStart = lineStart;
			drawSelEnd = lineEnd;
		}
	}

	// Draw selection bg
	if (hasSel && drawSelStart < drawSelEnd && draw_selection_background) {
		if (NeedCommit && !karaoke->enabled) dc.SetBrush(wxBrush(Options.AsColour(_T("Audio Selection Background Modified"))));
		else dc.SetBrush(wxBrush(Options.AsColour(_T("Audio Selection Background"))));
		dc.DrawRectangle(drawSelStart,0,drawSelEnd-drawSelStart,h);
	}

	// Draw spectrum
	if (spectrum) {
		DrawSpectrum(dc,weak);
	}

	// Waveform
	else if (provider) {
		DrawWaveform(dc,weak);
	}

	// Nothing
	else {
		dc.DrawLine(0,h/2,w,h/2);
	}

	// Draw seconds boundaries
	if (draw_boundary_lines) {
		int64_t start = Position*samples;
		int rate = provider->GetSampleRate();
		int pixBounds = rate / samples;
		dc.SetPen(wxPen(Options.AsColour(_T("Audio Seconds Boundaries")),1,wxDOT));
		if (pixBounds >= 8) {
			for (int x=0;x<w;x++) {
				if (((x*samples)+start) % rate < samples) {
					dc.DrawLine(x,0,x,h);
				}
			}
		}
	}

	// Draw current frame
	if (Options.AsBool(_T("Audio Draw Video Position"))) {
		if (VideoContext::Get()->IsLoaded()) {
			dc.SetPen(wxPen(Options.AsColour(_T("Audio Play Cursor")),2,wxLONG_DASH));
			int x = GetXAtMS(VFR_Output.GetTimeAtFrame(VideoContext::Get()->GetFrameN()));
			dc.DrawLine(x,0,x,h);
		}
	}

	// Draw keyframes
	if (drawKeyframes && VideoContext::Get()->KeyFramesLoaded()) {
		DrawKeyframes(dc);
	}

	// Draw previous line
	DrawInactiveLines(dc);

	if (hasSel) {
		// Draw boundaries
		if (true) {
			// Draw start boundary
			int selWidth = Options.AsInt(_T("Audio Line boundaries Thickness"));
			dc.SetPen(wxPen(Options.AsColour(_T("Audio Line boundary start"))));
			dc.SetBrush(wxBrush(Options.AsColour(_T("Audio Line boundary start"))));
			dc.DrawRectangle(lineStart-selWidth/2+1,0,selWidth,h);
			wxPoint points1[3] = { wxPoint(lineStart,0), wxPoint(lineStart+10,0), wxPoint(lineStart,10) };
			wxPoint points2[3] = { wxPoint(lineStart,h-1), wxPoint(lineStart+10,h-1), wxPoint(lineStart,h-11) };
			dc.DrawPolygon(3,points1);
			dc.DrawPolygon(3,points2);

			// Draw end boundary
			dc.SetPen(wxPen(Options.AsColour(_T("Audio Line boundary end"))));
			dc.SetBrush(wxBrush(Options.AsColour(_T("Audio Line boundary end"))));
			dc.DrawRectangle(lineEnd-selWidth/2+1,0,selWidth,h);
			wxPoint points3[3] = { wxPoint(lineEnd,0), wxPoint(lineEnd-10,0), wxPoint(lineEnd,10) };
			wxPoint points4[3] = { wxPoint(lineEnd,h-1), wxPoint(lineEnd-10,h-1), wxPoint(lineEnd,h-11) };
			dc.DrawPolygon(3,points3);
			dc.DrawPolygon(3,points4);
		}

		// Draw karaoke
		if (hasKaraoke) {
			try {
				// Prepare
				wxPen curPen(Options.AsColour(_T("Audio Syllable boundaries")),1,wxDOT);
				dc.SetPen(curPen);
				wxFont curFont(9,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Verdana"),wxFONTENCODING_SYSTEM);
				dc.SetFont(curFont);
				if (!spectrum) dc.SetTextForeground(Options.AsColour(_T("Audio Syllable text")));
				else dc.SetTextForeground(wxColour(255,255,255));
				size_t karn = karaoke->syllables.size();
				int64_t pos1,pos2;
				int len,curpos;
				wxCoord tw=0,th=0;
				AudioKaraokeSyllable *curSyl;
				wxString temptext;

				// Draw syllables
				for (size_t i=0;i<karn;i++) {
					curSyl = &karaoke->syllables.at(i);
					len = curSyl->duration*10;
					curpos = curSyl->start_time*10;
					if (len != -1) {
						pos1 = GetXAtMS(curStartMS+curpos);
						pos2 = GetXAtMS(curStartMS+len+curpos);
						dc.DrawLine(pos2,0,pos2,h);
						temptext = curSyl->text;
						temptext.Trim(true);
						temptext.Trim(false);
						GetTextExtent(temptext,&tw,&th,NULL,NULL,&curFont);
						dc.DrawText(temptext,(pos1+pos2-tw)/2,4);
					}
				}
			}
			catch (...) {
				// FIXME?
			}
		}
	}

	// Modified text
	if (NeedCommit) {
		dc.SetFont(wxFont(9,wxDEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Verdana"))); // FIXME: hardcoded font name
		dc.SetTextForeground(wxColour(255,0,0));
		if (selStart <= selEnd) {
			dc.DrawText(_T("Modified"),4,4);
		}
		else {
			dc.DrawText(_T("Negative time"),4,4);
		}
	}

	// Draw timescale
	if (timelineHeight) {
		DrawTimescale(dc);
	}

	// Draw selection border
	if (hasFocus) {
		dc.SetPen(*wxGREEN_PEN);
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.DrawRectangle(0,0,w,h);
	}

	// Done
	needImageUpdate = false;
	needImageUpdateWeak = true;
}
*/



/// @brief Draw other lines than the current active
/// @param dc The DC to draw to.
///
/// Draws markers for inactive lines, eg. the previous line, per configuration.
/*
void AudioDisplay::DrawInactiveLines(wxDC &dc) {
	// Check if there is anything to do
	int shadeType = Options.AsInt(_T("Audio Inactive Lines Display Mode"));
	if (shadeType == 0) return;

	// Spectrum?
	bool spectrum = false;
	if (provider && Options.AsBool(_T("Audio Spectrum"))) {
		spectrum = true;
	}

	// Set options
	dc.SetBrush(wxBrush(Options.AsColour(_T("Audio Line boundary inactive line"))));
	int selWidth = Options.AsInt(_T("Audio Line boundaries Thickness"));
	AssDialogue *shade;
	int shadeX1,shadeX2;
	int shadeFrom,shadeTo;

	// Only previous
	if (shadeType == 1) {
		shadeFrom = this->line_n-1;
		shadeTo = shadeFrom+1;
	}

	// All
	else {
		shadeFrom = 0;
		shadeTo = grid->GetRows();
	}
	
	for (int j=shadeFrom;j<shadeTo;j++) {
		if (j == line_n) continue;
		if (j < 0) continue;
		shade = grid->GetDialogue(j);

		if (shade) {
			// Get coordinates
			shadeX1 = GetXAtMS(shade->Start.GetMS());
			shadeX2 = GetXAtMS(shade->End.GetMS());
			if (shadeX2 < 0 || shadeX1 > w) continue;

			// Draw over waveform
			if (!spectrum) {
				// Selection
				int selX1 = MAX(0,GetXAtMS(curStartMS));
				int selX2 = MIN(w,GetXAtMS(curEndMS));

				// Get ranges (x1->x2, x3->x4).
				int x1 = MAX(0,shadeX1);
				int x2 = MIN(w,shadeX2);
				int x3 = MAX(x1,selX2);
				int x4 = MAX(x2,selX2);

				// Clip first range
				x1 = MIN(x1,selX1);
				x2 = MIN(x2,selX1);

				// Set pen and draw
				dc.SetPen(wxPen(Options.AsColour(_T("Audio Waveform Inactive"))));
				for (int i=x1;i<x2;i++) dc.DrawLine(i,peak[i],i,min[i]-1);
				for (int i=x3;i<x4;i++) dc.DrawLine(i,peak[i],i,min[i]-1);
			}

			// Draw boundaries
			dc.SetPen(wxPen(Options.AsColour(_T("Audio Line boundary inactive line"))));
			dc.DrawRectangle(shadeX1-selWidth/2+1,0,selWidth,h);
			dc.DrawRectangle(shadeX2-selWidth/2+1,0,selWidth,h);
		}
	}
}*/



/// @brief Draw keyframe markers
/// @param dc The DC to draw to.
/*
void AudioDisplay::DrawKeyframes(wxDC &dc) {
	wxArrayInt KeyFrames = VideoContext::Get()->GetKeyFrames();
	int nKeys = (int)KeyFrames.Count();
	dc.SetPen(wxPen(wxColour(255,0,255),1));

	// Get min and max frames to care about
	int minFrame = VFR_Output.GetFrameAtTime(GetMSAtX(0),true);
	int maxFrame = VFR_Output.GetFrameAtTime(GetMSAtX(w),true);

	// Scan list
	for (int i=0;i<nKeys;i++) {
		int cur = KeyFrames[i];
		if (cur >= minFrame && cur <= maxFrame) {
			int x = GetXAtMS(VFR_Output.GetTimeAtFrame(cur,true));
			dc.DrawLine(x,0,x,h);
		}
		else if (cur > maxFrame) break;
	}
}*/



/// @brief Draw timescale at bottom of audio display
/// @param dc The DC to draw to.
/*
void AudioDisplay::DrawTimescale(wxDC &dc) {
	// Set size
	int timelineHeight = Options.AsBool(_T("Audio Draw Timeline")) ? 20 : 0;

	// Set colours
	dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.DrawRectangle(0,h,w,timelineHeight);
	dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
	dc.DrawLine(0,h,w,h);
	dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DHIGHLIGHT));
	dc.DrawLine(0,h+1,w,h+1);
	dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
	dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
	wxFont scaleFont;
	scaleFont.SetFaceName(_T("Tahoma")); // FIXME: hardcoded font name
	scaleFont.SetPointSize(8);
	dc.SetFont(scaleFont);

	// Timescale ticks
	int64_t start = Position*samples;
	int rate = provider->GetSampleRate();
	for (int i=1;i<32;i*=2) {
		int pixBounds = rate / (samples * 4 / i);
		if (pixBounds >= 8) {
			for (int x=0;x<w;x++) {
				int64_t pos = (x*samples)+start;
				// Second boundary
				if (pos % rate < samples) {
					dc.DrawLine(x,h+2,x,h+8);

					// Draw text
					wxCoord textW,textH;
					int hr = 0;
					int m = 0;
					int s = pos/rate;
					while (s >= 3600) {
						s -= 3600;
						hr++;
					}
					while (s >= 60) {
						s -= 60;
						m++;
					}
					wxString text;
					if (hr) text = wxString::Format(_T("%i:%02i:%02i"),hr,m,s);
					else if (m) text = wxString::Format(_T("%i:%02i"),m,s);
					else text = wxString::Format(_T("%i"),s);
					dc.GetTextExtent(text,&textW,&textH,NULL,NULL,&scaleFont);
					dc.DrawText(text,MAX(0,x-textW/2)+1,h+8);
				}

				// Other
				else if (pos % (rate / 4 * i) < samples) {
					dc.DrawLine(x,h+2,x,h+5);
				}
			}
			break;
		}
	}
}*/



/// @brief Draw audio waveform
/// @param dc   The DC to draw to.
/// @param weak False if the visible portion of the display has changed.
/*
void AudioDisplay::DrawWaveform(wxDC &dc,bool weak) {
	// Prepare Waveform
	if (!weak || peak == NULL || min == NULL) {
		if (peak) delete[] peak;
		if (min) delete[] min;
		peak = new int[w];
		min = new int[w];
	}

	// Get waveform
	if (!weak) {
		provider->GetWaveForm(min,peak,Position*samples,w,h,samples,scale);
	}

	// Draw pre-selection
	if (!hasSel) selStartCap = w;
	dc.SetPen(wxPen(Options.AsColour(_T("Audio Waveform"))));
	for (int64_t i=0;i<selStartCap;i++) {
		dc.DrawLine(i,peak[i],i,min[i]-1);
	}

	if (hasSel) {
		// Draw selection
		if (Options.AsBool(_T("Audio Draw Selection Background"))) {
			if (NeedCommit && !karaoke->enabled) dc.SetPen(wxPen(Options.AsColour(_T("Audio Waveform Modified"))));
			else dc.SetPen(wxPen(Options.AsColour(_T("Audio Waveform Selected"))));
		}
		for (int64_t i=selStartCap;i<selEndCap;i++) {
			dc.DrawLine(i,peak[i],i,min[i]-1);
		}

		// Draw post-selection
		dc.SetPen(wxPen(Options.AsColour(_T("Audio Waveform"))));
		for (int64_t i=selEndCap;i<w;i++) {
			dc.DrawLine(i,peak[i],i,min[i]-1);
		}
	}
}*/



/// @brief Load from file 
/// @param file 
/// @return 
///
/*
void AudioDisplay::SetFile(wxString file) {
	wxLogDebug(_T("AudioDisplay::SetFile(file=%s)"), file.c_str());
	// Unload
	if (file.IsEmpty()) try {
		wxLogDebug(_T("AudioDisplay::SetFile: file is empty, just closing audio"));
		try {
			if (player) player->CloseStream();
		}
		catch (const wxChar *e) {
			wxLogError(e);
		}
		delete provider;
		delete player;
		delete spectrumRenderer;
		provider = NULL;
		player = NULL;
		spectrumRenderer = NULL;
		try {
			Reset();
		}
		catch (const wxChar *e) {
			wxLogError(e);
		}

		loaded = false;
		temporary = false;
		StandardPaths::SetPathValue(_T("?audio"),_T(""));
	}
	catch (wxString e) {
		wxLogError(e);
	}
	catch (const wxChar *e) {
		wxLogError(e);
	}
	catch (...) {
		wxLogError(_T("Unknown error unloading audio"));
	}

	// Load
	else {
		wxLogDebug(_T("AudioDisplay::SetFile: unloading old file"));
		SetFile(_T(""));
		try {
			// Get provider
			wxLogDebug(_T("AudioDisplay::SetFile: get audio provider"));
			bool is_dummy = false;
			if (file == _T("?dummy")) {
				is_dummy = true;
				provider = new DummyAudioProvider(150*60*1000, false); // 150 minutes non-noise
			} else if (file == _T("?noise")) {
				is_dummy = true;
				provider = new DummyAudioProvider(150*60*1000, true); // 150 minutes noise
			} else {
				provider = AudioProviderFactoryManager::GetAudioProvider(file);
			}

			// Get player
			wxLogDebug(_T("AudioDisplay::SetFile: get audio player"));
			player = AudioPlayerFactoryManager::GetAudioPlayer();
			player->SetDisplayTimer(&UpdateTimer);
			player->SetProvider(provider);
			player->OpenStream();
			loaded = true;

			// Add to recent
			if (!is_dummy) {
				wxLogDebug(_T("AudioDisplay::SetFile: add to recent"));
				Options.AddToRecentList(file,_T("Recent aud"));
				wxFileName fn(file);
				StandardPaths::SetPathValue(_T("?audio"),fn.GetPath());
			}

			// Update
			UpdateImage();
		}
		catch (const wxChar *e) {
			if (player) { delete player; player = 0; }
			if (provider) { delete provider; provider = 0; }
			wxLogError(e);
		}
		catch (wxString &err) {
			if (player) { delete player; player = 0; }
			if (provider) { delete provider; provider = 0; }
			wxLogDebug(_T("AudioDisplay::SetFile: gotcha!"));
			wxMessageBox(err,_T("Error loading audio"),wxICON_ERROR | wxOK);
		}
		catch (...) {
			if (player) { delete player; player = 0; }
			if (provider) { delete provider; provider = 0; }
			wxLogError(_T("Unknown error loading audio"));
		}
	}
	
	if (!loaded) return;

	assert(loaded == (provider != NULL));

	// Set default selection
	wxLogDebug(_T("AudioDisplay::SetFile: set default selection"));
	int n = grid->editBox->linen;
	SetDialogue(grid,grid->GetDialogue(n),n);
	wxLogDebug(_T("AudioDisplay::SetFile: returning"));
}
*/



/// @brief Load from video 
///
/*
void AudioDisplay::SetFromVideo() {
	wxLogDebug(_T("AudioDisplay::SetFromVideo"));
	if (VideoContext::Get()->IsLoaded()) {
		wxString extension = VideoContext::Get()->videoName.Right(4);
		extension.LowerCase();

		if (extension != _T(".d2v")) SetFile(VideoContext::Get()->videoName);
	}
}
*/


/// @brief Play 
/// @param start 
/// @param end   
/// @return 
///
/*
void AudioDisplay::Play(int start,int end) {
	wxLogDebug(_T("AudioDisplay::Play"));
	Stop();

	// Check provider
	if (!provider) {
		wxLogDebug(_T("AudioDisplay::Play: no audio provider"));
		// Load temporary provider from video
		if (VideoContext::Get()->IsLoaded()) {
			wxLogDebug(_T("AudioDisplay::Play: has video provider"));
			try {
				// Get provider
				if (!VideoContext::Get()->videoName.StartsWith(_T("?dummy")))
					provider = AudioProviderFactoryManager::GetAudioProvider(VideoContext::Get()->videoName, 0);
				else
					return;

				// Get player
				player = AudioPlayerFactoryManager::GetAudioPlayer();
				player->SetDisplayTimer(&UpdateTimer);
				player->SetProvider(provider);
				player->OpenStream();
				temporary = true;
				wxLogDebug(_T("AudioDisplay::Play: got temp audio provider from video provider"));
			}
			catch (...) {
				wxLogDebug(_T("AudioDisplay::Play: exception getting audio provider from video, returning"));
				return;
			}
		}
		if (!provider) {
			wxLogDebug(_T("AudioDisplay::Play: has no provider, returning"));
			return;
		}
	}

	// Set defaults
	wxLogDebug(_T("AudioDisplay::Play: initialising playback"));
	playingToEnd = end < 0;
	int64_t num_samples = provider->GetNumSamples();
	start = GetSampleAtMS(start);
	if (end != -1) end = GetSampleAtMS(end);
	else end = num_samples-1;

	// Sanity checking
	if (start < 0) start = 0;
	if (start >= num_samples) start = num_samples-1;
	if (end >= num_samples) end = num_samples-1;
	if (end < start) end = start;

	// Redraw the image to avoid any junk left over from mouse movements etc
	// See issue #598
	UpdateImage(true);

	// Call play
	player->Play(start,end-start);
	wxLogDebug(_T("AudioDisplay::Play: playback started, returning"));
}*/



/// @brief Set dialogue 
/// @param _grid 
/// @param diag  
/// @param n     
/// @return 
///
/*
void AudioDisplay::SetDialogue(SubtitlesGrid *_grid,AssDialogue *diag,int n) {
	wxLogDebug(_T("AudioDisplay::SetDialogue"));
	// Actual parameters
	if (_grid) {
		wxLogDebug(_T("AudioDisplay::SetDialogue: has grid"));
		// Set variables
		grid = _grid;
		line_n = n;
		dialogue = diag;

		// Set flags
		diagUpdated = false;
		NeedCommit = false;

		// Set times
		if (dialogue && !dontReadTimes && Options.AsBool(_T("Audio grab times on select"))) {
			wxLogDebug(_T("AudioDisplay::SetDialogue: grabbing times"));
			int s = dialogue->Start.GetMS();
			int e = dialogue->End.GetMS();

			// Never do it for 0:00:00.00->0:00:00.00 lines
			if (s != 0 || e != 0) {
				curStartMS = s;
				curEndMS = e;
			}
		}
	}

	// Read karaoke data
	if (dialogue && karaoke->enabled) {
		wxLogDebug(_T("AudioDisplay::SetDialogue: in karaoke mode, loading new line into karaoke control"));
		NeedCommit = karaoke->LoadFromDialogue(dialogue);

		// Reset karaoke pos
		wxLogDebug(_T("AudioDisplay::SetDialogue: resetting karaoke position"));
		if (karaoke->curSyllable == -1) karaoke->SetSyllable((int)karaoke->syllables.size()-1);
		else karaoke->SetSyllable(0);
	}

	// Update
	Update();
	wxLogDebug(_T("AudioDisplay::SetDialogue: returning"));
}*/



/// @brief Commit changes 
/// @param nextLine 
/// @return 
///
/*
void AudioDisplay::CommitChanges (bool nextLine) {
	wxLogDebug(_T("AudioDisplay::CommitChanges(nextLine=%d)"), nextLine?1:0);
	// Loaded?
	if (!loaded) return;

	// Check validity
	bool textNeedsCommit = grid->GetDialogue(line_n)->Text != grid->editBox->TextEdit->GetText();
	bool timeNeedsCommit = grid->GetDialogue(line_n)->Start.GetMS() != curStartMS || grid->GetDialogue(line_n)->End.GetMS() != curEndMS;
	if (timeNeedsCommit || textNeedsCommit) NeedCommit = true;
	bool wasKaraSplitting = false;
	bool validCommit = true;
	if (!karaoke->enabled && !karaoke->splitting) {
		if (!NeedCommit || curEndMS < curStartMS) validCommit = false;
	}

	// Update karaoke
	int karaSelStart = 0, karaSelEnd = -1;
	if (karaoke->enabled) {
		wxLogDebug(_T("AudioDisplay::CommitChanges: karaoke enabled, committing it"));
		wasKaraSplitting = karaoke->splitting;
		karaoke->Commit();
		// Get karaoke selection
		karaSelStart = karaoke->syllables.size();
		for (size_t k = 0; k < karaoke->syllables.size(); ++k) {
			if (karaoke->syllables[k].selected) {
				if ((signed)k < karaSelStart) karaSelStart = k;
				if ((signed)k > karaSelEnd) karaSelEnd = k;
			}
		}
		wxLogDebug(_T("AudioDisplay::CommitChanges: karaSelStart=%d karaSelEnd=%d"), karaSelStart, karaSelEnd);
	}
	
	// Get selected rows
	wxArrayInt sel = grid->GetSelection();

	// Commit ok?
	if (validCommit) {
		wxLogDebug(_T("AudioDisplay::CommitChanges: valid commit"));
		// Reset flags
		diagUpdated = false;
		NeedCommit = false;

		// Update dialogues
		blockUpdate = true;
		AssDialogue *curDiag;
		for (size_t i=0;i<sel.GetCount();i++) {
			if (grid->IsInSelection(line_n)) curDiag = grid->GetDialogue(sel[i]);
			else curDiag = grid->GetDialogue(line_n);
			if (timeNeedsCommit) {
				curDiag->Start.SetMS(curStartMS);
				curDiag->End.SetMS(curEndMS);
			}
			if (!karaoke->enabled && textNeedsCommit) {
				// If user was editing karaoke stuff, that should take precedence of manual changes in the editbox,
				// so only update from editbox when not in kara mode
				curDiag->Text = grid->editBox->TextEdit->GetText();
			}
			curDiag->UpdateData();
			if (!grid->IsInSelection(line_n)) break;
		}

		// Update edit box
		wxLogDebug(_T("AudioDisplay::CommitChanges: updating time edit boxes"));
		grid->editBox->StartTime->Update();
		grid->editBox->EndTime->Update();
		grid->editBox->Duration->Update();

		// Update grid
		wxLogDebug(_T("AudioDisplay::CommitChanges: update grid"));
		grid->editBox->Update(!karaoke->enabled);
		grid->ass->FlagAsModified(_T(""));
		grid->CommitChanges();
		karaoke->SetSelection(karaSelStart, karaSelEnd);
		blockUpdate = false;
	}

	// Next line (ugh what a condition, can this be simplified?)
	if (nextLine && !karaoke->enabled && Options.AsBool(_T("Audio Next Line on Commit")) && !wasKaraSplitting) {
		wxLogDebug(_T("AudioDisplay::CommitChanges: going to next line"));
		// Insert a line if it doesn't exist
		int nrows = grid->GetRows();
		if (nrows == line_n + 1) {
			wxLogDebug(_T("AudioDisplay::CommitChanges: was on last line, inserting new"));
			AssDialogue *def = new AssDialogue;
			def->Start = grid->GetDialogue(line_n)->End;
			def->End = grid->GetDialogue(line_n)->End;
			def->End.SetMS(def->End.GetMS()+Options.AsInt(_T("Timing Default Duration")));
			def->Style = grid->GetDialogue(line_n)->Style;
			grid->InsertLine(def,line_n,true);
			curStartMS = curEndMS;
			curEndMS = curStartMS + Options.AsInt(_T("Timing Default Duration"));
		}
		else if (grid->GetDialogue(line_n+1)->Start.GetMS() == 0 && grid->GetDialogue(line_n+1)->End.GetMS() == 0) {
			curStartMS = curEndMS;
			curEndMS = curStartMS + Options.AsInt(_T("Timing Default Duration"));
		}
		else {
			curStartMS = grid->GetDialogue(line_n+1)->Start.GetMS();
			curEndMS = grid->GetDialogue(line_n+1)->End.GetMS();
		}
		
		// Go to next
		dontReadTimes = true;
		ChangeLine(1,sel.GetCount() > 1 ? true : false);
		dontReadTimes = false;
	}

	Update();
	wxLogDebug(_T("AudioDisplay::CommitChanges: returning"));
}*/
#pragma endregion



///////////////
// Event table
BEGIN_EVENT_TABLE(AudioDisplay, wxWindow)
    EVT_MOUSE_EVENTS(AudioDisplay::OnMouseEvent)
    EVT_PAINT(AudioDisplay::OnPaint)
	EVT_SIZE(AudioDisplay::OnSize)
	EVT_KEY_DOWN(AudioDisplay::OnKeyDown)
	EVT_SET_FOCUS(AudioDisplay::OnFocus)
	EVT_KILL_FOCUS(AudioDisplay::OnFocus)
END_EVENT_TABLE()



/// @brief Paint 
/// @param event 
/// @return 
///
void AudioDisplay::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);

	if (!provider)
	{
		dc.SetBackground(*wxBLACK_BRUSH);
		dc.Clear();
		return;
	}

	int client_width, client_height;
	GetClientSize(&client_width, &client_height);

	wxRect audio_bounds(0, audio_top, client_width, audio_height);
	const wxRect &scrollbar_bounds = scrollbar->GetBounds();
	const wxRect &timeline_bounds = timeline->GetBounds();
	bool redraw_scrollbar = false;
	bool redraw_timeline = false;

	AudioController::SampleRange sel_samples(controller->GetSelection());
	int selection_start = (int)(sel_samples.begin() / pixel_samples);
	int selection_end = (int)(sel_samples.end() / pixel_samples);

	// Draw the tracking cursor first, to avoid it ever disappearing, because that
	// causes optical flicker. It looks better to have two cursors momentarily than
	// having no cursor momentarily.
	if (track_cursor_pos >= 0)
	{
		wxDCPenChanger penchanger(dc, wxPen(*wxWHITE));
		dc.DrawLine(track_cursor_pos-scroll_left, audio_top, track_cursor_pos-scroll_left, audio_top+audio_height);
	}

	wxRegionIterator region(GetUpdateRegion());
	while (region)
	{
		wxRect updrect = region.GetRect();

		redraw_scrollbar |= scrollbar_bounds.Intersects(updrect);
		redraw_timeline |= timeline_bounds.Intersects(updrect);

		if (audio_bounds.Intersects(updrect))
		{
			int p1, p2, p3, p4;
			// p1 -> p2 = before selection
			// p2 -> p3 = in selection
			// p3 -> p4 = after selection
			p1 = scroll_left + updrect.x;
			p2 = selection_start;
			p3 = selection_end;
			p4 = p1 + updrect.width;

			struct subregion {
				int x1, x2;
				bool selected;
				subregion(int x1, int x2, bool selected) : x1(x1), x2(x2), selected(selected) { }
			};
			std::vector<subregion> subregions;

			if (p1 < p2)
				subregions.push_back(subregion(p1, std::min(p2, p4), false));
			if (p4 > p2 && p1 < p3)
				subregions.push_back(subregion(std::max(p1, p2), std::min(p3, p4), true));
			if (p4 > p3)
				subregions.push_back(subregion(std::max(p1, p3), p4, false));

			int x = updrect.x;

			for (std::vector<subregion>::iterator sr = subregions.begin(); sr != subregions.end(); ++sr)
			{
				// Check if the track cursor is inside this redraw region and draw around it, to avoid overdraw
				if (track_cursor_pos >= sr->x1 && track_cursor_pos < sr->x2)
				{
					int tcp_x = track_cursor_pos - sr->x1;
					if (tcp_x > 0)
						audio_renderer->Render(dc, wxPoint(x, audio_top), sr->x1, tcp_x, sr->selected);
					if (track_cursor_pos < sr->x2 - 1)
						audio_renderer->Render(dc, wxPoint(x+tcp_x+1, audio_top), track_cursor_pos+1, sr->x2-track_cursor_pos-1, sr->selected);
				}
				else
				{
					audio_renderer->Render(dc, wxPoint(x, audio_top), sr->x1, sr->x2 - sr->x1, sr->selected);
				}

				x += sr->x2 - sr->x1;
			}
		}

		region++;
	}

	if (redraw_scrollbar)
		scrollbar->Paint(dc, HasFocus());
	if (redraw_timeline)
		timeline->Paint(dc);
}


void AudioDisplay::SetDraggedObject(AudioDisplayInteractionObject *new_obj)
{
	// Special case for audio markers being dragged: they use a temporary wrapper object
	// which must be deleted when it is no longer used.
	AudioMarkerInteractionObject *dragged_marker = dynamic_cast<AudioMarkerInteractionObject*>(dragged_object);
	if (dragged_marker)
		delete dragged_marker;

	dragged_object = new_obj;

	if (dragged_object && !HasCapture())
		CaptureMouse();
	else if (!dragged_object && HasCapture())
		ReleaseMouse();
}


void AudioDisplay::SetTrackCursor(int new_pos, bool show_time)
{
	if (new_pos != track_cursor_pos)
	{
		int old_pos = track_cursor_pos;
		track_cursor_pos = new_pos;

		RefreshRect(wxRect(old_pos - scroll_left - 0, audio_top, 1, audio_height));
		RefreshRect(wxRect(new_pos - scroll_left - 0, audio_top, 1, audio_height));

		AssTime new_label_time;
		new_label_time.SetMS(track_cursor_pos * pixel_samples);
		track_cursor_label = new_label_time.GetASSFormated();
		/// @todo Figure out a sensible way to handle the label bounding rect
	}
}


void AudioDisplay::RemoveTrackCursor()
{
	SetTrackCursor(-1, false);
}



/// @brief Handle mouse input
/// @param event Event data
void AudioDisplay::OnMouseEvent(wxMouseEvent& event)
{
	// Check for mouse wheel scrolling
	if (event.GetWheelRotation() != 0)
	{
		// First check if the cursor is inside or outside the display.
		// If it's outside, we want to send the event to the control it's over instead.
		/// @todo Factor this into a reusable function
		{
			wxWindow *targetwindow = wxFindWindowAtPoint(event.GetPosition());
			if (targetwindow && targetwindow != this)
			{
				targetwindow->GetEventHandler()->ProcessEvent(event);
				event.Skip(false);
				return;
			}
		}

		bool zoom = event.CmdDown();
		if (Options.AsBool(_T("Audio Wheel Default To Zoom"))) zoom = !zoom;

		if (!zoom)
		{
			int amount = -event.GetWheelRotation();
			// If the user did a horizontal scroll the amount should be inverted
			// for it to be natural.
			if (event.GetWheelAxis() == 1) amount = -amount;

			// Reset any accumulated zoom
			mouse_zoom_accum = 0;

			ScrollBy(amount);
		}
		else if (event.GetWheelAxis() == 0)
		{
			mouse_zoom_accum += event.GetWheelRotation();
			int zoom_delta = mouse_zoom_accum / event.GetWheelDelta();
			mouse_zoom_accum %= event.GetWheelDelta();
			SetZoomLevel(GetZoomLevel() + zoom_delta);
			/// @todo This has to update the trackbar in the audio box... maybe move handling mouse zoom to
			/// the audio box instead to avoid messing with friend classes?
		}

		// Scroll event processed
		return;
	}

	if (event.IsButton())
		SetFocus();

	// Handle any ongoing drag
	if (dragged_object && HasCapture())
	{
		if (!dragged_object->OnMouseEvent(event))
		{
			SetDraggedObject(0);
			SetCursor(wxNullCursor);
		}
		return;
	}
	else
	{
		// Something is wrong, we might have lost capture somehow.
		// Fix state and pretend it didn't happen.
		SetDraggedObject(0);
		SetCursor(wxNullCursor);
	}

	wxPoint mousepos = event.GetPosition();

	// Check for scrollbar action
	if (scrollbar->GetBounds().Contains(mousepos))
	{
		if (!controller->IsPlaying())
			RemoveTrackCursor();
		if (scrollbar->OnMouseEvent(event))
			SetDraggedObject(scrollbar);
		return;
	}

	// Check for timeline action
	if (timeline->GetBounds().Contains(mousepos))
	{
		SetCursor(wxCursor(wxCURSOR_SIZEWE));
		if (!controller->IsPlaying())
			RemoveTrackCursor();
		if (timeline->OnMouseEvent(event))
			SetDraggedObject(timeline);
		return;
	}

	AudioTimingController *timing = controller->GetTimingController();
	int drag_sensitivity = pixel_samples*3; /// @todo Make this depend on configuration

	// Not scrollbar, not timeline, no button action
	if (event.Moving())
	{
		if (timing)
		{
			int64_t samplepos = (scroll_left + mousepos.x) * pixel_samples;

			if (timing->IsNearbyMarker(samplepos, drag_sensitivity))
				SetCursor(wxCursor(wxCURSOR_SIZEWE));
			else
				SetCursor(wxNullCursor);
		}

		if (!controller->IsPlaying())
			SetTrackCursor(scroll_left + mousepos.x, true);
		else
			RemoveTrackCursor();
	}

	if (event.LeftDown() && timing)
	{
		int64_t samplepos = (scroll_left + mousepos.x) * pixel_samples;
		AudioMarker *marker = timing->OnLeftClick(samplepos, drag_sensitivity);

		if (marker)
		{
			RemoveTrackCursor();
			SetDraggedObject(new AudioMarkerInteractionObject(
				marker, timing,
				scroll_left, pixel_samples,
				wxMOUSE_BTN_LEFT));
			return;
		}
	}

	if (event.RightDown() && timing)
	{
		int64_t samplepos = (scroll_left + mousepos.x) * pixel_samples;
		AudioMarker *marker = timing->OnRightClick(samplepos, drag_sensitivity);

		if (marker)
		{
			RemoveTrackCursor();
			SetDraggedObject(new AudioMarkerInteractionObject(
				marker, timing,
				scroll_left, pixel_samples,
				wxMOUSE_BTN_RIGHT));
			return;
		}
	}

	/// @todo Handle middle click to seek video
}



/// @brief The audio display was resized
/// @param event Event data
///
/// Tell our renderer about the new size and repaint ourselves.
void AudioDisplay::OnSize(wxSizeEvent &event)
{
	wxSize size = GetClientSize();

	scrollbar->SetDisplaySize(size);
	timeline->SetDisplaySize(wxSize(size.x, scrollbar->GetBounds().y));

	audio_height = size.GetHeight();
	audio_height -= scrollbar->GetBounds().GetHeight();
	audio_height -= timeline->GetHeight();
	audio_renderer->SetHeight(audio_height);

	audio_top = timeline->GetHeight();

	Refresh();
}


/// @brief The audio display received or lost input focus
void AudioDisplay::OnFocus(wxFocusEvent &event)
{
	// The scrollbar indicates focus so repaint that
	RefreshRect(scrollbar->GetBounds());
}


/// @brief Key down 
/// @param event 
///
void AudioDisplay::OnKeyDown(wxKeyEvent &event) {
#pragma region Old code
	/*
	int key = event.GetKeyCode();
#ifdef __APPLE__
	Hotkeys.SetPressed(key,event.m_metaDown,event.m_altDown,event.m_shiftDown);
#else
	Hotkeys.SetPressed(key,event.m_controlDown,event.m_altDown,event.m_shiftDown);
#endif

	// Accept
	if (Hotkeys.IsPressed(_T("Audio Commit"))) {
		CommitChanges(true);
		//ChangeLine(1);
	}

	// Accept (SSA's "Grab times")
	if (Hotkeys.IsPressed(_T("Audio Commit Alt"))) {
		CommitChanges(true);
	}

	// Accept (stay)
	if (Hotkeys.IsPressed(_T("Audio Commit (Stay)"))) {
		CommitChanges();
	}

	// Previous
	if (Hotkeys.IsPressed(_T("Audio Prev Line")) || Hotkeys.IsPressed(_T("Audio Prev Line Alt"))) {
		Prev();
	}

	// Next
	if (Hotkeys.IsPressed(_T("Audio Next Line")) || Hotkeys.IsPressed(_T("Audio Next Line Alt"))) {
		Next();
	}

	// Play
	if (Hotkeys.IsPressed(_T("Audio Play")) || Hotkeys.IsPressed(_T("Audio Play Alt"))) {
		int start=0,end=0;
		GetTimesSelection(start,end);
		Play(start,end);
	}

	// Play/Stop
	if (Hotkeys.IsPressed(_T("Audio Play or Stop"))) {
		if (player->IsPlaying()) Stop();
		else {
			int start=0,end=0;
			GetTimesSelection(start,end);
			Play(start,end);
		}
	}

	// Stop
	if (Hotkeys.IsPressed(_T("Audio Stop"))) {
		Stop();
	}

	// Increase length
	if (Hotkeys.IsPressed(_T("Audio Karaoke Increase Len"))) {
		if (karaoke->enabled) {
			bool result = karaoke->SyllableDelta(karaoke->curSyllable,1,0);
			if (result) diagUpdated = true;
		}
	}

	// Increase length (shift)
	if (Hotkeys.IsPressed(_T("Audio Karaoke Increase Len Shift"))) {
		if (karaoke->enabled) {
			bool result = karaoke->SyllableDelta(karaoke->curSyllable,1,1);
			if (result) diagUpdated = true;
		}
	}

	// Decrease length
	if (Hotkeys.IsPressed(_T("Audio Karaoke Decrease Len"))) {
		if (karaoke->enabled) {
			bool result = karaoke->SyllableDelta(karaoke->curSyllable,-1,0);
			if (result) diagUpdated = true;
		}
	}

	// Decrease length (shift)
	if (Hotkeys.IsPressed(_T("Audio Karaoke Decrease Len Shift"))) {
		if (karaoke->enabled) {
			bool result = karaoke->SyllableDelta(karaoke->curSyllable,-1,1);
			if (result) diagUpdated = true;
		}
	}

	// Move backwards
	if (Hotkeys.IsPressed(_T("Audio Scroll Left"))) {
		UpdatePosition(Position-128,false);
		UpdateImage();
	}

	// Move forward
	if (Hotkeys.IsPressed(_T("Audio Scroll Right"))) {
		UpdatePosition(Position+128,false);
		UpdateImage();
	}

	// Play first 500 ms
	if (Hotkeys.IsPressed(_T("Audio Play First 500ms"))) {
		int start=0,end=0;
		GetTimesSelection(start,end);
		int e = start+500;
		if (e > end) e = end;
		Play(start,e);
	}

	// Play last 500 ms
	if (Hotkeys.IsPressed(_T("Audio Play Last 500ms"))) {
		int start=0,end=0;
		GetTimesSelection(start,end);
		int s = end-500;
		if (s < start) s = start;
		Play(s,end);
	}

	// Play 500 ms before
	if (Hotkeys.IsPressed(_T("Audio Play 500ms Before"))) {
		int start=0,end=0;
		GetTimesSelection(start,end);
		Play(start-500,start);
	}

	// Play 500 ms after
	if (Hotkeys.IsPressed(_T("Audio Play 500ms After"))) {
		int start=0,end=0;
		GetTimesSelection(start,end);
		Play(end,end+500);
	}

	// Play to end of file
	if (Hotkeys.IsPressed(_T("Audio Play To End"))) {
		int start=0,end=0;
		GetTimesSelection(start,end);
		Play(start,-1);
	}

	// Play original line
	if (Hotkeys.IsPressed(_T("Audio Play Original Line"))) {
		int start=0,end=0;
		GetTimesDialogue(start,end);
		SetSelection(start, end);
		Play(start,end);
	}

	// Lead in
	if (Hotkeys.IsPressed(_T("Audio Add Lead In"))) {
		AddLead(true,false);
	}

	// Lead out
	if (Hotkeys.IsPressed(_T("Audio Add Lead Out"))) {
		AddLead(false,true);
	}

	// Update
	if (diagUpdated) {
		diagUpdated = false;
		NeedCommit = true;
		if (Options.AsBool(_T("Audio Autocommit")) && curStartMS <= curEndMS) CommitChanges();
		else UpdateImage(true);
	}
	*/
#pragma endregion
}



void AudioDisplay::OnAudioOpen(AudioProvider *_provider)
{
	provider = _provider;

	audio_renderer->SetAudioProvider(provider);
	audio_renderer->SetCacheMaxSize(Options.AsInt(_T("Audio Spectrum Memory Max")) * 1024 * 1024);

	SetZoomLevel(zoom_level);

	Refresh();
}


void AudioDisplay::OnAudioClose()
{
	OnAudioOpen(0);
}


void AudioDisplay::OnPlaybackPosition(int64_t sample_position)
{
	SetTrackCursor(sample_position / pixel_samples, false);
}


void AudioDisplay::OnPlaybackStop()
{
	RemoveTrackCursor();
}


void AudioDisplay::OnMarkersMoved()
{
	Refresh();
}


void AudioDisplay::OnSelectionChanged()
{
	AudioController::SampleRange sel(controller->GetSelection());
	scrollbar->SetSelection(sel.begin() / pixel_samples, sel.length() / pixel_samples);

	RefreshRect(wxRect(0, audio_top, GetClientSize().GetX(), audio_height));
}


void AudioDisplay::OnTimingControllerChanged()
{
	/// @todo Do something about the new timing controller?
}

