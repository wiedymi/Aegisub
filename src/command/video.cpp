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

/// @file video.cpp
/// @brief video/ commands.
/// @ingroup command
///

#include "config.h"

#ifndef AGI_PRE
#endif

#include "aegisub/context.h"
#include "video_context.h"
#include "main.h"
#include "utils.h"
#include "frame_main.h"
#include "video_display.h"
#include "dialog_detached_video.h"
#include "dialog_video_details.h"
#include "video_slider.h"
#include "dialog_dummy_video.h"
#include "compat.h"
#include "dialog_jumpto.h"

namespace cmd {

void video_aspect_cinematic(agi::Context *c) {
    VideoContext::Get()->Stop();
    VideoContext::Get()->SetAspectRatio(3);
    wxGetApp().frame->SetDisplayMode(1,-1);
}


void video_aspect_custom(agi::Context *c) {
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
                if (scale) c->videoBox->videoDisplay->SetZoom(b / VideoContext::Get()->GetHeight());
            }
        }
        else numval = 0.0;
    }

    // Sanity check
    if (numval < 0.5 || numval > 5.0) wxMessageBox(_("Invalid value! Aspect ratio must be between 0.5 and 5.0."),_("Invalid Aspect Ratio"),wxICON_ERROR|wxOK);

    // Set value
    else {
        VideoContext::Get()->SetAspectRatio(4,numval);
        wxGetApp().frame->SetDisplayMode(1,-1);
    }
}


void video_aspect_default(agi::Context *c) {
    VideoContext::Get()->Stop();
    VideoContext::Get()->SetAspectRatio(0);
    wxGetApp().frame->SetDisplayMode(1,-1);
}


void video_aspect_full(agi::Context *c) {
    VideoContext::Get()->Stop();
    VideoContext::Get()->SetAspectRatio(1);
    wxGetApp().frame->SetDisplayMode(1,-1);
}


void video_aspect_wide(agi::Context *c) {
    VideoContext::Get()->Stop();
    VideoContext::Get()->SetAspectRatio(2);
    wxGetApp().frame->SetDisplayMode(1,-1);
}


void video_close(agi::Context *c) {
	wxGetApp().frame->LoadVideo(_T(""));
}


void video_detach(agi::Context *c) {
	wxGetApp().frame->DetachVideo(!c->detachedVideo);
}


void video_details(agi::Context *c) {
    VideoContext::Get()->Stop();
    DialogVideoDetails videodetails(c->parent);
    videodetails.ShowModal();
}


void video_focus_seek(agi::Context *c) {
    wxWindow *curFocus = wxWindow::FindFocus();
    if (curFocus == c->videoBox->videoSlider) {
        if (c->PreviousFocus) c->PreviousFocus->SetFocus();
    }
    else {
        c->PreviousFocus = curFocus;
        c->videoBox->videoSlider->SetFocus();
    }
}


void video_frame_next(agi::Context *c) {
    c->videoBox->videoSlider->NextFrame();
}


void video_frame_play(agi::Context *c) {
    VideoContext::Get()->Play();
}


void video_frame_prev(agi::Context *c) {
    c->videoBox->videoSlider->PrevFrame();
}


void video_jump(agi::Context *c) {
    VideoContext::Get()->Stop();
    if (VideoContext::Get()->IsLoaded()) {
        DialogJumpTo JumpTo(c->parent);
        JumpTo.ShowModal();
        c->videoBox->videoSlider->SetFocus();
    }
}


void video_jump_end(agi::Context *c) {
    c->SubsGrid->SetVideoToSubs(false);
}


void video_jump_start(agi::Context *c) {
    c->SubsGrid->SetVideoToSubs(true);
}


void video_open(agi::Context *c) {
    wxString path = lagi_wxString(OPT_GET("Path/Last/Video")->GetString());
    wxString str = wxString(_("Video Formats")) + _T(" (*.avi,*.mkv,*.mp4,*.avs,*.d2v,*.ogm,*.mpeg,*.mpg,*.vob,*.mov)|*.avi;*.avs;*.d2v;*.mkv;*.ogm;*.mp4;*.mpeg;*.mpg;*.vob;*.mov|")
                 + _("All Files") + _T(" (*.*)|*.*");
    wxString filename = wxFileSelector(_("Open video file"),path,_T(""),_T(""),str,wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (!filename.empty()) {
        wxGetApp().frame->LoadVideo(filename);
        OPT_SET("Path/Last/Video")->SetString(STD_STR(filename));
    }
}


void video_open_dummy(agi::Context *c) {
    wxString fn;
    if (DialogDummyVideo::CreateDummyVideo(c->parent, fn)) {
        wxGetApp().frame->LoadVideo(fn);
    }
}


void video_show_overscan(agi::Context *c) {
//XXX: Fix to not require using an event. (maybe)
//    OPT_SET("Video/Overscan Mask")->SetBool(event.IsChecked());
    VideoContext::Get()->Stop();
    c->videoBox->videoDisplay->Render();
}


void video_zoom_100(agi::Context *c) {
    VideoContext::Get()->Stop();
    c->videoBox->videoDisplay->SetZoom(1.);

}


void video_zoom_200(agi::Context *c) {
    VideoContext::Get()->Stop();
    c->videoBox->videoDisplay->SetZoom(2.);

}


void video_zoom_50(agi::Context *c) {
    VideoContext::Get()->Stop();
    c->videoBox->videoDisplay->SetZoom(.5);

}


void video_zoom_in(agi::Context *c) {
    VideoContext::Get()->Stop();
    c->videoBox->videoDisplay->SetZoom(c->videoBox->videoDisplay->GetZoom() + .125);

}


void video_zoom_out(agi::Context *c) {
    VideoContext::Get()->Stop();
    c->videoBox->videoDisplay->SetZoom(c->videoBox->videoDisplay->GetZoom() - .125);

}


} // namespace cmd
