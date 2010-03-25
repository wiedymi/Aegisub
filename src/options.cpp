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
// Aegisub Project http://www.aegisub.org/
//
// $Id$

/// @file options.cpp
/// @brief Initialise, load and store configuration settings, including all defaults
/// @ingroup main
///


////////////
// Includes
#include "config.h"

#ifndef AGI_PRE
#include <fstream>
#include <string>

#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/stopwatch.h>
#include <wx/utils.h>
#endif

#include "colorspace.h"
#include "compat.h"
#include "main.h"
#include "options.h"
#include "text_file_reader.h"
#include "text_file_writer.h"
#include "utils.h"


/// @brief Constructor 
///
OptionsManager::OptionsManager() {
	modified = false;
	overriding = false;
	lastVersion = -1;
}



/// @brief Destructor 
///
OptionsManager::~OptionsManager() {
	Clear();
}



/// @brief Clear 
///
void OptionsManager::Clear() {
	opt.clear();
	optType.clear();
}



/// @brief Set filename 
/// @param file 
///
void OptionsManager::SetFile(wxString file) {
	filename = file;
}



/// @brief Get filename 
/// @return 
///
wxString OptionsManager::GetFile() const
{
	return filename;
}



/// @brief Save 
/// @return 
///
void OptionsManager::Save() {
	// Check if it's actually modified
	if (!modified) return;

	// Open file
	TextFileWriter file(filename,_T("UTF-8"));
	file.WriteLineToFile(_T("[Config]"));

	// Put variables in it
	for (std::map<wxString,VariableData>::iterator cur=opt.begin();cur!=opt.end();cur++) {
		file.WriteLineToFile((*cur).first + _T("=") + (*cur).second.AsText());
	}

	// Close
	modified = false;
}



/// @brief Load 
/// @return 
///
void OptionsManager::Load() {
	// Check if file exists
	wxFileName path(filename);
	if (!path.FileExists()) {
		modified = true;
		return;
	}

	// Read header
	TextFileReader file(filename);
	wxString header;
	try {
		if (file.GetCurrentEncoding() != _T("binary"))
			header = file.ReadLineFromFile();
	}
	catch (wxString e) {
		header = _T("");
	}
	if (header != _T("[Config]")) {
		wxMessageBox(_("Configuration file is either invalid or corrupt. The current file will be backed up and replaced with a default file."),_("Error"),wxCENTRE|wxICON_WARNING|wxOK);
		wxRenameFile(filename,filename + wxString::Format(_T(".%i.backup"),wxGetUTCTime()));
		modified = true;
		return;
	}

	// Get variables
	std::map<wxString,VariableData>::iterator cur;
	wxString curLine;
	while (file.HasMoreLines()) {
		// Parse line
		try {
			curLine = file.ReadLineFromFile();
		}
		catch (wxString e) {
			wxMessageBox(_("Configuration file is either invalid or corrupt. The current file will be backed up and replaced with a default file."),_("Error"),wxCENTRE|wxICON_WARNING|wxOK);
			wxRenameFile(filename,filename + wxString::Format(_T(".%i.backup"),wxGetUTCTime()));
			modified = true;
			return;
		}
		if (curLine.IsEmpty()) continue;
		size_t pos = curLine.Find(_T("="));
		if (pos == wxString::npos) continue;
		wxString key = curLine.Left(pos);
		wxString value = curLine.Mid(pos+1);

		// Find it
		cur = opt.find(key);
		if (cur != opt.end()) {
			(*cur).second.ResetWith(value);
		}
		else SetText(key,value);
	}

	lastVersion = OPT_GET("Version/Last Version")->GetInt();
}



/// @brief Write int 
/// @param key           
/// @param param         
/// @param ifLastVersion 
/// @return 
///
void OptionsManager::SetInt(wxString key,int param,int ifLastVersion) {
	if (ifLastVersion == -1) {
		if (overriding) ifLastVersion = 0;
		else ifLastVersion = 0x7FFFFFFF;
	}
	if (lastVersion >= ifLastVersion) return;
	opt[key.Lower()].SetInt(param);
	if (curModType != MOD_OFF) optType[key.Lower()] = curModType;
	modified = true;
}



/// @brief Write float 
/// @param key           
/// @param param         
/// @param ifLastVersion 
/// @return 
///
void OptionsManager::SetFloat(wxString key,double param,int ifLastVersion) {
	if (ifLastVersion == -1) {
		if (overriding) ifLastVersion = 0;
		else ifLastVersion = 0x7FFFFFFF;
	}
	if (lastVersion >= ifLastVersion) return;
	opt[key.Lower()].SetFloat(param);
	if (curModType != MOD_OFF) optType[key.Lower()] = curModType;
	modified = true;
}



/// @brief Write string 
/// @param key           
/// @param param         
/// @param ifLastVersion 
/// @return 
///
void OptionsManager::SetText(wxString key,wxString param,int ifLastVersion) {
	if (ifLastVersion == -1) {
		if (overriding) ifLastVersion = 0;
		else ifLastVersion = 0x7FFFFFFF;
	}
	if (lastVersion >= ifLastVersion) return;
	opt[key.Lower()].SetText(param);
	if (curModType != MOD_OFF) optType[key.Lower()] = curModType;
	modified = true;
}



/// @brief Write boolean 
/// @param key           
/// @param param         
/// @param ifLastVersion 
/// @return 
///
void OptionsManager::SetBool(wxString key,bool param,int ifLastVersion) {
	if (ifLastVersion == -1) {
		if (overriding) ifLastVersion = 0;
		else ifLastVersion = 0x7FFFFFFF;
	}
	if (lastVersion >= ifLastVersion) return;
	opt[key.Lower()].SetBool(param);
	if (curModType != MOD_OFF) optType[key.Lower()] = curModType;
	modified = true;
}



/// @brief Write colour 
/// @param key           
/// @param param         
/// @param ifLastVersion 
/// @return 
///
void OptionsManager::SetColour(wxString key,wxColour param,int ifLastVersion) {
	if (ifLastVersion == -1) {
		if (overriding) ifLastVersion = 0;
		else ifLastVersion = 0x7FFFFFFF;
	}
	if (lastVersion >= ifLastVersion) return;
	opt[key.Lower()].SetColour(param);
	if (curModType != MOD_OFF) optType[key.Lower()] = curModType;
	modified = true;
}



/// @brief Reset with 
/// @param key   
/// @param param 
///
void OptionsManager::ResetWith(wxString key,wxString param) {
	opt[key.Lower()].ResetWith(param);
	modified = true;
}



/// @brief As float 
/// @param key 
/// @return 
///
double OptionsManager::AsFloat(wxString key) {
	std::map<wxString,VariableData>::iterator cur;
	cur = (opt.find(key.Lower()));
	if (cur != opt.end()) {
		return (*cur).second.AsFloat();
	}
	else throw key.c_str();//_T("Internal error: Attempted getting undefined configuration setting");
}



/// @brief Modification type 
/// @param key 
/// @return 
///
ModType OptionsManager::GetModType(wxString key) {
	std::map<wxString,ModType>::iterator cur;
	cur = (optType.find(key.Lower()));
	if (cur != optType.end()) {
		return (*cur).second;
	}
	else return MOD_AUTOMATIC;
}



/// @brief Is defined? 
/// @param key 
/// @return 
///
bool OptionsManager::IsDefined(wxString key) {
	std::map<wxString,VariableData>::iterator cur;
	cur = (opt.find(key.Lower()));
	return (cur != opt.end());
}


/// @brief Get recent list 
/// @param list 
/// @return 
///
wxArrayString OptionsManager::GetRecentList (wxString list) {
	wxArrayString work;

	agi::MRUManager::MRUListMap *map_list = AegisubApp::Get()->mru->Get(STD_STR(list));

	for (agi::MRUManager::MRUListMap::const_iterator i_lst = map_list->begin(); i_lst != map_list->end(); ++i_lst) {
		work.Add(wxString(i_lst->second));
	}

	return work;
}



/// @brief Set modification type 
/// @param type 
///
void OptionsManager::SetModificationType(ModType type) {
	curModType = type;
}



/// DOCME
OptionsManager Options;


