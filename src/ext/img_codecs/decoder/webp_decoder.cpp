#include <base/ext/img_codecs/decoder/webp.h>

#include <base/logging.h>
#include <webp/decode.h>

namespace Base
{
	void WebPDecoder::load(const void* pointer, uint64_t size)
	{
		CHECK(WebPGetInfo((const uint8_t*)pointer, size, &_width, &_height));
		_pointer = pointer;
	}

	int WebPDecoder::getWidth()
	{
		return _width;
	}

	int WebPDecoder::getHeight()
	{
		return _height;
	}

	int WebPDecoder::getDecompressedSize()
	{
		return _width * _height * 3;
	}

	void WebPDecoder::decode(void* buffer)
	{
		CHECK_EQ(WebPDecodeRGBInto((const uint8_t*)_pointer, _size, (uint8_t*)buffer, _width * _height * 3, _width * 3), (uint8_t*)buffer);
	}
}
