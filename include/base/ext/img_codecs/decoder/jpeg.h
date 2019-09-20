#pragma once

#include <base/ext/img_codecs/common.h>

#ifdef HAVE_LIB_JPEG_TURBO
#include <turbojpeg.h>
#include <cstdint>
namespace Base {
    class IMAGE_CODECS_INTERFACE JPEGDecoder {
    public:
        JPEGDecoder();
        JPEGDecoder(const JPEGDecoder &) = delete;
        JPEGDecoder(JPEGDecoder &&object) noexcept;
        ~JPEGDecoder();
        void load(const void *pointer, uint64_t size);
        int getWidth();
        int getHeight();
        int getDecompressedSize();
        void decode(void *buffer);
    private:
        const unsigned char *_pointer;
        unsigned long _fileSize;
        tjhandle _handle;
        int _width, _height;
        int _jpegSubsamp, _jpegColorspace;
    };
}
#elif defined HAVE_LIB_JPEG
#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include <stdio.h>
#ifdef _MSC_VER
#pragma warning(push, 0)
#elif defined __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#endif
#include <jpeglib.h>
#ifdef _MSC_VER
#pragma warning(pop)
#elif defined __GNUC__
#pragma GCC diagnostic pop
#endif
#include <cstdint>

namespace Base
{	
	class IMAGE_CODECS_INTERFACE JPEGDecoder
	{
	public:
		JPEGDecoder();
		JPEGDecoder(const JPEGDecoder&) = delete;
		JPEGDecoder(JPEGDecoder&& object) noexcept;
		~JPEGDecoder();
		void load(const void* pointer, uint64_t size);
		[[nodiscard]] unsigned getWidth();
		[[nodiscard]] unsigned getHeight();
		[[nodiscard]] uint64_t getDecompressedSize();
		void decode(void* buffer);
	private:
		const unsigned char* _pointer;
		unsigned long _fileSize;
		
		jpeg_decompress_struct decInfo;
		struct jpeg_error_mgr jerr;
	};
}
#endif

#ifdef HAVE_INTEL_MEDIA_SDK
#include <mfxvideo.h> /* SDK functions in C */
#include <mfxjpeg.h>

namespace Base{
	
class IMAGE_CODECS_INTERFACE IntelGraphicsJpegDecoder
{
public:
	void decode();
private:

};
}
#endif