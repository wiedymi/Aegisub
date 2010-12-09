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

/// @file frame_main_events.cpp
/// @brief Event handlers for controls in main window
/// @ingroup main_ui


#include "config.h"

#ifndef AGI_PRE
#include <wx/clipbrd.h>
#include <wx/filename.h>
#include <wx/mimetype.h>
#include <wx/rawbmp.h>
#include <wx/stdpaths.h>
#include <wx/sysopt.h>
#include <wx/tglbtn.h>
#endif

#include "ass_dialogue.h"
#include "ass_file.h"
#include "selection_controller.h"
#include "audio_controller.h"
#include "audio_box.h"
#include "audio_display.h"
#ifdef WITH_AUTOMATION
#include "auto4_base.h"
#endif
#include "command/command.h"
#include "compat.h"
#include "dialog_about.h"
#include "dialog_attachments.h"
#include "dialog_automation.h"
#include "dialog_dummy_video.h"
#include "dialog_export.h"
#include "dialog_fonts_collector.h"
#include "dialog_jumpto.h"
#include "dialog_kara_timing_copy.h"
#include "dialog_log.h"
#include "dialog_progress.h"
#include "dialog_properties.h"
#include "dialog_resample.h"
#include "dialog_search_replace.h"
#include "dialog_selection.h"
#include "dialog_shift_times.h"
#include "dialog_spellchecker.h"
#include "dialog_style_manager.h"
#include "dialog_timing_processor.h"
#include "dialog_translation.h"
#include "dialog_version_check.h"
#include "dialog_video_details.h"
#include "frame_main.h"
#include "hotkeys.h"
#include "include/aegisub/audio_player.h"
#include "keyframe.h"
#include "libresrc/libresrc.h"
#include "main.h"
#include "preferences.h"
#include "standard_paths.h"
#include "selection_controller.h"
#include "subs_edit_box.h"
#include "subs_edit_ctrl.h"
#include "subs_grid.h"
#include "toggle_bitmap.h"
#include "utils.h"
#include "video_box.h"
#include "video_context.h"
#include "video_display.h"
#include "video_slider.h"

#ifdef __APPLE__
#include <libaegisub/util_osx.h>
#endif

BEGIN_EVENT_TABLE(FrameMain, wxFrame)
	EVT_TIMER(ID_APP_TIMER_AUTOSAVE, FrameMain::OnAutoSave)
	EVT_TIMER(ID_APP_TIMER_STATUSCLEAR, FrameMain::OnStatusClear)

	EVT_CLOSE(FrameMain::OnCloseWindow)

	EVT_SASH_DRAGGED(ID_SASH_MAIN_AUDIO, FrameMain::OnAudioBoxResize)

	EVT_MENU_OPEN(FrameMain::OnMenuOpen)

//	EVT_MENU(cmd::id("subtitle/new"), FrameMain::cmd_call)
//	EVT_MENU(cmd::id("subtitle/open"), FrameMain::cmd_call)
//	EVT_MENU(cmd::id("subtitle/save"), FrameMain::cmd_call)

//	EVT_MENU_RANGE(Menu_File_Recent,Menu_File_Recent+99, FrameMain::OnOpenRecentSubs)
//	EVT_MENU_RANGE(Menu_Video_Recent,Menu_Video_Recent+99, FrameMain::OnOpenRecentVideo)
//	EVT_MENU_RANGE(Menu_Audio_Recent,Menu_Audio_Recent+99, FrameMain::OnOpenRecentAudio)
//	EVT_MENU_RANGE(Menu_Timecodes_Recent,Menu_Timecodes_Recent+99, FrameMain::OnOpenRecentTimecodes)
//	EVT_MENU_RANGE(Menu_Keyframes_Recent,Menu_Keyframes_Recent+99, FrameMain::OnOpenRecentKeyframes)
//	EVT_MENU_RANGE(Menu_Automation_Macro,Menu_Automation_Macro+99, FrameMain::OnAutomationMacro)

//	EVT_MENU_RANGE(MENU_GRID_START+1,MENU_GRID_END-1,FrameMain::OnGridEvent)
//	EVT_COMBOBOX(Toolbar_Zoom_Dropdown, FrameMain::OnSetZoom)
//	EVT_TEXT_ENTER(Toolbar_Zoom_Dropdown, FrameMain::OnSetZoom)

#ifdef __WXMAC__
   EVT_MENU(wxID_ABOUT, FrameMain::OnAbout)
   EVT_MENU(wxID_EXIT, FrameMain::OnExit)
#endif
END_EVENT_TABLE()


/// @brief Redirect grid events to grid 
/// @param event 
void FrameMain::OnGridEvent (wxCommandEvent &event) {
	SubsGrid->GetEventHandler()->ProcessEvent(event);
}

/// @brief Rebuild recent list 
/// @param listName 
/// @param menu     
/// @param startID  
void FrameMain::RebuildRecentList(wxString listName,wxMenu *menu,int startID) {
	// Wipe previous list
	int count = (int)menu->GetMenuItemCount();
	for (int i=count;--i>=0;) {
		menu->Destroy(menu->FindItemByPosition(i));
	}

	// Rebuild
	int added = 0;
	wxString n;
	wxArrayString entries = lagi_MRU_wxAS(listName);
	for (size_t i=0;i<entries.Count();i++) {
		n = wxString::Format(_T("%ld"),i+1);
		if (i < 9) n = _T("&") + n;
		wxFileName shortname(entries[i]);
		wxString filename = shortname.GetFullName();
		menu->Append(startID+i,n + _T(" ") + filename);
		added++;
	}

	// Nothing added, add an empty placeholder
	if (added == 0) menu->Append(startID,_("Empty"))->Enable(false);
}

/// @brief Menu is being opened 
/// @param event 
void FrameMain::OnMenuOpen (wxMenuEvent &event) {
	// Get menu
	MenuBar->Freeze();
	wxMenu *curMenu = event.GetMenu();

	// File menu
	if (curMenu == fileMenu) {
		// Rebuild recent
		RebuildRecentList(_T("Subtitle"),RecentSubs,ID_RECENT_FILE);

		MenuBar->Enable(cmd::id("subtitle/open/video"),VideoContext::Get()->HasSubtitles());
	}

	// View menu
	else if (curMenu == viewMenu) {
		// Flags
		bool aud = audioController->IsAudioOpen();
		bool vid = VideoContext::Get()->IsLoaded() && !detachedVideo;

		// Set states
		MenuBar->Enable(cmd::id("app/display/audio_subs"),aud);
		MenuBar->Enable(cmd::id("app/display/video_subs"),vid);
		MenuBar->Enable(cmd::id("app/display/full"),aud && vid);

		// Select option
		if (!showVideo && !showAudio) MenuBar->Check(cmd::id("app/display/subs"),true);
		else if (showVideo && !showAudio) MenuBar->Check(cmd::id("app/display/video_subs"),true);
		else if (showAudio && showVideo) MenuBar->Check(cmd::id("app/display/full"),true);
		else MenuBar->Check(cmd::id("app/display/audio_subs"),true);

		int sub_grid = OPT_GET("Subtitle/Grid/Hide Overrides")->GetInt();
		if (sub_grid == 1) MenuBar->Check(cmd::id("grid/tags/show"), true);
		if (sub_grid == 2) MenuBar->Check(cmd::id("grid/tags/simplify"), true);
		if (sub_grid == 3) MenuBar->Check(cmd::id("grid/tags/hide"), true);


	}

	// Video menu
	else if (curMenu == videoMenu) {
		bool state = VideoContext::Get()->IsLoaded();
		bool attached = state && !detachedVideo;

		// Set states
		MenuBar->Enable(cmd::id("video/jump"),state);
		MenuBar->Enable(cmd::id("video/jump/start"),state);
		MenuBar->Enable(cmd::id("video/jump/end"),state);
		MenuBar->Enable(ID_SM_VIDEO_ZOOM,attached);
		MenuBar->Enable(cmd::id("video/zoom/50"),attached);
		MenuBar->Enable(cmd::id("video/zoom/100"),attached);
		MenuBar->Enable(cmd::id("video/zoom/200"),attached);
		MenuBar->Enable(cmd::id("video/close"),state);
		MenuBar->Enable(ID_SM_VIDEO_OVERRIDE_AR,attached);
		MenuBar->Enable(cmd::id("video/aspect/default"),attached);
		MenuBar->Enable(cmd::id("video/aspect/full"),attached);
		MenuBar->Enable(cmd::id("video/aspect/wide"),attached);
		MenuBar->Enable(cmd::id("video/aspect/cinematic"),attached);
		MenuBar->Enable(cmd::id("video/aspect/custom"),attached);
		MenuBar->Enable(cmd::id("video/detach"),state);
		MenuBar->Enable(cmd::id("timecode/save"),VideoContext::Get()->TimecodesLoaded());
		MenuBar->Enable(cmd::id("timecode/close"),VideoContext::Get()->OverTimecodesLoaded());
		MenuBar->Enable(cmd::id("keyframe/close"),VideoContext::Get()->OverKeyFramesLoaded());
		MenuBar->Enable(cmd::id("keyframe/save"),VideoContext::Get()->KeyFramesLoaded());
		MenuBar->Enable(cmd::id("video/detach"),state);
		MenuBar->Enable(cmd::id("video/show_overscan"),state);

		// Set AR radio
		int arType = VideoContext::Get()->GetAspectRatioType();
		MenuBar->Check(cmd::id("video/aspect/default"),false);
		MenuBar->Check(cmd::id("video/aspect/full"),false);
		MenuBar->Check(cmd::id("video/aspect/wide"),false);
		MenuBar->Check(cmd::id("video/aspect/cinematic"),false);
		MenuBar->Check(cmd::id("video/aspect/custom"),false);
		switch (arType) {
			case 0: MenuBar->Check(cmd::id("video/aspect/default"),true); break;
			case 1: MenuBar->Check(cmd::id("video/aspect/full"),true); break;
			case 2: MenuBar->Check(cmd::id("video/aspect/wide"),true); break;
			case 3: MenuBar->Check(cmd::id("video/aspect/cinematic"),true); break;
			case 4: MenuBar->Check(cmd::id("video/aspect/custom"),true); break;
		}

		// Set overscan mask
		MenuBar->Check(cmd::id("video/show_overscan"),OPT_GET("Video/Overscan Mask")->GetBool());

		// Rebuild recent lists
		RebuildRecentList(_T("Video"),RecentVids,ID_MENU_RECENT_VIDEO);
		RebuildRecentList(_T("Timecodes"),RecentTimecodes,ID_MENU_RECENT_TIMECODES);
		RebuildRecentList(_T("Keyframes"),RecentKeyframes,ID_MENU_RECENT_KEYFRAMES);
	}

	// Audio menu
	else if (curMenu == audioMenu) {
		bool state = audioController->IsAudioOpen();
		bool vidstate = VideoContext::Get()->IsLoaded();

		MenuBar->Enable(cmd::id("audio/open/video"),vidstate);
		MenuBar->Enable(cmd::id("audio/close"),state);

		// Rebuild recent
		RebuildRecentList(_T("Audio"),RecentAuds,ID_MENU_RECENT_AUDIO);
	}

	// Subtitles menu
	else if (curMenu == subtitlesMenu) {
		// Variables
		bool continuous;
		wxArrayInt sels = SubsGrid->GetSelection(&continuous);
		int count = sels.Count();
		bool state,state2;

		// Entries
		state = count > 0;
		MenuBar->Enable(cmd::id("subtitle/insert/before"),state);
		MenuBar->Enable(cmd::id("subtitle/insert/after"),state);
		MenuBar->Enable(cmd::id("edit/line/split/by_karaoke"),state);
		MenuBar->Enable(cmd::id("edit/line/delete"),state);
		state2 = count > 0 && VideoContext::Get()->IsLoaded();
		MenuBar->Enable(cmd::id("subtitle/insert/before/videotime"),state2);
		MenuBar->Enable(cmd::id("subtitle/insert/after/videotime"),state2);
		MenuBar->Enable(ID_SM_SUBTITLES_INSERT,state);
		state = count > 0 && continuous;
		MenuBar->Enable(cmd::id("edit/line/duplicate"),state);
		state = count > 0 && continuous && VideoContext::Get()->TimecodesLoaded();
		MenuBar->Enable(cmd::id("edit/line/duplicate/shift"),state);
		state = count == 2;
		MenuBar->Enable(cmd::id("edit/line/swap"),state);
		state = count >= 2 && continuous;
		MenuBar->Enable(cmd::id("edit/line/join/concatenate"),state);
		MenuBar->Enable(cmd::id("edit/line/join/keep_first"),state);
		MenuBar->Enable(cmd::id("edit/line/join/as_karaoke"),state);
		MenuBar->Enable(ID_SM_SUBTITLES_JOIN,state);
		state = (count == 2 || count == 3) && continuous;
		MenuBar->Enable(cmd::id("edit/line/recombine"),state);
	}

	// Timing menu
	else if (curMenu == timingMenu) {
		// Variables
		bool continuous;
		wxArrayInt sels = SubsGrid->GetSelection(&continuous);
		int count = sels.Count();

		// Video related
		bool state = VideoContext::Get()->IsLoaded();
		MenuBar->Enable(cmd::id("time/snap/start_video"),state);
		MenuBar->Enable(cmd::id("time/snap/end_video"),state);
		MenuBar->Enable(cmd::id("time/snap/scene"),state);
		MenuBar->Enable(cmd::id("time/frame/current"),state);

		// Other
		state = count >= 2 && continuous;
		MenuBar->Enable(cmd::id("time/continous/start"),state);
		MenuBar->Enable(cmd::id("time/continous/end"),state);
	}

	// Edit menu
	else if (curMenu == editMenu) {
		// Undo state
		wxMenuItem *item;
		wxString undo_text = _("&Undo") + wxString(_T(" ")) + ass->GetUndoDescription() + wxString(_T("\t")) + Hotkeys.GetText(_T("Undo"));
		item = editMenu->FindItem(cmd::id("edit/undo"));
		item->SetItemLabel(undo_text);
		item->Enable(!ass->IsUndoStackEmpty());

		// Redo state
		wxString redo_text = _("&Redo") + wxString(_T(" ")) + ass->GetRedoDescription() + wxString(_T("\t")) + Hotkeys.GetText(_T("Redo"));
		item = editMenu->FindItem(cmd::id("edit/redo"));
		item->SetItemLabel(redo_text);
		item->Enable(!ass->IsRedoStackEmpty());

		// Copy/cut/paste
		wxArrayInt sels = SubsGrid->GetSelection();
		bool can_copy = (sels.Count() > 0);

		bool can_paste = true;
		if (wxTheClipboard->Open()) {
			can_paste = wxTheClipboard->IsSupported(wxDF_TEXT);
			wxTheClipboard->Close();
		}

		MenuBar->Enable(cmd::id("edit/line/cut"),can_copy);
		MenuBar->Enable(cmd::id("edit/line/copy"),can_copy);
		MenuBar->Enable(cmd::id("edit/line/paste"),can_paste);
		MenuBar->Enable(cmd::id("edit/line/paste/over"),can_copy&&can_paste);
	}

	// Automation menu
#ifdef WITH_AUTOMATION
	else if (curMenu == automationMenu) {
		// Remove old macro items
		for (unsigned int i = 0; i < activeMacroItems.size(); i++) {
			wxMenu *p = 0;
			wxMenuItem *it = MenuBar->FindItem(ID_MENU_AUTOMATION_MACRO + i, &p);
			if (it)
				p->Delete(it);
		}
		activeMacroItems.clear();

		// Add new ones
		int added = 0;
		added += AddMacroMenuItems(automationMenu, wxGetApp().global_scripts->GetMacros());
		added += AddMacroMenuItems(automationMenu, local_scripts->GetMacros());

		// If none were added, show a ghosted notice
		if (added == 0) {
			automationMenu->Append(ID_MENU_AUTOMATION_MACRO, _("No Automation macros loaded"))->Enable(false);
			activeMacroItems.push_back(0);
		}
	}
#endif

	MenuBar->Thaw();
}

/// @brief Macro menu creation helper 
/// @param menu   
/// @param macros 
/// @return 
int FrameMain::AddMacroMenuItems(wxMenu *menu, const std::vector<Automation4::FeatureMacro*> &macros) {
#ifdef WITH_AUTOMATION
	if (macros.empty()) {
		return 0;
	}

	int id = activeMacroItems.size();;
	for (std::vector<Automation4::FeatureMacro*>::const_iterator i = macros.begin(); i != macros.end(); ++i) {
		wxMenuItem * m = menu->Append(ID_MENU_AUTOMATION_MACRO + id, (*i)->GetName(), (*i)->GetDescription());
		m->Enable((*i)->Validate(SubsGrid->ass, SubsGrid->GetAbsoluteSelection(), SubsGrid->GetFirstSelRow()));
		activeMacroItems.push_back(*i);
		id++;
	}

	return macros.size();
#else
	return 0;
#endif
}

/// @brief Open recent subs menu entry 
/// @param event 
void FrameMain::OnOpenRecentSubs(wxCommandEvent &event) {
	int number = event.GetId()-ID_RECENT_FILE;
	LoadSubtitles(lagi_wxString(config::mru->GetEntry("Subtitle", number)));
}

/// @brief Open recent video menu entry 
/// @param event 
void FrameMain::OnOpenRecentVideo(wxCommandEvent &event) {
	int number = event.GetId()-ID_MENU_RECENT_VIDEO;
	LoadVideo(lagi_wxString(config::mru->GetEntry("Video", number)));
}

/// @brief Open recent timecodes entry 
/// @param event 
void FrameMain::OnOpenRecentTimecodes(wxCommandEvent &event) {
	int number = event.GetId()-ID_MENU_RECENT_TIMECODES;
	LoadVFR(lagi_wxString(config::mru->GetEntry("Timecodes", number)));
}

/// @brief Open recent Keyframes entry 
/// @param event 
void FrameMain::OnOpenRecentKeyframes(wxCommandEvent &event) {
printf("THIS IS BROKEN\n");
	VideoContext::Get()->LoadKeyframes(lagi_wxString(config::mru->GetEntry("Keyframes", event.GetId()-ID_SM_VIDEO_ID_MENU_RECENT_KEYFRAMES)));
}

/// @brief Open recent audio menu entry 
/// @param event 
void FrameMain::OnOpenRecentAudio(wxCommandEvent &event) {
printf("THIS IS BROKEN\n");
	audioController->OpenAudio(lagi_wxString(config::mru->GetEntry("Audio", event.GetId()-ID_SM_AUDIO_ID_MENU_RECENT_AUDIO)));
}

/// @brief Play video 
void FrameMain::OnVideoPlay(wxCommandEvent &) {
	VideoContext::Get()->Play();
}


/// @brief Open video 
void FrameMain::OnOpenVideo(wxCommandEvent&) {
	wxString path = lagi_wxString(OPT_GET("Path/Last/Video")->GetString());
	wxString str = wxString(_("Video Formats")) + _T(" (*.avi,*.mkv,*.mp4,*.avs,*.d2v,*.ogm,*.mpeg,*.mpg,*.vob,*.mov)|*.avi;*.avs;*.d2v;*.mkv;*.ogm;*.mp4;*.mpeg;*.mpg;*.vob;*.mov|")
				 + _("All Files") + _T(" (*.*)|*.*");
	wxString filename = wxFileSelector(_("Open video file"),path,_T(""),_T(""),str,wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (!filename.empty()) {
		LoadVideo(filename);
		OPT_SET("Path/Last/Video")->SetString(STD_STR(filename));
	}
}

/// @brief Close video 
void FrameMain::OnCloseVideo(wxCommandEvent&) {
	LoadVideo(_T(""));
}


/// @brief Open subtitles 
void FrameMain::OnOpenSubtitles(wxCommandEvent&) {
	wxString path = lagi_wxString(OPT_GET("Path/Last/Subtitles")->GetString());	
	wxString filename = wxFileSelector(_("Open subtitles file"),path,_T(""),_T(""),AssFile::GetWildcardList(0),wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (!filename.empty()) {
		LoadSubtitles(filename);
		wxFileName filepath(filename);
		OPT_SET("Path/Last/Subtitles")->SetString(STD_STR(filepath.GetPath()));
	}
}

/// @brief Open subtitles with specific charset 
void FrameMain::OnOpenSubtitlesCharset(wxCommandEvent&) {
	// Initialize charsets
	wxString path = lagi_wxString(OPT_GET("Path/Last/Subtitles")->GetString());

	// Get options and load
	wxString filename = wxFileSelector(_("Open subtitles file"),path,_T(""),_T(""),AssFile::GetWildcardList(0),wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (!filename.empty()) {
		wxString charset = wxGetSingleChoice(_("Choose charset code:"), _("Charset"),agi::charset::GetEncodingsList<wxArrayString>(),this,-1, -1,true,250,200);
		if (!charset.empty()) {
			LoadSubtitles(filename,charset);
		}
		OPT_SET("Path/Last/Subtitles")->SetString(STD_STR(filename));
	}
}

/// @brief Open subtitles from the currently open video file
void FrameMain::OnOpenSubtitlesVideo(wxCommandEvent&) {
	LoadSubtitles(VideoContext::Get()->videoName, "binary");
}

/// @brief Save subtitles as 
void FrameMain::OnSaveSubtitlesAs(wxCommandEvent&) {
	SaveSubtitles(true);
}

/// @brief Save subtitles 
void FrameMain::OnSaveSubtitles(wxCommandEvent&) {
	SaveSubtitles(false);
}

/// @brief Save subtitles with specific charset 
void FrameMain::OnSaveSubtitlesCharset(wxCommandEvent&) {
	SaveSubtitles(true,true);
}

/// @brief Close subtitles 
void FrameMain::OnNewSubtitles(wxCommandEvent&) {
	LoadSubtitles(_T(""));
}

/// @brief Export subtitles 
void FrameMain::OnExportSubtitles(wxCommandEvent &) {
#ifdef WITH_AUTOMATION
	int autoreload = OPT_GET("Automation/Autoreload Mode")->GetInt();
	if (autoreload & 1) {
		// Local scripts
		const std::vector<Automation4::Script*> scripts = local_scripts->GetScripts();
		for (size_t i = 0; i < scripts.size(); ++i) {
			try {
				scripts[i]->Reload();
			}
			catch (const wchar_t *e) {
				wxLogError(_T("Error while reloading Automation scripts before export: %s"), e);
			}
			catch (...) {
				wxLogError(_T("An unknown error occurred reloading Automation script '%s'."), scripts[i]->GetName().c_str());
			}
		}
	}
	if (autoreload & 2) {
		// Global scripts
		wxGetApp().global_scripts->Reload();
	}
#endif

	DialogExport exporter(this, ass);
	exporter.ShowModal();
}

/// @brief Zoom levels 
void FrameMain::OnSetZoom50(wxCommandEvent&) {
	VideoContext::Get()->Stop();
	videoBox->videoDisplay->SetZoom(.5);
}


/// @brief DOCME
void FrameMain::OnSetZoom100(wxCommandEvent&) {
	VideoContext::Get()->Stop();
	videoBox->videoDisplay->SetZoom(1.);
}


/// @brief DOCME
void FrameMain::OnSetZoom200(wxCommandEvent&) {
	VideoContext::Get()->Stop();
	videoBox->videoDisplay->SetZoom(2.);
}


/// @brief DOCME
void FrameMain::OnZoomIn (wxCommandEvent &) {
	VideoContext::Get()->Stop();
	videoBox->videoDisplay->SetZoom(videoBox->videoDisplay->GetZoom() + .125);
}


/// @brief DOCME
void FrameMain::OnZoomOut (wxCommandEvent &) {
	VideoContext::Get()->Stop();
	videoBox->videoDisplay->SetZoom(videoBox->videoDisplay->GetZoom() - .125);
}


/// @brief DOCME
void FrameMain::OnSetZoom(wxCommandEvent &) {
	videoBox->videoDisplay->SetZoomFromBox();
}

/// @brief Detach video 
void FrameMain::OnDetachVideo(wxCommandEvent &) {
	DetachVideo(!detachedVideo);
}

/// @brief Use dummy video 
void FrameMain::OnDummyVideo (wxCommandEvent &) {
	wxString fn;
	if (DialogDummyVideo::CreateDummyVideo(this, fn)) {
		LoadVideo(fn);
	}
}

/// @brief Overscan toggle 
void FrameMain::OnOverscan (wxCommandEvent &event) {
	OPT_SET("Video/Overscan Mask")->SetBool(event.IsChecked());
	VideoContext::Get()->Stop();
	videoBox->videoDisplay->Render();
}

/// @brief Show video details 
void FrameMain::OnOpenVideoDetails (wxCommandEvent &) {
	VideoContext::Get()->Stop();
	DialogVideoDetails videodetails(this);
	videodetails.ShowModal();
}

/// @brief Open jump to dialog 
void FrameMain::OnJumpTo(wxCommandEvent&) {
	VideoContext::Get()->Stop();
	if (VideoContext::Get()->IsLoaded()) {
		DialogJumpTo JumpTo(this);
		JumpTo.ShowModal();
		videoBox->videoSlider->SetFocus();
	}
}

/// @brief Open shift dialog 
void FrameMain::OnShift(wxCommandEvent&) {
	VideoContext::Get()->Stop();
	DialogShiftTimes Shift(this,SubsGrid);
	Shift.ShowModal();
}

/// @brief Open properties 
void FrameMain::OnOpenProperties (wxCommandEvent &) {
	VideoContext::Get()->Stop();
	DialogProperties Properties(this, ass);
	Properties.ShowModal();
}

/// @brief Open attachments 
void FrameMain::OnOpenAttachments(wxCommandEvent&) {
	VideoContext::Get()->Stop();
	DialogAttachments attachments(this, ass);
	attachments.ShowModal();
}

/// @brief Open Spell Checker 
void FrameMain::OnOpenSpellCheck (wxCommandEvent &) {
	VideoContext::Get()->Stop();
	new DialogSpellChecker(this);
}


/// @brief General handler for all Automation-generated menu items
/// @param event 
void FrameMain::OnAutomationMacro (wxCommandEvent &event) {
#ifdef WITH_AUTOMATION
	SubsGrid->BeginBatch();
	// First get selection data
	std::vector<int> selected_lines = SubsGrid->GetAbsoluteSelection();
	int first_sel = SubsGrid->GetFirstSelRow();
	// Run the macro...
	activeMacroItems[event.GetId()-ID_MENU_AUTOMATION_MACRO]->Process(SubsGrid->ass, selected_lines, first_sel, this);
	SubsGrid->SetSelectionFromAbsolute(selected_lines);
	SubsGrid->EndBatch();
#endif
}

/// @brief Snap subs to video 
void FrameMain::OnSnapSubsStartToVid (wxCommandEvent &) {
	SubsGrid->SetSubsToVideo(true);
}

/// @brief DOCME
void FrameMain::OnSnapSubsEndToVid (wxCommandEvent &) {
	SubsGrid->SetSubsToVideo(false);
}

/// @brief Jump video to subs 
void FrameMain::OnSnapVidToSubsStart (wxCommandEvent &) {
	SubsGrid->SetVideoToSubs(true);
}


/// @brief DOCME
void FrameMain::OnSnapVidToSubsEnd (wxCommandEvent &) {
	SubsGrid->SetVideoToSubs(false);
}

/// @brief Snap to scene 
void FrameMain::OnSnapToScene (wxCommandEvent &) {
	VideoContext *con = VideoContext::Get();
	if (!con->IsLoaded() || !con->KeyFramesLoaded()) return;

	// Get frames
	wxArrayInt sel = SubsGrid->GetSelection();
	int curFrame = con->GetFrameN();
	int prev = 0;
	int next = 0;

	const std::vector<int> &keyframes = con->GetKeyFrames();
	if (curFrame < keyframes.front()) {
		next = keyframes.front();
	}
	else if (curFrame >= keyframes.back()) {
		prev = keyframes.back();
		next = con->GetLength();
	}
	else {
		std::vector<int>::const_iterator kf = std::lower_bound(keyframes.begin(), keyframes.end(), curFrame);
		if (*kf == curFrame) {
			prev = *kf;
			next = *(kf + 1);
		}
		else {
			prev = *(kf - 1);
			next = *kf;
		}
	}

	// Get times
	int start_ms = con->TimeAtFrame(prev,agi::vfr::START);
	int end_ms = con->TimeAtFrame(next-1,agi::vfr::END);
	AssDialogue *cur;

	// Update rows
	for (size_t i=0;i<sel.Count();i++) {
		cur = SubsGrid->GetDialogue(sel[i]);
		cur->Start.SetMS(start_ms);
		cur->End.SetMS(end_ms);
	}

	// Commit
	SubsGrid->ass->Commit(_("snap to scene"), AssFile::COMMIT_TIMES);
}

/// @brief Shift to frame 
void FrameMain::OnShiftToFrame (wxCommandEvent &) {
	if (!VideoContext::Get()->IsLoaded()) return;

	wxArrayInt sels = SubsGrid->GetSelection();
	size_t n=sels.Count();
	if (n == 0) return;

	// Get shifting in ms
	AssDialogue *cur = SubsGrid->GetDialogue(sels[0]);
	if (!cur) return;
	int shiftBy = VideoContext::Get()->TimeAtFrame(VideoContext::Get()->GetFrameN(),agi::vfr::START) - cur->Start.GetMS();

	// Update
	for (size_t i=0;i<n;i++) {
		cur = SubsGrid->GetDialogue(sels[i]);
		if (cur) {
			cur->Start.SetMS(cur->Start.GetMS()+shiftBy);
			cur->End.SetMS(cur->End.GetMS()+shiftBy);
		}
	}

	// Commit
	SubsGrid->ass->Commit(_("shift to frame"), AssFile::COMMIT_TIMES);
}

/// @brief Find 
void FrameMain::OnFind(wxCommandEvent &) {
	VideoContext::Get()->Stop();
	Search.OpenDialog(false);
}

/// @brief Find next 
void FrameMain::OnFindNext(wxCommandEvent &) {
	VideoContext::Get()->Stop();
	Search.FindNext();
}

/// @brief Change aspect ratio to default 
void FrameMain::OnSetARDefault (wxCommandEvent &) {
	VideoContext::Get()->Stop();
	VideoContext::Get()->SetAspectRatio(0);
	SetDisplayMode(1,-1);
}

/// @brief Change aspect ratio to fullscreen 
void FrameMain::OnSetARFull (wxCommandEvent &) {
	VideoContext::Get()->Stop();
	VideoContext::Get()->SetAspectRatio(1);
	SetDisplayMode(1,-1);
}

/// @brief Change aspect ratio to widescreen 
void FrameMain::OnSetARWide (wxCommandEvent &) {
	VideoContext::Get()->Stop();
	VideoContext::Get()->SetAspectRatio(2);
	SetDisplayMode(1,-1);
}

/// @brief Change aspect ratio to 2:35 
void FrameMain::OnSetAR235 (wxCommandEvent &) {
	VideoContext::Get()->Stop();
	VideoContext::Get()->SetAspectRatio(3);
	SetDisplayMode(1,-1);
}

/// @brief Change aspect ratio to a custom value 
void FrameMain::OnSetARCustom (wxCommandEvent &) {
	VideoContext::Get()->Stop();

	wxString value = wxGetTextFromUser(_("Enter aspect ratio in either:\n  decimal (e.g. 2.35)\n  fractional (e.g. 16:9)\n  specific resolution (e.g. 853x480)"),_("Enter aspect ratio"),AegiFloatToString(VideoContext::Get()->GetAspectRatioValue()));
	if (value.IsEmpty()) return;

	value.MakeLower();

	// Process text
	double numval;
	if (value.ToDouble(&numval)) {
		//Nothing to see here, move along
	}
	else {
		double a,b;
		int pos=0;
		bool scale=false;

		//Why bloat using Contains when we can just check the output of Find?
		pos = value.Find(':');
		if (pos==wxNOT_FOUND) pos = value.Find('/');
		if (pos==wxNOT_FOUND&&value.Contains(_T('x'))) {
			pos = value.Find('x');
			scale=true;
		}

		if (pos>0) {
			wxString num = value.Left(pos);
			wxString denum = value.Mid(pos+1);
			if (num.ToDouble(&a) && denum.ToDouble(&b) && b!=0) {
				numval = a/b;
				if (scale) videoBox->videoDisplay->SetZoom(b / VideoContext::Get()->GetHeight());
			}
		}
		else numval = 0.0;
	}

	// Sanity check
	if (numval < 0.5 || numval > 5.0) wxMessageBox(_("Invalid value! Aspect ratio must be between 0.5 and 5.0."),_("Invalid Aspect Ratio"),wxICON_ERROR|wxOK);

	// Set value
	else {
		VideoContext::Get()->SetAspectRatio(4,numval);
		SetDisplayMode(1,-1);
	}
}

/// @brief Window is attempted to be closed
/// @param event
void FrameMain::OnCloseWindow (wxCloseEvent &event) {
	// Stop audio and video
	VideoContext::Get()->Stop();
	audioController->Stop();

	// Ask user if he wants to save first
	bool canVeto = event.CanVeto();
	int result = TryToCloseSubs(canVeto);

	// Store maximization state
	OPT_SET("App/Maximized")->SetBool(IsMaximized());

	// Abort/destroy
	if (canVeto) {
		if (result == wxCANCEL) event.Veto();
		else Destroy();
	}
	else Destroy();
}




/// @brief Select visible lines 
void FrameMain::OnSelectVisible (wxCommandEvent &) {
	VideoContext::Get()->Stop();
	SubsGrid->SelectVisible();
}

/// @brief Sort subtitles by start time
void FrameMain::OnSortStart (wxCommandEvent &) {
	ass->Sort();
	ass->Commit(_("sort"));
}
/// @brief Sort subtitles by end time
void FrameMain::OnSortEnd (wxCommandEvent &) {
	ass->Sort(AssFile::CompEnd);
	ass->Commit(_("sort"));
}
/// @brief Sort subtitles by style name
void FrameMain::OnSortStyle (wxCommandEvent &) {
	ass->Sort(AssFile::CompStyle);
	ass->Commit(_("sort"));
}


/// @brief Autosave the currently open file, if any
void FrameMain::OnAutoSave(wxTimerEvent &) {
	try {
		if (ass->loaded && ass->IsModified()) {
			// Set path
			wxFileName origfile(ass->filename);
			wxString path = lagi_wxString(OPT_GET("Path/Auto/Save")->GetString());
			if (path.IsEmpty()) path = origfile.GetPath();
			wxFileName dstpath(path);
			if (!dstpath.IsAbsolute()) path = StandardPaths::DecodePathMaybeRelative(path, _T("?user/"));
			dstpath.AssignDir(path);
			if (!dstpath.DirExists()) wxMkdir(path);

			wxString name = origfile.GetName();
			if (name.IsEmpty()) {
				dstpath.SetFullName("Untitled.AUTOSAVE.ass");
			}
			else {
				dstpath.SetFullName(name + L".AUTOSAVE.ass");
			}

			ass->Save(dstpath.GetFullPath(),false,false);

			// Set status bar
			StatusTimeout(_("File backup saved as \"") + dstpath.GetFullPath() + _T("\"."));
		}
	}
	catch (const agi::Exception& err) {
		StatusTimeout(lagi_wxString("Exception when attempting to autosave file: " + err.GetMessage()));
	}
	catch (wxString err) {
		StatusTimeout(_T("Exception when attempting to autosave file: ") + err);
	}
	catch (const wchar_t *err) {
		StatusTimeout(_T("Exception when attempting to autosave file: ") + wxString(err));
	}
	catch (...) {
		StatusTimeout(_T("Unhandled exception when attempting to autosave file."));
	}
}

/// @brief Clear statusbar 
void FrameMain::OnStatusClear(wxTimerEvent &) {
	SetStatusText(_T(""),1);
}

/// @brief Next frame hotkey 
void FrameMain::OnNextFrame(wxCommandEvent &) {
	videoBox->videoSlider->NextFrame();
}

/// @brief Previous frame hotkey 
void FrameMain::OnPrevFrame(wxCommandEvent &) {
	videoBox->videoSlider->PrevFrame();
}

/// @brief Toggle focus between seek bar and whatever else 
void FrameMain::OnFocusSeek(wxCommandEvent &) {
	wxWindow *curFocus = wxWindow::FindFocus();
	if (curFocus == videoBox->videoSlider) {
		if (PreviousFocus) PreviousFocus->SetFocus();
	}
	else {
		PreviousFocus = curFocus;
		videoBox->videoSlider->SetFocus();
	}
}


void FrameMain::OnAudioBoxResize(wxSashEvent &event)
{
	if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
		return;

	wxRect rect = event.GetDragRect();

	if (rect.GetHeight() < audioSash->GetMinimumSizeY())
		rect.SetHeight(audioSash->GetMinimumSizeY());

	audioBox->SetMinSize(wxSize(-1, rect.GetHeight()));
	Panel->Layout();
	Refresh();
}

void FrameMain::OnAudioOpen(AudioProvider *provider)
{
	SetDisplayMode(-1, 1);
}

void FrameMain::OnAudioClose()
{
	SetDisplayMode(-1, 0);
}

void FrameMain::OnSubtitlesFileChanged() {
	if (OPT_GET("App/Auto/Save on Every Change")->GetBool()) {
		if (ass->IsModified() && !ass->filename.empty()) SaveSubtitles(false);
	}

	UpdateTitle();
}
