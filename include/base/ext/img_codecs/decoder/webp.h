#pragma once

#ifdef HAVE_LIB_WEBP

#include <base/ext/img_codecs/common.h>
#include <cstdint>

namespace Base
{
	class IMAGE_CODECS_INTERFACE WebPDecoder
	{
	public:
		void load(const void* pointer, uint64_t size);
		int getWidth();
		int getHeight();
		int getDecompressedSize();
		void decode(void* buffer);
	private:
		int _width, _height;
		const void* _pointer;
		uint64_t _size;
	};
}
#endif