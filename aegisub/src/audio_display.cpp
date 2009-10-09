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

/// @file audio_display.cpp
/// @brief Display audio in the main UI
/// @ingroup audio_ui
///


///////////
// Headers
#include "config.h"

#include "audio_controller.h"
#include "audio_display.h"
#include "block_cache.h"
#include "audio_renderer.h"
#include "audio_renderer_spectrum.h"


/// @brief Constructor 
/// @param parent 
///
AudioDisplay::AudioDisplay(wxWindow *parent, AudioController *_controller)
: wxWindow(parent, -1, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS)
, controller(_controller)
{
	audio_renderer = new AudioRenderer;
	audio_spectrum_renderer = new AudioSpectrumRenderer;

	scroll_left = 0;
	pixel_samples = 1000;
	scale_amplitude = 1.0;

	controller->AddListener(this);

	audio_renderer->SetRenderer(audio_spectrum_renderer);
	audio_renderer->SetSamplesPerPixel(pixel_samples);
	audio_renderer->SetAmplitudeScale(scale_amplitude);

	SetMinClientSize(wxSize(-1, 70));
	//SetBackgroundStyle(wxBG_STYLE_CUSTOM); // intended to be wxBG_STYLE_PAINT but that doesn't exist for me
	SetBackgroundColour(*wxBLACK);
}



/// @brief Destructor 
///
AudioDisplay::~AudioDisplay()
{
	delete audio_renderer;
	delete audio_spectrum_renderer;

	controller->RemoveListener(this);
}



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



/// @brief Draw spectrum analyzer 
/// @param finaldc The DC to draw to.
/// @param weak    False if the visible portion of the display has changed.
//void AudioDisplay::DrawSpectrum(wxDC &finaldc,bool weak);




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



///////////////
// Event table
BEGIN_EVENT_TABLE(AudioDisplay, wxWindow)
    EVT_MOUSE_EVENTS(AudioDisplay::OnMouseEvent)
    EVT_PAINT(AudioDisplay::OnPaint)
	EVT_SIZE(AudioDisplay::OnSize)
	EVT_KEY_DOWN(AudioDisplay::OnKeyDown)
END_EVENT_TABLE()



/// @brief Paint 
/// @param event 
/// @return 
///
void AudioDisplay::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);

	//audio_renderer->Render(dc, wxPoint(1, 1), scroll_left, GetClientSize().GetWidth()-2, false);
}



/// @brief Mouse event 
/// @param event 
/// @return 
///
void AudioDisplay::OnMouseEvent(wxMouseEvent& event)
{
}



/// @brief The audio display was resized
/// @param event Event data
///
/// Tell our renderer about the new size and repaint ourselves.
void AudioDisplay::OnSize(wxSizeEvent &event)
{
	/// @todo This is wrong, the height for the renderer is smaller than the client height.
	audio_renderer->SetHeight(GetClientSize().GetHeight()-2);
}


/// @brief Key down 
/// @param event 
///
void AudioDisplay::OnKeyDown(wxKeyEvent &event) {
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
}



void AudioDisplay::OnAudioOpen(AudioProvider *_provider)
{
	provider = _provider;
	audio_renderer->SetAudioProvider(provider);
	Refresh();
}


void AudioDisplay::OnAudioClose()
{
	OnAudioOpen(0);
}


void AudioDisplay::OnMarkersMoved()
{
	Refresh();
}


void AudioDisplay::OnSelectionChanged()
{
	Refresh();
}


void AudioDisplay::OnPlaybackPosition(int64_t sample_position)
{
	markers.playback_pos = sample_position / pixel_samples;
	Refresh();
}


void AudioDisplay::OnPlaybackStop()
{
	markers.playback_pos = -1;
	Refresh();
}
