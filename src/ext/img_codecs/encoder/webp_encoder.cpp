#include <base/ext/img_codecs/encoder/webp.h>

#include <base/logging.h>
#include <webp/encode.h>

namespace Base
{
	WebPEncoder::Container::Container(void* ptr, size_t size)
		: _ptr(ptr), _size(size) { }

	WebPEncoder::Container::~Container()
	{
		WebPFree(_ptr);
	}

	void* WebPEncoder::Container::get()
	{
		return _ptr;
	}

	size_t WebPEncoder::Container::size()
	{
		return _size;
	}

	WebPEncoder::Container WebPEncoder::encode(const void* rgb, int width, int height)
	{
		uint8_t* output;
		const size_t size = WebPEncodeRGB((const uint8_t*)rgb, width, height, 3 * width, 80, &output);
		CHECK(size);
		return Container(output, size);
	}
}