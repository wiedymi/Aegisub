// Copyright (c) 2005-2007, Rodrigo Braz Monteiro
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
// -----------------------------------------------------------------------------
//
// AEGISUB
//
// Website: http://aegisub.cellosoft.com
// Contact: mailto:zeratul@cellosoft.com
//


#pragma once


///////////
// Headers
#include <wx/wxprec.h>
#include <time.h>
#include <wx/glcanvas.h>


//////////////
// Prototypes
class SubtitlesGrid;
class AudioProvider;
class AudioDisplay;
class AssDialogue;
class VideoProvider;
class VideoDisplay;


//////////////
// Main class
class VideoContext : public wxEvtHandler {
	friend class AudioProvider;
	friend class VideoDisplayVisual;

private:
	static VideoContext *instance;
	std::list<VideoDisplay*> displayList;

	wxGLContext *glContext;
	wxString tempfile;

	VideoProvider *provider;

	bool threaded;
	int nextFrame;

	bool keyFramesLoaded;
	bool overKeyFramesLoaded;
	wxArrayInt KeyFrames;
	wxArrayInt overKeyFrames;
	wxString keyFramesFilename;

	clock_t PlayTime;
	clock_t StartTime;
	wxTimer Playback;

	int StartFrame;
	int EndFrame;
	int PlayNextFrame;

	bool loaded;
	int w,h;
	int frame_n;
	int length;
	bool isPlaying;
	double fps;

	int GetFrame(int n);
	void SaveSnapshot();
	void OnPlayTimer(wxTimerEvent &event);

public:
	SubtitlesGrid *grid;
	wxString videoName;

	AssDialogue *curLine;
	AudioDisplay *audio;

	VideoContext();
	~VideoContext();

	void AddDisplay(VideoDisplay *display);
	void RemoveDisplay(VideoDisplay *display);

	VideoProvider *GetProvider() { return provider; }

	bool IsLoaded() { return loaded; }
	bool IsPlaying() { return isPlaying; }
	int GetWidth() { return w; }
	int GetHeight() { return h; }
	int GetLength() { return length; }
	int GetFrameN() { return frame_n; }
	double GetFPS() { return fps; }
	void SetFPS(double _fps) { fps = _fps; }

	void SetVideo(const wxString &filename);
	void Reset();

	void JumpToFrame(int n);
	void JumpToTime(int ms);

	void Refresh(bool video,bool subtitles);
	void UpdateDisplays();

	void GetScriptSize(int &w,int &h);
	wxString GetTempWorkFile ();

	void Play();
	void PlayLine();
	void Stop();

	wxArrayInt GetKeyFrames();
	void SetKeyFrames(wxArrayInt frames);
	void SetOverKeyFrames(wxArrayInt frames);
	void CloseOverKeyFrames();
	bool OverKeyFramesLoaded();
	bool KeyFramesLoaded();
	wxString GetKeyFramesName() { return keyFramesFilename; }
	void SetKeyFramesName(wxString name) { keyFramesFilename = name; }

	static VideoContext *Get();
	static void Clear();

	DECLARE_EVENT_TABLE()
};
