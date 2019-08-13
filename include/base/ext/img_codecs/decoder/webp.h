#pragma once

#ifdef HAVE_LIB_WEBP
#include <cstdint>

namespace Base
{
	class WebPDecoder
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