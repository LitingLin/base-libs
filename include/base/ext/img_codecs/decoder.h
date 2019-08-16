#pragma once

#include <stddef.h>

#include <base/ext/img_codecs/decoder/jpeg.h>
#include <base/ext/img_codecs/decoder/png.h>
#include <base/ext/img_codecs/decoder/webp.h>
#include <base/ext/img_codecs/types.h>

namespace Base
{
    class ImageDecoder
    {
    public:
        void load(const void *buffer, size_t size, ImageFormatType formatType);
        int getHeight();
        int getWidth();
        int getDecompressedSize();
        void decode(void *output);

    private:
        Base::JPEGDecoder _jpegDecoder;
        Base::PNGDecoder _pngDecoder;
        Base::WebPDecoder _webpDecoder;

        ImageFormatType _format;
    };
}