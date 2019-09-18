#pragma once

#include <cstddef>

namespace Base
{
	class EncodedImageContainer
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