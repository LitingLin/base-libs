#ifdef HAVE_LIB_WEBP
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

	WebPEncoder::Container WebPEncoder::encode(const void* rgb, unsigned width, unsigned height)
	{
		L_CHECK_LE(width * 3ULL, uint64_t(std::numeric_limits<int>::max()));
		L_CHECK_LE(height, unsigned(std::numeric_limits<int>::max()));
		uint8_t* output;
		const size_t size = WebPEncodeRGB((const uint8_t*)rgb, int(width), int(height), 3 * int(width), 80, &output);
		L_CHECK(size);
		return Container(output, size);
	}
}
#endif