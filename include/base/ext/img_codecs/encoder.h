#pragma once

#include <cstddef>

#include <base/ext/img_codecs/common.h>

namespace Base
{
	class IMAGE_CODECS_INTERFACE EncodedImageContainer
	{
	public:
		EncodedImageContainer(void* ptr, size_t size);
		virtual ~EncodedImageContainer() = default;
		[[nodiscard]] size_t size();
		[[nodiscard]] void* get();
	protected:
		void* _ptr;
		size_t _size;
	};
}