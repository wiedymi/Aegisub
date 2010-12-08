// Copyright (c) 2005-2010, Niels Martin Hansen
// Copyright (c) 2005-2010, Rodrigo Braz Monteiro
// Copyright (c) 2010, Amar Takhar
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

/// @file help.cpp
/// @brief help commands
/// @ingroup command
///

#include "config.h"

#ifndef AGI_PRE
#include <wx/window.h>
#endif

#include "aegisub/context.h"

#include "help_button.h" // help_contents
#include "main.h"

namespace cmd {

void help_bugs(agi::Context *c) {
	if (wxGetMouseState().CmdDown()) {
		if (wxGetMouseState().ShiftDown()) {
			 wxMessageBox(_T("Now crashing with an access violation..."));
			for (char *foo = (char*)0;;) *foo++ = 42;
		} else {
			wxMessageBox(_T("Now crashing with an unhandled exception..."));
			throw c->parent;
		}
	}

	AegisubApp::OpenURL(_T("http://devel.aegisub.org/"));
}


void help_contents(agi::Context *c) {
	HelpButton::OpenPage(_T("Main"));
}


void help_files(agi::Context *c) {
#ifdef __WXMAC__
	char *shared_path = agi::util::OSX_GetBundleSharedSupportDirectory();
	wxString help_path = wxString::Format(_T("%s/doc"), wxString(shared_path, wxConvUTF8).c_str());
	agi::util::OSX_OpenLocation(help_path.c_str());
	free(shared_path);
#endif
}


void help_forums(agi::Context *c) {
	AegisubApp::OpenURL(_T("http://forum.aegisub.org/"));
}


void help_irc(agi::Context *c) {
	AegisubApp::OpenURL(_T("irc://irc.rizon.net/aegisub"));
}


void help_website(agi::Context *c) {
	AegisubApp::OpenURL(_T("http://www.aegisub.org/"));
}


} // namespace cmd
