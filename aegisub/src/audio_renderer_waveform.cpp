// Copyright (c) 2010, Niels Martin Hansen
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

/// @file audio_renderer_waveform.cpp
/// @ingroup audio_ui
///
/// Render a waveform display of PCM audio data

#include "config.h"

#ifndef AGI_PRE
#include <algorithm>

#include <wx/dcmemory.h>
#endif

#include "block_cache.h"
#include "include/aegisub/audio_provider.h"
#include "audio_colorscheme.h"
#include "audio_renderer.h"
#include "audio_renderer_waveform.h"
#include "colorspace.h"



AudioWaveformRenderer::AudioWaveformRenderer()
: AudioRendererBitmapProvider()
, colors_normal(6)
, colors_selected(6)
{
	colors_normal.InitIcyBlue_Normal();
	colors_selected.InitIcyBlue_Selected();
}


AudioWaveformRenderer::~AudioWaveformRenderer()
{
}


void AudioWaveformRenderer::Render(wxBitmap &bmp, int start, bool selected)
{
	wxMemoryDC dc(bmp);
	wxRect rect(wxPoint(0, 0), bmp.GetSize());
	int midpoint = rect.height / 2;

	AudioColorScheme *pal = selected ? &colors_selected : &colors_normal;

	// Fill the background
	dc.SetBrush(wxBrush(pal->get(0.0f)));
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.DrawRectangle(rect);

	/// @todo Store these buffers in the instance to avoid constant re-allocations
	int *peak_min = new int[rect.width];
	int *peak_max = new int[rect.width];

	/// @todo Move this out of AudioProvider and extend it to get both average and extreme values
	provider->GetWaveForm(peak_min, peak_max, start*pixel_samples, rect.width, rect.height, pixel_samples, amplitude_scale);

	dc.SetPen(wxPen(pal->get(0.8f)));
	for (int x = 0; x < rect.width; ++x)
	{
		dc.DrawLine(x, peak_max[x], x, peak_min[x]-1);
	}

	dc.SetPen(wxPen(pal->get(1.0f)));
	dc.DrawLine(0, midpoint, rect.width, midpoint);

	delete[] peak_min;
	delete[] peak_max;
}


void AudioWaveformRenderer::RenderBlank(wxDC &dc, const wxRect &rect, bool selected)
{
	AudioColorScheme *pal = selected ? &colors_selected : &colors_normal;

	wxColor line(pal->get(1.0));
	wxColor bg(pal->get(0.0));

	// Draw the line as background above and below, and line in the middle, to avoid
	// overdraw flicker (the common theme in all of audio display direct drawing).
	int halfheight = (rect.height - rect.y) / 2;

	dc.SetBrush(wxBrush(bg));
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.DrawRectangle(rect.x, rect.y, rect.width, halfheight);
	dc.DrawRectangle(rect.x, halfheight + 1, rect.width, halfheight-1);

	dc.SetPen(wxPen(line));
	dc.DrawLine(rect.x, rect.y+halfheight, rect.x+rect.width, rect.y+halfheight);
}


