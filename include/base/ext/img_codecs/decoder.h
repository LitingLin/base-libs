#pragma once

#include <stddef.h>

#include <base/ext/img_codecs/decoder/jpeg.h>
#include <base/ext/img_codecs/decoder/png.h>
#include <base/ext/img_codecs/decoder/webp.h>
#include <base/ext/img_codecs/types.h>

namespace Base
{
    class IMAGE_CODECS_INTERFACE ImageDecoder
    {
    public:
        void load(const void *buffer, size_t size, ImageFormatType formatType);
		[[nodiscard]] unsigned getHeight() const;
		[[nodiscard]] unsigned getWidth() const;
		[[nodiscard]] uint64_t getDecompressedSize() const;
        void decode(void *output);
    private:
#if (defined HAVE_LIB_JPEG) || (defined HAVE_LIB_JPEG_TURBO)
        JPEGDecoder _jpegDecoder;
#endif
#if (defined HAVE_LIB_PNG)
        PNGDecoder _pngDecoder;
#endif
#if (defined HAVE_LIB_WEBP)
        WebPDecoder _webpDecoder;
#endif
        ImageFormatType _format;
    };
}