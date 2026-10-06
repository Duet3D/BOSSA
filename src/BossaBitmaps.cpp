///////////////////////////////////////////////////////////////////////////////
// BOSSA
//
// Copyright (c) 2011-2018, ShumaTech
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the <organization> nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////
#include "BossaBitmaps.h"

#include <wx/mstream.h>

#include "BossaLogo.cpp"
#include "BossaIcon.cpp"
#include "ShumaTechLogo.cpp"

BossaBitmaps::BossaBitmaps()
{
}

void
BossaBitmaps::init()
{
    _bossaLogo = GetBitmapFromMemory(BossaLogo_bmp, sizeof(BossaLogo_bmp));
    _bossaIcon = GetBitmapFromMemory(BossaIcon_bmp, sizeof(BossaIcon_bmp));
    _shumaTechLogo = GetBitmapFromMemory(ShumaTechLogo_bmp, sizeof(ShumaTechLogo_bmp));
}

wxBitmap
BossaBitmaps::GetBitmapFromMemory(const unsigned char *data, int length)
{
    wxMemoryInputStream is(data, length);
    wxImage image(is, wxBITMAP_TYPE_ANY, -1);

    // Newer wxWidgets versions ignore the alpha channel of a 32-bit BMP with a
    // plain BITMAPINFOHEADER, which shows the transparent parts of the logos
    // as black, so apply the alpha channel here
    if (!image.HasAlpha() && length > 54 && data[0] == 'B' && data[1] == 'M' &&
        readLE(data + 28, 2) == 32 && readLE(data + 30, 4) == 0)
    {
        uint32_t offset = readLE(data + 10, 4);
        int32_t width = readLE(data + 18, 4);
        int32_t height = readLE(data + 22, 4);
        bool bottomUp = height > 0;
        if (!bottomUp)
            height = -height;

        if (width == image.GetWidth() && height == image.GetHeight() &&
            offset + (uint64_t) width * height * 4 <= (uint64_t) length)
        {
            image.SetAlpha();
            for (int y = 0; y < height; y++)
            {
                const unsigned char* row = data + offset + (bottomUp ? height - 1 - y : y) * width * 4;
                for (int x = 0; x < width; x++)
                    image.SetAlpha(x, y, row[x * 4 + 3]);
            }
        }
    }

    return wxBitmap(image, -1);
}

uint32_t
BossaBitmaps::readLE(const unsigned char* data, int size)
{
    uint32_t value = 0;
    for (int i = size - 1; i >= 0; i--)
        value = (value << 8) | data[i];
    return value;
}


