#pragma once

#ifdef HAVE_LIB_JPEG

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include <base/ext/img_codecs/common.h>
#include <stdio.h>
#include <jpeglib.h>
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