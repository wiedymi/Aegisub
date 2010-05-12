// Copyright (c) 2010, Amar Takhar <verm@aegisub.org>
//
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
// $Id$

/// @file preferences.cpp
/// @brief Preferences dialogue
/// @ingroup configuration_ui


#ifndef AGI_PRE
#include <wx/filefn.h>
#include <wx/spinctrl.h>
#include <wx/stdpaths.h>
#include <wx/treebook.h>
#endif

//#include <libaegisub/option_value.h>

#include "libresrc/libresrc.h"
#include "preferences.h"
#include "main.h"
#include "subtitles_provider_manager.h"
#include "video_provider_manager.h"

Preferences::Preferences(wxWindow *parent): wxDialog(parent, -1, _("Preferences"), wxDefaultPosition, wxSize(500,500)) {
//	SetIcon(BitmapToIcon(GETIMAGE(options_button_24)));

	book = new wxTreebook(this, -1, wxDefaultPosition, wxDefaultSize);

	General(book);
	Subtitles(book);
	Audio(book);
	Video(book);
	Interface(book);
	Interface_Colours(book);
	Interface_Hotkeys(book);
	Paths(book);
	File_Associations(book);
	Backup(book);
	Advanced(book);
	Advanced_Interface(book);
	Advanced_Audio(book);
	Advanced_Video(book);

	book->Fit();

	/// @todo Save the last page and start with that page on next launch.
	book->ChangeSelection(3);

	// Bottom Buttons
	wxStdDialogButtonSizer *stdButtonSizer = new wxStdDialogButtonSizer();
	stdButtonSizer->AddButton(new wxButton(this,wxID_OK));
	stdButtonSizer->AddButton(new wxButton(this,wxID_CANCEL));
	stdButtonSizer->AddButton(new wxButton(this,wxID_APPLY));
	stdButtonSizer->Realize();
	wxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	wxButton *defaultButton = new wxButton(this,2342,_("Restore Defaults"));
	buttonSizer->Add(defaultButton,0,wxEXPAND);
	buttonSizer->AddStretchSpacer(1);
	buttonSizer->Add(stdButtonSizer,0,wxEXPAND);


	// Main Sizer
	wxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(book, 1 ,wxEXPAND | wxALL, 5);
	mainSizer->Add(buttonSizer,0,wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM,5);
//	mainSizer->SetSizeHints(this);
	SetSizerAndFit(mainSizer);
	this->SetMinSize(wxSize(500,500));
	CenterOnParent();



}

void Preferences::OptionChoice(wxPanel *parent, wxFlexGridSizer *flex, const wxString &name, const wxArrayString &choices, const char *opt_name) {
	agi::OptionValue *opt = OPT_GET(opt_name);

	int type = opt->GetType();
	wxString selection;

	switch (type) {
		case agi::OptionValue::Type_Int: {
			selection = choices.Item(opt->GetInt());
			break;
		}
		case agi::OptionValue::Type_String: {
			selection.assign(opt->GetString());
			break;
		}

//		default:
		// throw
	}

	flex->Add(new wxStaticText(parent, wxID_ANY, name), 1, wxALIGN_CENTRE_VERTICAL);
	wxComboBox *cb = new wxComboBox(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY | wxCB_DROPDOWN);
	cb->SetValue(selection);
	flex->Add(cb, 1, wxEXPAND, 0);
}


void Preferences::OptionAdd(wxPanel *parent, wxFlexGridSizer *flex, const wxString &name, const char *opt_name, double min, double max, double inc) {

	agi::OptionValue *opt = OPT_GET(opt_name);

	int type = opt->GetType();

	switch (type) {

		case agi::OptionValue::Type_Bool: {
			wxCheckBox *cb = new wxCheckBox(parent, wxID_ANY, name);
			flex->Add(cb, 1, wxEXPAND, 0);
			cb->SetValue(opt->GetBool());
			break;
		}

		case agi::OptionValue::Type_Int:
		case agi::OptionValue::Type_Double: {
			flex->Add(new wxStaticText(parent, wxID_ANY, name), 1, wxALIGN_CENTRE_VERTICAL);
			wxSpinCtrlDouble *scd = new wxSpinCtrlDouble(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, min, max, opt->GetInt(), inc);
			flex->Add(scd);
			break;
		}

		case agi::OptionValue::Type_String: {
			break;
		}

		case agi::OptionValue::Type_Colour: {
			break;
		}

//		default:
			// throw here
	}

}

Preferences::~Preferences() {

}


void Preferences::OnOK(wxCommandEvent &event) {
	EndModal(0);
}


void Preferences::OnApply(wxCommandEvent &event) {
}


void Preferences::OnCancel(wxCommandEvent &event) {
	EndModal(0);
}


#define PAGE_CREATE(name)                           \
	wxPanel *panel = new wxPanel(book, -1);         \
	book->AddPage(panel, name, true);               \
	wxSizer *sizer = new wxBoxSizer(wxVERTICAL);    \

#define SUBPAGE_CREATE(name)                        \
	wxPanel *panel = new wxPanel(book, -1);         \
	book->AddSubPage(panel, name, true);            \
	wxSizer *sizer = new wxBoxSizer(wxVERTICAL);    \


#define PAGE_SIZER(name, name_value)                                                           \
	wxSizer *name_value##_sizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, name);            \
	sizer->Add(name_value##_sizer, 0,wxEXPAND, 5);                                             \
	wxFlexGridSizer *name_value##_flex = new wxFlexGridSizer(2,5,5);                           \
	name_value##_flex->AddGrowableCol(0,1);                                                    \
	name_value##_sizer->Add(name_value##_flex, 1, wxEXPAND, 5);                                \
	sizer->AddSpacer(8);

#define PAGE_END() \
	panel->SetSizerAndFit(sizer);



void Preferences::Subtitles(wxTreebook *book) {
	PAGE_CREATE(_("Subtitles"))
	PAGE_END()
}


void Preferences::General(wxTreebook *book) {

	PAGE_CREATE(_("General"))

	PAGE_SIZER(_("Startup"), startup)
	OptionAdd(panel, startup_flex, _("Check for updates"), "App/Splash");
	OptionAdd(panel, startup_flex, _("Show Splash Screen"), "App/Splash");

	PAGE_SIZER(_("Recently Used Lists"), recent)
	OptionAdd(panel, recent_flex, _("Files"), "Limits/MRU");
	OptionAdd(panel, recent_flex, _("Find/Replace"), "Limits/Find Replace");
	sizer->AddSpacer(15);

	PAGE_SIZER(_("Undo / Redo Settings"), undo)
	OptionAdd(panel, undo_flex, _("Undo Levels"), "Limits/MRU");

	PAGE_END()
}


void Preferences::Audio(wxTreebook *book) {
	PAGE_CREATE(_("Audio"))
	PAGE_END()
}


void Preferences::Video(wxTreebook *book) {

	PAGE_CREATE(_("Video"))

	PAGE_SIZER(_("Options"), general)

	OptionAdd(panel, general_flex, _("Show keyframes in slider"), "Video/Slider/Show Keyframes");
	OptionAdd(panel, general_flex, _("Always show visual tools"), "Tool/Visual/Always Show");

	const wxString cres_arr[3] = { _("Never"), _("Ask"), _("Always") };
	wxArrayString choice_res(3, cres_arr);
	OptionChoice(panel, general_flex, _("Match video resolution on open"), choice_res, "Video/Check Script Res");

	const wxString czoom_arr[24] = { _T("12.5%"), _T("25%"), _T("37.5%"), _T("50%"), _T("62.5%"), _T("75%"), _T("87.5%"), _T("100%"), _T("112.5%"), _T("125%"), _T("137.5%"), _T("150%"), _T("162.5%"), _T("175%"), _T("187.5%"), _T("200%"), _T("212.5%"), _T("225%"), _T("237.5%"), _T("250%"), _T("262.5%"), _T("275%"), _T("287.5%"), _T("300%") };
	wxArrayString choice_zoom(24, czoom_arr);
	OptionChoice(panel, general_flex, _("Default Zoom"), choice_zoom, "Video/Default Zoom");

	OptionAdd(panel, general_flex, _("Fast jump step in frames"), "Video/Slider/Fast Jump Step");

	const wxString cscr_arr[3] = { _("?video"), _("?script"), _(".") };
	wxArrayString scr_res(3, cscr_arr);
	OptionChoice(panel, general_flex, _("Screenshot save path"), scr_res, "Path/Screenshot");



	panel->SetSizerAndFit(sizer);


}


void Preferences::Interface(wxTreebook *book) {
	PAGE_CREATE(_("Interface"))
	PAGE_END()
}

void Preferences::Interface_Colours(wxTreebook *book) {
	SUBPAGE_CREATE(_("Colours"))
	PAGE_END()
}

void Preferences::Interface_Hotkeys(wxTreebook *book) {
	SUBPAGE_CREATE(_("Hotkeys"))
	PAGE_END()
}

void Preferences::Paths(wxTreebook *book) {
	PAGE_CREATE(_("Paths"))
	PAGE_END()
}

void Preferences::File_Associations(wxTreebook *book) {
	PAGE_CREATE(_("File Assoc."))
	PAGE_END()
}

void Preferences::Backup(wxTreebook *book) {
	PAGE_CREATE(_("Backup"))
	PAGE_END()
}

void Preferences::Advanced(wxTreebook *book) {
	PAGE_CREATE(_("Advanced"))
	PAGE_END()
}

void Preferences::Advanced_Interface(wxTreebook *book) {
	SUBPAGE_CREATE(_("Interface"))
	PAGE_END()
}

void Preferences::Advanced_Audio(wxTreebook *book) {
	SUBPAGE_CREATE(_("Audio"))
	PAGE_END()
}

void Preferences::Advanced_Video(wxTreebook *book) {
	SUBPAGE_CREATE(_("Video"))


	wxStaticText *warning = new wxStaticText(panel, wxID_ANY ,_("Changing these settings might result in bugs and/or crashes.  Do not touch these unless you know what you're doing."));
	warning->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	sizer->Fit(panel);
	warning->Wrap(400);
	sizer->Add(warning, 0, wxALL, 5);

	PAGE_SIZER(_("Options"), expert)
	wxArrayString vp_choice = VideoProviderFactoryManager::GetFactoryList();
	OptionChoice(panel, expert_flex, _("Video provider"), vp_choice, "Video/Provider");

	wxArrayString sp_choice = SubtitlesProviderFactoryManager::GetFactoryList();
	OptionChoice(panel, expert_flex, _("Subtitle provider"), sp_choice, "Subtitle/Provider");

#ifdef WIN32
	PAGE_SIZER(_("Windows Only"), windows);

	OptionAdd(panel, windows_flex, _("Allow pre-2.56a Avisynth"), "Provider/Avisynth/Allow Ancient");
	OptionAdd(panel, windows_flex, _("Avisynth memory limit"), "Provider/Avisynth/Memory Max");
#endif

	PAGE_END()
}




BEGIN_EVENT_TABLE(Preferences, wxDialog)
    EVT_BUTTON(wxID_OK, Preferences::OnOK)
    EVT_BUTTON(wxID_CANCEL, Preferences::OnCancel)
    EVT_BUTTON(wxID_APPLY, Preferences::OnApply)
END_EVENT_TABLE()

