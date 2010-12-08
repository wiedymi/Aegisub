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

/// @file frame_main.h
/// @see frame_main.cpp
/// @ingroup main_ui
///

#ifndef AGI_PRE
#include <vector>

#include <wx/combobox.h>
#include <wx/frame.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/sashwin.h>
#include <wx/timer.h>
#endif

#include "aegisub/context.h"


class AssFile;
class VideoDisplay;
class VideoSlider;
class VideoZoomSlider;
class SubtitlesGrid;
class SubsEditBox;
class AudioBox;
class VideoBox;
class DialogDetachedVideo;
class DialogStyling;
class AegisubFileDropTarget;
class AudioController;
class AudioProvider;

namespace Automation4 { class FeatureMacro; class ScriptManager; }


enum SubMenuID {
	ID_SM_SUBTITLES_INSERT					= 11001,
	ID_SM_SUBTITLES_JOIN					= 11002,
	ID_SM_SUBTITLES_SORT					= 11003,
	ID_SM_TIMING_CONTINOUS					= 11004,
	ID_SM_VIDEO_ZOOM						= 11005,
	ID_SM_VIDEO_OVERRIDE_AR					= 10006,
	ID_SM_FILE_RECENT_SUBS					= 10007,
	ID_SM_VIDEO_ID_MENU_RECENT_VIDEO		= 10008,
	ID_SM_AUDIO_ID_MENU_RECENT_AUDIO		= 10009,
	ID_SM_VIDEO_ID_MENU_RECENT_TIMECODES	= 10010,
	ID_SM_VIDEO_ID_MENU_RECENT_KEYFRAMES	= 10011,
	ID_TOOLBAR_ZOOM_DROPDOWN				= 11001,
	ID_APP_TIMER_AUTOSAVE					= 12001,
	ID_APP_TIMER_STATUSCLEAR				= 12002,
	ID_RECENT_FILE							= 13001,
	ID_MENU_RECENT_VIDEO					= 13002,
	ID_MENU_RECENT_AUDIO					= 13003,
	ID_MENU_RECENT_TIMECODES				= 13004,
	ID_MENU_RECENT_KEYFRAMES				= 13005,
	ID_MENU_AUTOMATION_MACRO				= 13006,
	ID_SASH_MAIN_AUDIO						= 14001
};

/// DOCME
/// @class FrameMain
/// @brief DOCME
///
/// DOCME
class FrameMain: public wxFrame {
	friend class AegisubFileDropTarget;
	friend class AegisubApp;
	friend class SubtitlesGrid;

private:

	agi::Context temp_context;

	void cmd_call(wxCommandEvent& event);

	AssFile *ass;

	/// DOCME

	/// DOCME
	bool showVideo,showAudio;

	/// DOCME
	bool HasSelection;

	/// DOCME
	bool menuCreated;

	/// DOCME
	wxTimer AutoSave;

	/// DOCME
	wxTimer StatusClear;


	/// DOCME
	bool blockVideoLoad;


	/// DOCME
	wxPanel *Panel;


	/// DOCME
	wxMenuBar *MenuBar;

	/// DOCME
	wxMenu *fileMenu;

	/// DOCME
	wxMenu *editMenu;

	/// DOCME
	wxMenu *videoMenu;

	/// DOCME
	wxMenu *timingMenu;

	/// DOCME
	wxMenu *subtitlesMenu;

	/// DOCME
	wxMenu *helpMenu;

	/// DOCME
	wxMenu *audioMenu;

	/// DOCME
	wxMenu *viewMenu;

	/// DOCME
	wxMenu *automationMenu;

	/// DOCME
	wxMenu *kanjiTimingMenu;


	/// DOCME
	wxMenu *RecentSubs;

	/// DOCME
	wxMenu *RecentVids;

	/// DOCME
	wxMenu *RecentAuds;

	/// DOCME
	wxMenu *RecentTimecodes;

	/// DOCME
	wxMenu *RecentKeyframes;


	/// DOCME
	wxToolBar *Toolbar;

	/// DOCME
	wxComboBox *ZoomBox;


	/// DOCME
	wxWindow *PreviousFocus;

#ifdef WITH_AUTOMATION

	/// DOCME
	Automation4::ScriptManager *local_scripts;
#endif


	/// DOCME
	std::vector<Automation4::FeatureMacro*> activeMacroItems;
	int AddMacroMenuItems(wxMenu *menu, const std::vector<Automation4::FeatureMacro*> &macros);

	void InitToolbar();
	void InitContents();
	void DeInitContents();

	void OnAutoSave(wxTimerEvent &event);
	void OnStatusClear(wxTimerEvent &event);

	void OnVideoPlay(wxCommandEvent &event);

	void OnAudioBoxResize(wxSashEvent &event);

	void OnOpenRecentSubs (wxCommandEvent &event);
	void OnOpenRecentVideo (wxCommandEvent &event);
	void OnOpenRecentAudio (wxCommandEvent &event);
	void OnOpenRecentTimecodes (wxCommandEvent &event);
	void OnOpenRecentKeyframes (wxCommandEvent &event);

	void OnNewWindow (wxCommandEvent &event);
	void OnCloseWindow (wxCloseEvent &event);
	void OnMenuOpen (wxMenuEvent &event);
	void OnExit(wxCommandEvent &WXUNUSED(event));
	void OnAbout (wxCommandEvent &event);
	void OnLog (wxCommandEvent &event);
	void OnCheckUpdates (wxCommandEvent &event);
	void OnContents (wxCommandEvent &event);
	void OnFiles (wxCommandEvent &event);
	void OnWebsite (wxCommandEvent &event);
	void OnForums (wxCommandEvent &event);
	void OnBugTracker (wxCommandEvent &event);
	void OnIRCChannel (wxCommandEvent &event);

	void OnNewSubtitles (wxCommandEvent &event);
	void OnOpenSubtitles (wxCommandEvent &event);
	void OnOpenSubtitlesCharset (wxCommandEvent &event);
	void OnOpenSubtitlesVideo (wxCommandEvent &event);
	void OnSaveSubtitles (wxCommandEvent &event);
	void OnSaveSubtitlesAs (wxCommandEvent &event);
	void OnSaveSubtitlesCharset (wxCommandEvent &event);
	void OnExportSubtitles (wxCommandEvent &event);
	void OnOpenVideo (wxCommandEvent &event);
	void OnCloseVideo (wxCommandEvent &event);
	void OnOpenVFR (wxCommandEvent &event);
	void OnSaveVFR (wxCommandEvent &event);
	void OnCloseVFR (wxCommandEvent &event);
	void OnOpenKeyframes (wxCommandEvent &event);
	void OnCloseKeyframes (wxCommandEvent &event);
	void OnSaveKeyframes (wxCommandEvent &event);

	void OnZoomIn (wxCommandEvent &event);
	void OnZoomOut (wxCommandEvent &event);
	void OnSetZoom50 (wxCommandEvent &event);
	void OnSetZoom100 (wxCommandEvent &event);
	void OnSetZoom200 (wxCommandEvent &event);
	void OnSetZoom (wxCommandEvent &event);
	void OnSetARDefault (wxCommandEvent &event);
	void OnSetARWide (wxCommandEvent &event);
	void OnSetARFull (wxCommandEvent &event);
	void OnSetAR235 (wxCommandEvent &event);
	void OnSetARCustom (wxCommandEvent &event);
	void OnDetachVideo (wxCommandEvent &event);
	void OnDummyVideo (wxCommandEvent &event);
	void OnOverscan (wxCommandEvent &event);

	void OnOpenAudio (wxCommandEvent &event);
	void OnOpenAudioFromVideo (wxCommandEvent &event);
	void OnCloseAudio (wxCommandEvent &event);
	void OnAudioDisplayMode (wxCommandEvent &event);
#ifdef _DEBUG
	void OnOpenDummyAudio(wxCommandEvent &event);
	void OnOpenDummyNoiseAudio(wxCommandEvent &event);
#endif

	void OnChooseLanguage (wxCommandEvent &event);
	void OnViewStandard (wxCommandEvent &event);
	void OnViewVideo (wxCommandEvent &event);
	void OnViewAudio (wxCommandEvent &event);
	void OnViewSubs (wxCommandEvent &event);
	void OnSetTags (wxCommandEvent &event);

	void OnUndo (wxCommandEvent &event);
	void OnRedo (wxCommandEvent &event);
	void OnCut (wxCommandEvent &event);
	void OnCopy (wxCommandEvent &event);
	void OnPaste (wxCommandEvent &event);
	void OnPasteOver (wxCommandEvent &event);
	void OnDelete (wxCommandEvent &event);
	void OnFind (wxCommandEvent &event);
	void OnFindNext (wxCommandEvent &event);
	void OnReplace (wxCommandEvent &event);
	void OnJumpTo (wxCommandEvent &event);
	void OnShift (wxCommandEvent &event);
	void OnSortStart (wxCommandEvent &event);
	void OnSortEnd (wxCommandEvent &event);
	void OnSortStyle (wxCommandEvent &event);
	void OnOpenProperties (wxCommandEvent &event);
	void OnOpenStylesManager (wxCommandEvent &event);
	void OnOpenAttachments (wxCommandEvent &event);
	void OnOpenTranslation (wxCommandEvent &event);
	void OnOpenSpellCheck (wxCommandEvent &event);
	void OnOpenFontsCollector (wxCommandEvent &event);
	void OnSnapSubsStartToVid (wxCommandEvent &event);
	void OnSnapSubsEndToVid (wxCommandEvent &event);
	void OnSnapVidToSubsStart (wxCommandEvent &event);
	void OnSnapVidToSubsEnd (wxCommandEvent &event);
	void OnSnapToScene (wxCommandEvent &event);
	void OnShiftToFrame (wxCommandEvent &event);
	void OnSelectVisible (wxCommandEvent &event);
	void OnSelect (wxCommandEvent &event);
	void OnOpenStylingAssistant (wxCommandEvent &event);
	void OnOpenResample (wxCommandEvent &event);
	void OnOpenTimingProcessor (wxCommandEvent &event);
	void OnOpenKanjiTimer (wxCommandEvent &event);
	void OnOpenVideoDetails (wxCommandEvent &event);
	void OnOpenASSDraw (wxCommandEvent &event);

	void OnOpenPreferences (wxCommandEvent &event);
	void OnGridEvent (wxCommandEvent &event);

	void OnOpenAutomation (wxCommandEvent &event);
	void OnAutomationMacro(wxCommandEvent &event);

	void OnNextFrame(wxCommandEvent &event);
	void OnPrevFrame(wxCommandEvent &event);
	void OnFocusSeek(wxCommandEvent &event);
	void OnNextLine(wxCommandEvent &event);
	void OnPrevLine(wxCommandEvent &event);
	void OnToggleTags(wxCommandEvent &event);

	void OnMedusaPlay(wxCommandEvent &event);
	void OnMedusaStop(wxCommandEvent &event);
	void OnMedusaShiftStartForward(wxCommandEvent &event);
	void OnMedusaShiftStartBack(wxCommandEvent &event);
	void OnMedusaShiftEndForward(wxCommandEvent &event);
	void OnMedusaShiftEndBack(wxCommandEvent &event);
	void OnMedusaPlayBefore(wxCommandEvent &event);
	void OnMedusaPlayAfter(wxCommandEvent &event);
	void OnMedusaEnter(wxCommandEvent &event);
	void OnMedusaNext(wxCommandEvent &event);
	void OnMedusaPrev(wxCommandEvent &event);

	void LoadVideo(wxString filename,bool autoload=false);
	void LoadVFR(wxString filename);
	void LoadSubtitles(wxString filename,wxString charset=_T(""));
	bool SaveSubtitles(bool saveas=false,bool withCharset=false);
	int TryToCloseSubs(bool enableCancel=true);

	void RebuildRecentList(wxString listName,wxMenu *menu,int startID);
	void SynchronizeProject(bool FromSubs=false);

	// AudioControllerAudioEventListener implementation
	void OnAudioOpen(AudioProvider *provider);
	void OnAudioClose();


	void OnSubtitlesFileChanged();


public:

	/// The subtitle editing area
	SubtitlesGrid *SubsGrid;

	/// The subtitle editing textbox
	SubsEditBox *EditBox;

	/// Sash for resizing the audio area
	wxSashWindow *audioSash;

	/// The audio area
	AudioBox *audioBox;

	/// The video area
	VideoBox *videoBox;

	/// DOCME
	DialogDetachedVideo *detachedVideo;

	/// DOCME
	DialogStyling *stylingAssistant;

	/// The audio controller for the open project
	AudioController *audioController;


	/// Arranges things from top to bottom in the window
	wxBoxSizer *MainSizer;

	/// Arranges video box and tool box from left to right
	wxBoxSizer *TopSizer;

	/// Arranges audio and editing areas top to bottom
	wxBoxSizer *ToolsSizer;


	FrameMain (wxArrayString args);
	~FrameMain ();

	bool LoadList(wxArrayString list);
	static void OpenHelp(wxString page=_T(""));
	void UpdateTitle();
	void StatusTimeout(wxString text,int ms=10000);
	void DetachVideo(bool detach=true);

	void SetAccelerators();
	void InitMenu();
	void UpdateToolbar();
	void SetDisplayMode(int showVid,int showAudio);
	
	void SetUndoRedoDesc();
	bool HasASSDraw();

	DECLARE_EVENT_TABLE()
};

