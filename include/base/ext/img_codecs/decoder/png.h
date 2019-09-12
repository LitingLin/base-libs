#pragma once

#ifdef HAVE_LIB_PNG

#include <base/ext/img_codecs/common.h>
#include <png.h>
#include <vector>
#include <cstdint>

namespace Base
{	
	class IMAGE_CODECS_INTERFACE PNGDecoder
	{
	public:
		PNGDecoder();
		PNGDecoder(const PNGDecoder&) = delete;
		PNGDecoder(PNGDecoder&& object) noexcept;
		~PNGDecoder();
		void load(const void* image, uint64_t size);
		[[nodiscard]] unsigned getWidth() const;
		[[nodiscard]] unsigned getHeight() const;
		[[nodiscard]] uint64_t getDecompressedSize();
		void decode(void* buffer);
	private:
		unsigned char* _sourceImage;
		unsigned char* _currentImagePosition;
		png_structp _png_ptr;
		png_infop _info_ptr;
		unsigned long _image_width, _image_height;
		::std::vector<png_bytep> _row_pointers;
	};
}
#endif
