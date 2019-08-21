#pragma once

#include <stddef.h>

#include <base/ext/img_codecs/decoder/jpeg.h>
#include <base/ext/img_codecs/decoder/png.h>
#include <base/ext/img_codecs/decoder/webp.h>
#include <base/ext/img_codecs/types.h>

namespace Base
{
    class ATTRIBUTE_INTERFACE ImageDecoder
    {
    public:
        void load(const void *buffer, size_t size, ImageFormatType formatType);
        int getHeight();
        int getWidth();
        int getDecompressedSize();
        void decode(void *output);

    private:
        JPEGDecoder _jpegDecoder;
        PNGDecoder _pngDecoder;
        WebPDecoder _webpDecoder;

        ImageFormatType _format;
    };
}