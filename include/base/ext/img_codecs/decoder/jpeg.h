#pragma once

#ifdef HAVE_LIB_JPEG
#include <cstdio>
#include <jpeglib.h>
#include <cstdint>

namespace Base
{
	class JPEGDecoder
	{
	public:
		JPEGDecoder();
		JPEGDecoder(const JPEGDecoder&) = delete;
		JPEGDecoder(JPEGDecoder&& object) = delete;
		~JPEGDecoder();
		void load(const void* pointer, uint64_t size);
		int getWidth();
		int getHeight();
		int getDecompressedSize();
		void decode(void* buffer);
	private:
		const unsigned char* _pointer;
		unsigned long _fileSize;
		
		jpeg_decompress_struct decInfo;
		struct jpeg_error_mgr jerr;
	};
}
#endif