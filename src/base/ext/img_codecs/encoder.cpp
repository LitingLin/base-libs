#include <base/ext/img_codecs/encoder.h>

namespace Base
{
	EncodedImageContainer::EncodedImageContainer(void* ptr, size_t size)
		: _ptr(ptr), _size(size)
	{
	}

	size_t EncodedImageContainer::size()
	{
		return _size;
	}

	void* EncodedImageContainer::get()
	{
		return _ptr;
	}
}