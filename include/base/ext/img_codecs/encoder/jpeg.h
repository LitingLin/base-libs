#pragma once

#ifdef HAVE_LIB_JPEG

#include <base/ext/img_codecs/common.h>
#include <base/ext/img_codecs/encoder.h>

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

namespace Base {
	class IMAGE_CODECS_INTERFACE JPEGEncoder
	{
	public:
		class IMAGE_CODECS_INTERFACE JPEGImageContainer : public EncodedImageContainer
		{
		public:
			JPEGImageContainer(void* ptr, size_t size);
			JPEGImageContainer(const JPEGImageContainer&) = delete;
			JPEGImageContainer(JPEGImageContainer&&) = delete;
			virtual ~JPEGImageContainer();
		};
		JPEGEncoder();
		~JPEGEncoder();
		[[nodiscard]] JPEGImageContainer encode(const void* rgb, unsigned width, unsigned height);
	private:
		jpeg_compress_struct _jpegCompress;
		struct jpeg_error_mgr jerr;
	};
}

#endif