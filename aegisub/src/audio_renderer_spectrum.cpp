// Copyright (c) 2005, 2006, Rodrigo Braz Monteiro
// Copyright (c) 2006, 2007, 2009, Niels Martin Hansen
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

/// @file audio_renderer_spectrum.cpp
/// @brief Caching frequency-power spectrum renderer for audio display
/// @ingroup audio_ui

#include "config.h"

#ifndef AGI_PRE
#include <wx/rawbmp.h>
#include <algorithm>
#endif

#include "block_cache.h"
#include "include/aegisub/audio_provider.h"
#include "audio_renderer.h"
#include "audio_renderer_spectrum.h"
#include "fft.h"
#include "colorspace.h"


// Something is defining "min" and "max" macros, and they interfere with using std::min and std::max
#undef min
#undef max


void AudioSpectrumColorMap::InitIcyBlue_Normal()
{
	unsigned char *palptr = palette;
	for (size_t i = 0; i < factor; ++i)
	{
		float t = (float)i / (factor-1);
		int H = (int)(255 * (1.5 - t) / 2);
		int S = (int)(255 * (0.5 + t/2));
		int L = std::min(255, (int)(255 * 2 * i));
		hsl_to_rgb(H, S, L, palptr + 0, palptr + 1, palptr + 2);
		palptr += 4;
	}
}


void AudioSpectrumColorMap::InitIcyBlue_Selected()
{
	unsigned char *palptr = palette;
	for (size_t i = 0; i < factor; ++i)
	{
		float t = (float)i / (factor-1);
		int H = (int)(255 * (1.5 - t) / 2);
		int S = (int)(255 * (0.5 + t/2));
		int L = std::min(255, (int)(255 * (3 * i/2 + 0.25)));
		hsl_to_rgb(H, S, L, palptr + 0, palptr + 1, palptr + 2);
		palptr += 4;
	}
}



/// Allocates blocks of derived data for the audio spectrum
struct AudioSpectrumCacheBlockFactory {
	/// Pointer back to the owning spectrum renderer
	AudioSpectrumRenderer *spectrum;

	/// @brief Constructor
	/// @param s The owning spectrum renderer
	AudioSpectrumCacheBlockFactory(AudioSpectrumRenderer *s) : spectrum(s) { }

	/// @brief Allocate and fill a data block
	/// @param i Index of the block to produce data for
	/// @return Newly allocated and filled block
	///
	/// The filling is delegated to the spectrum renderer
	float *ProduceBlock(size_t i)
	{
		float *res = new float[1<<spectrum->derivation_size];
		spectrum->FillBlock(i, res);
		return res;
	}

	/// @brief De-allocate a cache block
	/// @param block The block to dispose of
	void DisposeBlock(float *block)
	{
		delete[] block;
	}

	/// @brief Calculate the in-memory size of a spec
	/// @return The size in bytes of a spectrum cache block
	size_t GetBlockSize() const
	{
		return sizeof(float) << spectrum->derivation_size;
	}
};


/// @brief Cache for audio spectrum frequency-power data
class AudioSpectrumCache
	: public DataBlockCache<float, 10, AudioSpectrumCacheBlockFactory> {
public:
	AudioSpectrumCache(size_t block_count, AudioSpectrumRenderer *renderer)
		: DataBlockCache(block_count, AudioSpectrumCacheBlockFactory(renderer))
	{
	}
};




AudioSpectrumRenderer::AudioSpectrumRenderer()
: AudioRendererBitmapProvider()
, cache(0)
, colors_normal(12)
, colors_selected(12)
, derivation_size(8)
, derivation_dist(8)
, fft_scratch(0)
, audio_scratch(0)
{
	colors_normal.InitIcyBlue_Normal();
	colors_selected.InitIcyBlue_Selected();
}


AudioSpectrumRenderer::~AudioSpectrumRenderer()
{
	delete cache;
	delete[] fft_scratch;
	delete[] audio_scratch;
}


void AudioSpectrumRenderer::RecreateCache()
{
	delete cache;
	delete[] fft_scratch;
	delete[] audio_scratch;
	cache = 0;
	fft_scratch = 0;
	audio_scratch = 0;

	if (provider)
	{
		size_t block_count = (size_t)((provider->GetNumSamples() + (size_t)(1<<derivation_dist) - 1) >> derivation_dist);
		cache = new AudioSpectrumCache(block_count, this);

		// Allocate scratch for 6x the derivation size:
		// 2x for the input sample data
		// 2x for the real part of the output
		// 2x for the imaginary part of the output
		fft_scratch = new float[6<<derivation_size];
		audio_scratch = new int16_t[2<<derivation_size];
	}
}


void AudioSpectrumRenderer::OnSetProvider()
{
	RecreateCache();
}


void AudioSpectrumRenderer::SetResolution(size_t _derivation_size, size_t _derivation_dist)
{
	if (derivation_dist != _derivation_dist)
	{
		derivation_dist = _derivation_dist;
		if (cache)
			cache->Age(0);
	}

	if (derivation_size != _derivation_size)
	{
		derivation_size = _derivation_size;
		RecreateCache();
	}
}


void AudioSpectrumRenderer::FillBlock(size_t block_index, float *block)
{
	assert(cache);
	assert(block);

	int64_t first_sample = ((int64_t)block_index) << derivation_dist;
	provider->GetAudio(audio_scratch, first_sample, 2 << derivation_size);

	float *fft_input = fft_scratch;
	float *fft_real = fft_scratch + (2 << derivation_size);
	float *fft_imag = fft_scratch + (4 << derivation_size);

	// Convert audio data to float range [-1;+1)
	for (size_t si = 2<<derivation_size; si > 0; --si)
	{
		*fft_input++ = (float)(*audio_scratch++) / 32768.f;
	}
	fft_input = fft_scratch;

	FFT fft;
	fft.Transform(2<<derivation_size, fft_input, fft_real, fft_imag);

	for (size_t si = 1<<derivation_size; si > 0; --si)
	{
		// with x in range [0;1], log10(x*9+1) will also be in range [0;1]
		*block++ = log10( sqrt(*fft_real * *fft_real + *fft_imag * *fft_imag) * 9 / sqrt(2.) + 1);
		fft_real++; fft_imag++;
	}
}


void AudioSpectrumRenderer::Render(wxBitmap &bmp, int start, bool selected)
{
	if (!cache)
		return;

	assert(bmp.IsOk());
	assert(bmp.GetDepth() == 24);

	int end = start + bmp.GetWidth();

	assert(start >= 0);
	assert(end >= 0);
	assert(end >= start);

	wxNativePixelData pixels(bmp);
	int imgheight = bmp.GetHeight();

	struct {
		uint8_t r, g, b, a;
	} color;
	AudioSpectrumColorMap *pal = selected ? &colors_selected : &colors_normal;

	/// @todo Make minband and maxband configurable
	int minband = 0;
	int maxband = 1 << derivation_size;

	// ax = absolute x, absolute to the virtual spectrum bitmap
	for (int ax = start; ax < end; ++ax)
	{
		// Derived audio data
		size_t block_index = (size_t)(ax * pixel_samples) >> derivation_dist;
		float *power = cache->Get(block_index);

		// Prepare bitmap writing
		wxNativePixelData::Iterator p = pixels.GetPixels();
		p.MoveTo(pixels, ax - start, 0);

		// Scale up or down vertically?
		if (imgheight > 1<<derivation_size)
		{
			// Interpolate
			for (int y = 0; y < imgheight; ++y)
			{
				float ideal = (float)(y+1.)/imgheight * (maxband-minband) + minband;
				float sample1 = power[(int)floor(ideal)+minband];
				float sample2 = power[(int)ceil(ideal)+minband];
				float frac = ideal - floor(ideal);
				float val = (1-frac)*sample1 + frac*sample2;
				pal->map(val, &color.r);
				p.Red() = color.r; p.Green() = color.g; p.Blue() = color.b;
				p.OffsetY(pixels, 1);
			}
		}
		else
		{
			// Pick greatest
			for (int y = 0; y < imgheight; ++y)
			{
				int sample1 = std::max(0, maxband * y/imgheight + minband);
				int sample2 = std::min((1<<derivation_size)-1, maxband * (y+1)/imgheight + minband);
				float maxval = 0;
				for (int samp = sample1; samp <= sample2; samp++)
					if (power[samp] > maxval) maxval = power[samp];
				pal->map(maxval, &color.r);
				p.Red() = color.r; p.Green() = color.g; p.Blue() = color.b;
				p.OffsetY(pixels, 1);
			}
		}
	}
}


void AudioSpectrumRenderer::AgeCache(size_t max_size)
{
	if (cache)
		cache->Age(max_size);
}

