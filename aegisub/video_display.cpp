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
#include "video_display_visual.h"
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
#if USE_FEXTRACKER == 1
#include "../FexTrackerSource/FexTracker.h"
#include "../FexTrackerSource/FexTrackingFeature.h"
#include "../FexTrackerSource/FexMovement.h"
#endif


///////
// IDs
enum {
	VIDEO_MENU_COPY_TO_CLIPBOARD = 1230,
	VIDEO_MENU_COPY_COORDS,
	VIDEO_MENU_SAVE_SNAPSHOT,
};


///////////////
// Event table
BEGIN_EVENT_TABLE(VideoDisplay, wxGLCanvas)
	//EVT_MOUSE_EVENTS(VideoDisplay::OnMouseEvent)
	//EVT_KEY_DOWN(VideoDisplay::OnKey)
	//EVT_LEAVE_WINDOW(VideoDisplay::OnMouseLeave)
	//EVT_PAINT(VideoDisplay::OnPaint)

	//EVT_TIMER(VIDEO_PLAY_TIMER,VideoDisplay::OnPlayTimer)

	//EVT_MENU(VIDEO_MENU_COPY_TO_CLIPBOARD,VideoDisplay::OnCopyToClipboard)
	//EVT_MENU(VIDEO_MENU_SAVE_SNAPSHOT,VideoDisplay::OnSaveSnapshot)
	//EVT_MENU(VIDEO_MENU_COPY_COORDS,VideoDisplay::OnCopyCoords)
END_EVENT_TABLE()


///////////////
// Constructor
VideoDisplay::VideoDisplay(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxGLCanvas (parent, id, NULL, pos, size, style, name)
{
	//// Set options
	ControlSlider = NULL;
	PositionDisplay = NULL;
	origSize = size;
	arType = 0;
	zoomValue = 0.5;
	visual = new VideoDisplayVisual(this);
}


//////////////
// Destructor
VideoDisplay::~VideoDisplay () {
	delete visual;
	VideoContext::Get()->RemoveDisplay(this);
}


///////////////
// Update size
void  VideoDisplay::UpdateSize() {
	//if (provider) {
	//	w = provider->GetWidth();
	//	h = provider->GetHeight();

	//	// Set the size for this control
	//	SetSizeHints(w,h,w,h);
	//	SetClientSize(w,h);
	//	int _w,_h;
	//	GetSize(&_w,&_h);
	//	SetSizeHints(_w,_h,_w,_h);

	//	box->VideoSizer->Fit(box);
	//}
}


//////////
// Resets
void VideoDisplay::Reset() {
	int w = origSize.GetX();
	int h = origSize.GetY();
	SetClientSize(w,h);
	int _w,_h;
	GetSize(&_w,&_h);
	SetSizeHints(_w,_h,_w,_h);
}


/////////////////
// OnPaint event
void VideoDisplay::OnPaint(wxPaintEvent& event) {
	wxPaintDC dc(this);

	// Draw frame
	//RefreshVideo();
	//if (provider) dc.DrawBitmap(GetFrame(frame_n),0,0);
}


///////////////
// Mouse stuff
void VideoDisplay::OnMouseEvent(wxMouseEvent& event) {
	// Disable when playing
	//if (IsPlaying) return;

	//if (event.Leaving()) {
	//	// OnMouseLeave isn't called as long as we have an OnMouseEvent
	//	// Just check for it and call it manually instead
	//	OnMouseLeave(event);
	//	event.Skip(true);
	//	return;
	//}

	//// Right click
	//if (event.ButtonUp(wxMOUSE_BTN_RIGHT)) {
	//	wxMenu menu;
	//	menu.Append(VIDEO_MENU_SAVE_SNAPSHOT,_("Save PNG snapshot"));
	//	menu.Append(VIDEO_MENU_COPY_TO_CLIPBOARD,_("Copy image to Clipboard"));
	//	menu.Append(VIDEO_MENU_COPY_COORDS,_("Copy coordinates to Clipboard"));
	//	PopupMenu(&menu);
	//	return;
	//}

	//// Click?
	//if (event.ButtonDown(wxMOUSE_BTN_ANY)) {
	//	SetFocus();
	//}

	//// Send to visual
	////visual->OnMouseEvent(event);
}


/////////////
// Key event
void VideoDisplay::OnKey(wxKeyEvent &event) {
	//visual->OnKeyEvent(event);
}



//////////////////////
// Mouse left display
void VideoDisplay::OnMouseLeave(wxMouseEvent& event) {
	//if (IsPlaying) return;

	//bTrackerEditing = 0;

	//RefreshVideo();
}


///////////////////
// Sets zoom level
void VideoDisplay::SetZoom(double value) {
	//zoomValue = value;
	//if (provider) {
	//	provider->SetZoom(value);
	//	UpdateSize();
	//	RefreshVideo();
	//	box->GetParent()->Layout();
	//}
}


//////////////////////
// Sets zoom position
void VideoDisplay::SetZoomPos(int value) {
	//if (value < 0) value = 0;
	//if (value > 15) value = 15;
	//SetZoom(double(value+1)/8.0);
	//if (zoomBox->GetSelection() != value) zoomBox->SetSelection(value);
}


//////////////////////////
// Calculate aspect ratio
double VideoDisplay::GetARFromType(int type) {
	if (type == 0) return (double)VideoContext::Get()->GetWidth()/(double)VideoContext::Get()->GetHeight();
	if (type == 1) return 4.0/3.0;
	if (type == 2) return 16.0/9.0;
	if (type == 3) return 2.35;
	return 1.0;  //error
}


/////////////////////
// Sets aspect ratio
void VideoDisplay::SetAspectRatio(int _type, double value) {
	//if (provider) {
	//	// Get value
	//	if (_type != 4) value = GetARFromType(_type);
	//	if (value < 0.5) value = 0.5;
	//	if (value > 5.0) value = 5.0;

	//	// Set
	//	provider->SetDAR(value);
	//	arType = _type;
	//	arValue = value;
	//	UpdateSize();
	//	RefreshVideo();
	//	GetParent()->Layout();
	//}
}


////////////////////////////
// Updates position display
void VideoDisplay::UpdatePositionDisplay() {
	//// Update position display control
	//if (!PositionDisplay) {
	//	throw _T("Position Display not set!");
	//}

	//// Get time
	//int time = VFR_Output.GetTimeAtFrame(frame_n,true,true);
	//int temp = time;
	//int h=0, m=0, s=0, ms=0;
	//while (temp >= 3600000) {
	//	temp -= 3600000;
	//	h++;
	//}
	//while (temp >= 60000) {
	//	temp -= 60000;
	//	m++;
	//}
	//while (temp >= 1000) {
	//	temp -= 1000;
	//	s++;
	//}
	//ms = temp;

	//// Position display update
	//PositionDisplay->SetValue(wxString::Format(_T("%01i:%02i:%02i.%03i - %i"),h,m,s,ms,frame_n));
	//if (GetKeyFrames().Index(frame_n) != wxNOT_FOUND) {
	//	PositionDisplay->SetBackgroundColour(Options.AsColour(_T("Grid selection background")));
	//	PositionDisplay->SetForegroundColour(Options.AsColour(_T("Grid selection foreground")));
	//}
	//else {
	//	PositionDisplay->SetBackgroundColour(wxNullColour);
	//	PositionDisplay->SetForegroundColour(wxNullColour);
	//}

	//// Subs position display update
	//UpdateSubsRelativeTime();
}


////////////////////////////////////////////////////
// Updates box with subs position relative to frame
void VideoDisplay::UpdateSubsRelativeTime() {
	//// Set variables
	//wxString startSign;
	//wxString endSign;
	//int startOff,endOff;

	//// Set start/end
	//if (curLine) {
	//	int time = VFR_Output.GetTimeAtFrame(frame_n,true,true);
	//	startOff = time - curLine->Start.GetMS();
	//	endOff = time - curLine->End.GetMS();
	//}

	//// Fallback to zero
	//else {
	//	startOff = 0;
	//	endOff = 0;
	//}

	//// Positive signs
	//if (startOff > 0) startSign = _T("+");
	//if (endOff > 0) endSign = _T("+");

	//// Update line
	//SubsPosition->SetValue(wxString::Format(_T("%s%ims; %s%ims"),startSign.c_str(),startOff,endSign.c_str(),endOff));
}


/////////////////////
// Copy to clipboard
void VideoDisplay::OnCopyToClipboard(wxCommandEvent &event) {
	//if (wxTheClipboard->Open()) {
	//	wxTheClipboard->SetData(new wxBitmapDataObject(GetFrame(frame_n)));
	//	wxTheClipboard->Close();
	//}
}


/////////////////
// Save snapshot
void VideoDisplay::OnSaveSnapshot(wxCommandEvent &event) {
	//SaveSnapshot();
}


/////////////////////
// Copy coordinates
void VideoDisplay::OnCopyCoords(wxCommandEvent &event) {
	//if (wxTheClipboard->Open()) {
	//	int sw,sh;
	//	GetScriptSize(sw,sh);
	//	int vx = (sw * visual->mouseX + w/2) / w;
	//	int vy = (sh * visual->mouseY + h/2) / h;
	//	wxTheClipboard->SetData(new wxTextDataObject(wxString::Format(_T("%i,%i"),vx,vy)));
	//	wxTheClipboard->Close();
	//}
}


//////////////////
// Refresh screen
//void VideoDisplay::RefreshVideo() {
	//// Is shown?
	//if (!GetParent()->IsShown()) return;

	//// Set GL context
	//SetCurrent(*glContext);

	//// Clear
	//glClearColor(0.0f,0.0f,0.0f,0.0f);
	//glClear(GL_COLOR_BUFFER_BIT);

	//// Draw something
	//glBegin(GL_POLYGON);
	//	glColor3f(1.0f,0.0f,0.0f);
	//	glVertex2f(-1.0f,-1.0f);
	//	glColor3f(0.0f,1.0f,0.0f);
	//	glVertex2f(1.0f,-1.0f);
	//	glColor3f(0.0f,0.0f,1.0f);
	//	glVertex2f(1.0f,1.0f);
	//	glColor3f(1.0f,0.0f,1.0f);
	//	glVertex2f(-1.0f,1.0f);
	//glEnd();

	//// Swap
	//glFinish();
	//SwapBuffers();

	//// Draw frame
	//wxClientDC dc(this);
	////dc.DrawBitmap(GetFrame(),0,0);

	//// Draw the control points for FexTracker
	//visual->DrawTrackingOverlay(dc);
//}


//////////////////
// DrawVideoWithOverlay
void VideoDisplay::DrawText( wxPoint Pos, wxString text ) {
	//// Draw frame
	//wxClientDC dc(this);
	//dc.SetBrush(wxBrush(wxColour(128,128,128),wxSOLID));
	//dc.DrawRectangle( 0,0, provider->GetWidth(), provider->GetHeight() );
	//dc.SetTextForeground(wxColour(64,64,64));
	//dc.DrawText(text,Pos.x+1,Pos.y-1);
	//dc.DrawText(text,Pos.x+1,Pos.y+1);
	//dc.DrawText(text,Pos.x-1,Pos.y-1);
	//dc.DrawText(text,Pos.x-1,Pos.y+1);
	//dc.SetTextForeground(wxColour(255,255,255));
	//dc.DrawText(text,Pos.x,Pos.y);
}

