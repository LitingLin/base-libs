#pragma once

#ifdef HAVE_LIB_JPEG_TURBO
#include <turbojpeg.h>
#include <cstdint>

namespace Base
{
	class JPEGDecoder
	{
	public:
		JPEGDecoder();
		JPEGDecoder(const JPEGDecoder&) = delete;
		JPEGDecoder(JPEGDecoder&& object) noexcept;
		~JPEGDecoder();
		void load(const void* pointer, uint64_t size);
		int getWidth();
		int getHeight();
		int getDecompressedSize();
		void decode(void* buffer);
	private:
		const unsigned char* _pointer;
		unsigned long _fileSize;
		tjhandle _handle;
		int _width, _height;
		int _jpegSubsamp, _jpegColorspace;
	};
}
#endif