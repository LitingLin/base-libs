#ifdef HAVE_LIB_WEBP
#include <base/ext/img_codecs/decoder/webp.h>

#include <base/logging.h>
#include <webp/decode.h>

namespace Base
{
	void WebPDecoder::load(const void* pointer, uint64_t size)
	{
		L_CHECK(WebPGetInfo((const uint8_t*)pointer, size, &_width, &_height));
		L_CHECK_GT(_width, 0);
		L_CHECK_GT(_height, 0);
		_pointer = pointer;
		_size = size;
	}

	unsigned WebPDecoder::getWidth() const
	{
		return unsigned(_width);
	}

	unsigned WebPDecoder::getHeight() const
	{
		return unsigned(_height);
	}

	uint64_t WebPDecoder::getDecompressedSize() const
	{
		return uint64_t(_width) * uint64_t(_height) * 3ULL;
	}

	void WebPDecoder::decode(void* buffer)
	{
		L_CHECK_EQ(WebPDecodeRGBInto((const uint8_t*)_pointer, _size, (uint8_t*)buffer, getDecompressedSize(), uint64_t(_width) * 3ULL), (uint8_t*)buffer);
	}
}
#endif