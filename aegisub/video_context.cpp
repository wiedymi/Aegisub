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


////////////
// Includes
#include "setup.h"
#include <wx/image.h>
#include <string.h>
#include <wx/clipbrd.h>
#include <wx/filename.h>
#include <wx/config.h>
#include "utils.h"
#include "video_display.h"
#include "video_context.h"
#include "video_provider.h"
#include "vfr.h"
#include "ass_file.h"
#include "ass_time.h"
#include "ass_dialogue.h"
#include "ass_style.h"
#include "subs_grid.h"
#include "vfw_wrap.h"
#include "mkv_wrap.h"
#include "options.h"
#include "subs_edit_box.h"
#include "audio_display.h"
#include "main.h"
#include "video_slider.h"
#include "video_box.h"


///////
// IDs
enum {
	VIDEO_PLAY_TIMER = 1300
};


///////////////
// Event table
BEGIN_EVENT_TABLE(VideoContext, wxEvtHandler)
	EVT_TIMER(VIDEO_PLAY_TIMER,VideoContext::OnPlayTimer)
END_EVENT_TABLE()


////////////
// Instance
VideoContext *VideoContext::instance = NULL;


///////////////
// Constructor
VideoContext::VideoContext() {
	// Set GL context
	glContext = NULL;

	// Set options
	audio = NULL;
	provider = NULL;
	curLine = NULL;
	loaded = false;
	keyFramesLoaded = false;
	overKeyFramesLoaded = false;
	frame_n = 0;
	isPlaying = false;
	threaded = Options.AsBool(_T("Threaded Video"));
	nextFrame = -1;
}


//////////////
// Destructor
VideoContext::~VideoContext () {
	Reset();
}


////////////////
// Get Instance
VideoContext *VideoContext::Get() {
	if (!instance) instance = new VideoContext;
	return instance;
}


/////////
// Clear
void VideoContext::Clear() {
	delete instance;
	instance = NULL;
}


/////////
// Reset
void VideoContext::Reset() {
	// Clear keyframes
	KeyFrames.Clear();
	keyFramesLoaded = false;

	// Remove temporary audio provider
	if (audio && audio->temporary) {
		delete audio->provider;
		audio->provider = NULL;
		delete audio->player;
		audio->player = NULL;
		audio->temporary = false;
	}
	audio = NULL;

	// Remove video data
	loaded = false;
	frame_n = 0;
	keyFramesLoaded = false;
	overKeyFramesLoaded = false;
	isPlaying = false;
	nextFrame = -1;

	// Update displays
	UpdateDisplays();

	// Finish clean up
	wxRemoveFile(tempfile);
	tempfile = _T("");
	videoName = _T("");
	delete provider;
	provider = NULL;
	delete glContext;
	glContext = NULL;
}


///////////////////////
// Sets video filename
void VideoContext::SetVideo(const wxString &filename) {
	// Unload video
	Reset();
	
	// Load video
	if (!filename.IsEmpty()) {
		try {
			grid->CommitChanges(true);
			bool isVfr = false;
			double overFps = 0;
			FrameRate temp;

			// Unload timecodes
			//int unload = wxYES;
			//if (VFR_Output.IsLoaded()) unload = wxMessageBox(_("Do you want to unload timecodes, too?"),_("Unload timecodes?"),wxYES_NO | wxICON_QUESTION);
			//if (unload == wxYES) VFR_Output.Unload();

			// Read extra data from file
			bool mkvOpen = MatroskaWrapper::wrapper.IsOpen();
			wxString ext = filename.Right(4).Lower();
			KeyFrames.Clear();
			if (ext == _T(".mkv") || mkvOpen) {
				// Parse mkv
				if (!mkvOpen) MatroskaWrapper::wrapper.Open(filename);

				// Get keyframes
				KeyFrames = MatroskaWrapper::wrapper.GetKeyFrames();
				keyFramesLoaded = true;

				// Ask to override timecodes
				int override = wxYES;
				if (VFR_Output.IsLoaded()) override = wxMessageBox(_("You already have timecodes loaded. Replace them with the timecodes from the Matroska file?"),_("Replace timecodes?"),wxYES_NO | wxICON_QUESTION);
				if (override == wxYES) {
					MatroskaWrapper::wrapper.SetToTimecodes(temp);
					isVfr = temp.GetFrameRateType() == VFR;
					if (isVfr) {
						overFps = temp.GetCommonFPS();
						MatroskaWrapper::wrapper.SetToTimecodes(VFR_Input);
	 					MatroskaWrapper::wrapper.SetToTimecodes(VFR_Output);
					}
				}

				// Close mkv
				MatroskaWrapper::wrapper.Close();
			}
#ifdef __WINDOWS__
			else if (ext == _T(".avi")) {
				KeyFrames = VFWWrapper::GetKeyFrames(filename);
				keyFramesLoaded = true;
			}
#endif

			// Choose a provider
			provider = VideoProvider::GetProvider(filename,GetTempWorkFile(),overFps);
			if (isVfr) provider->OverrideFrameTimeList(temp.GetFrameTimeList());

			//Gather video parameters
			length = provider->GetFrameCount();
			fps = provider->GetFPS();
			w = provider->GetWidth();
			h = provider->GetHeight();

			// Set CFR
			if (!isVfr) {
				VFR_Input.SetCFR(fps);
				if (VFR_Output.GetFrameRateType() != VFR) VFR_Output.SetCFR(fps);
			}

			// Set filename
			videoName = filename;
			Options.AddToRecentList(filename,_T("Recent vid"));

			// Update displays
			UpdateDisplays();
		}
		
		catch (wxString &e) {
			wxMessageBox(e,_T("Error setting video"),wxICON_ERROR | wxOK);
		}
	}

	loaded = provider != NULL;
}


///////////////////
// Add new display
void VideoContext::AddDisplay(VideoDisplay *display) {
	for (std::list<VideoDisplay*>::iterator cur=displayList.begin();cur!=displayList.end();cur++) {
		if ((*cur) == display) return;
	}
	displayList.push_back(display);
}


//////////////////
// Remove display
void VideoContext::RemoveDisplay(VideoDisplay *display) {
	displayList.remove(display);
}


///////////////////
// Update displays
void VideoContext::UpdateDisplays() {
	for (std::list<VideoDisplay*>::iterator cur=displayList.begin();cur!=displayList.end();cur++) {
		VideoDisplay *display = *cur;
		
		display->UpdateSize();
		display->ControlSlider->SetRange(0,GetLength()-1);
		display->ControlSlider->SetValue(GetFrameN());
		display->UpdatePositionDisplay();
		display->Refresh();
	}
}


/////////////////////
// Refresh subtitles
void VideoContext::Refresh (bool video, bool subtitles) {
	//provider->RefreshSubtitles();
	//RefreshVideo();
}


///////////////////////////////////////
// Jumps to a frame and update display
void VideoContext::JumpToFrame(int n) {
	// Loaded?
	if (!loaded) return;

	// Prevent intervention during playback
	if (isPlaying && n != PlayNextFrame) return;

	// Set frame number
	frame_n = n;

	// Display
	UpdateDisplays();

	// Update grid
	if (!isPlaying && Options.AsBool(_T("Highlight subs in frame"))) grid->Refresh(false);
}


////////////////////////////
// Jumps to a specific time
void VideoContext::JumpToTime(int ms) {
	JumpToFrame(VFR_Output.GetFrameAtTime(ms));
}


//////////////////
// Get GL context
wxGLContext *VideoContext::GetGLContext(wxGLCanvas *canvas) {
	if (!glContext) glContext = new wxGLContext(canvas);
	return glContext;
}


///////////////////////////
// Get GL Texture of frame
GLuint VideoContext::GetFrameAsTexture(int n) {
	// See if frame is available on cache
	for (std::list<CachedTexture>::iterator cur=textureCache.begin();cur!=textureCache.end();cur++) {
		if ((*cur).frame == n) {
			return (*cur).texture;
		}
	}

	// Get frame
	AegiVideoFrame frame = provider->GetFrame(n);

	// Generate texture with GL
	GLuint tex;
	GLenum err;
	glGenTextures(1, &tex);
	err = glGetError();
	glBindTexture(GL_TEXTURE_2D, tex);
	err = glGetError();

	// Image type
	GLenum type = GL_RGBA;
	if (frame.invertChannels) type = GL_BGRA_EXT;
	isInverted = frame.flipped;

	// Load image data into texture
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,frame.w,frame.h,0,type,GL_UNSIGNED_BYTE,frame.data[0]);
	err = glGetError();

	// Unload frame
	frame.Clear();

	// Cache texture
	CachedTexture temp;
	temp.frame = n;
	temp.texture = tex;
	textureCache.push_back(temp);

	// Remove old cache data
	if (textureCache.size() > 32) {
		textureCache.front().Unload();
		textureCache.pop_front();
	}

	return tex;
}


/////////////////
// Save snapshot
void VideoContext::SaveSnapshot() {
	// Get folder
	wxString option = Options.AsText(_("Video Screenshot Path"));
	wxFileName videoFile(videoName);
	wxString basepath;
	if (option == _T("?video")) {
		basepath = videoFile.GetPath();
	}
	else if (option == _T("?script")) {
		if (grid->ass->filename.IsEmpty()) basepath = videoFile.GetPath();
		else {
			wxFileName file2(grid->ass->filename);
			basepath = file2.GetPath();
		}
	}
	else basepath = DecodeRelativePath(option,((AegisubApp*)wxTheApp)->folderName);
	basepath += _T("/") + videoFile.GetName();

	// Get full path
	int session_shot_count = 1;
	wxString path;
	while (1) {
		path = basepath + wxString::Format(_T("_%03i_%i.png"),session_shot_count,frame_n);
		++session_shot_count;
		wxFileName tryPath(path);
		if (!tryPath.FileExists()) break;
	}

	// Save
	// TODO
	//GetFrame(frame_n).ConvertToImage().SaveFile(path,wxBITMAP_TYPE_PNG);
}


////////////////////////
// Requests a new frame
int VideoContext::GetFrame(int n) {
	//if (n < 0) n = frame_n;
	//frame_n = n;
	//return provider->GetFrame(n);
	//Refresh(true,false);
	return 0;
}


////////////////////////////
// Get dimensions of script
void VideoContext::GetScriptSize(int &sw,int &sh) {
	grid->ass->GetResolution(sw,sh);
}


////////
// Play
void VideoContext::Play() {
	// Stop if already playing
	if (isPlaying) {
		Stop();
		return;
	}

	// Set variables
	isPlaying = true;
	StartFrame = frame_n;
	EndFrame = -1;

	// Start playing audio
	audio->Play(VFR_Output.GetTimeAtFrame(StartFrame),-1);

	// Start timer
	StartTime = clock();
	PlayTime = StartTime;
	Playback.SetOwner(this,VIDEO_PLAY_TIMER);
	Playback.Start(1);
}


/////////////
// Play line
void VideoContext::PlayLine() {
	// Get line
	AssDialogue *curline = grid->GetDialogue(grid->editBox->linen);
	if (!curline) return;

	// Start playing audio
	audio->Play(curline->Start.GetMS(),curline->End.GetMS());

	// Set variables
	isPlaying = true;
	StartFrame = VFR_Output.GetFrameAtTime(curline->Start.GetMS(),true);
	EndFrame = VFR_Output.GetFrameAtTime(curline->End.GetMS(),false);

	// Jump to start
	PlayNextFrame = StartFrame;
	JumpToFrame(StartFrame);

	// Set other variables
	StartTime = clock();
	PlayTime = StartTime;

	// Start timer
	Playback.SetOwner(this,VIDEO_PLAY_TIMER);
	Playback.Start(1);
}


////////
// Stop
void VideoContext::Stop() {
	if (isPlaying) {
		Playback.Stop();
		isPlaying = false;
		audio->Stop();
	}
}


//////////////
// Play timer
void VideoContext::OnPlayTimer(wxTimerEvent &event) {
	// Get time difference
	clock_t cur = clock();
	int dif = (clock() - StartTime)*1000/CLOCKS_PER_SEC;
	if (!dif) return;
	PlayTime = cur;

	// Find next frame
	int startMs = VFR_Output.GetTimeAtFrame(StartFrame);
	int nextFrame = frame_n;
	for (int i=0;i<10;i++) {
		if (nextFrame >= length) break;
		if (dif < VFR_Output.GetTimeAtFrame(nextFrame) - startMs) {
			break;
		}
		nextFrame++;
	}

	// Same frame
	if (nextFrame == frame_n) return;

	// End
	if (nextFrame >= length || (EndFrame != -1 && nextFrame > EndFrame)) {
		Stop();
		return;
	}

	// Next frame is before or over 2 frames ahead, so force audio resync
	if (nextFrame < frame_n || nextFrame > frame_n + 2) audio->player->SetCurrentPosition(audio->GetSampleAtMS(VFR_Output.GetTimeAtFrame(nextFrame)));

	// Jump to next frame
	PlayNextFrame = nextFrame;
	JumpToFrame(nextFrame);

	// Sync audio
	if (nextFrame % 10 == 0) {
		__int64 audPos = audio->GetSampleAtMS(VFR_Output.GetTimeAtFrame(nextFrame));
		__int64 curPos = audio->player->GetCurrentPosition();
		int delta = int(audPos-curPos);
		if (delta < 0) delta = -delta;
		int maxDelta = audio->provider->GetSampleRate();
		if (delta > maxDelta) audio->player->SetCurrentPosition(audPos);
	}
}


//////////////////////////////
// Get name of temp work file
wxString VideoContext::GetTempWorkFile () {
	if (tempfile.IsEmpty()) {
		tempfile = wxFileName::CreateTempFileName(_T("aegisub"));
		wxRemoveFile(tempfile);
		tempfile += _T(".ass");
	}
	return tempfile;
}


/////////////////
// Get keyframes
wxArrayInt VideoContext::GetKeyFrames() {
	if (OverKeyFramesLoaded()) return overKeyFrames;
	return KeyFrames;
}


/////////////////
// Set keyframes
void VideoContext::SetKeyFrames(wxArrayInt frames) {
	KeyFrames = frames;
}


/////////////////////////
// Set keyframe override
void VideoContext::SetOverKeyFrames(wxArrayInt frames) {
	overKeyFrames = frames;
	overKeyFramesLoaded = true;
}


///////////////////
// Close keyframes
void VideoContext::CloseOverKeyFrames() {
	overKeyFrames.Clear();
	overKeyFramesLoaded = false;
}


//////////////////////////////////////////
// Check if override keyframes are loaded
bool VideoContext::OverKeyFramesLoaded() {
	return overKeyFramesLoaded;
}


/////////////////////////////////
// Check if keyframes are loaded
bool VideoContext::KeyFramesLoaded() {
	return overKeyFramesLoaded || keyFramesLoaded;
}


//////////////////
// Cached texture
void CachedTexture::Unload() {
	glDeleteTextures(1,&texture);
}
