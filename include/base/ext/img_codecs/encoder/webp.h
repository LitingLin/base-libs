#pragma once

#ifdef HAVE_LIB_WEBP

#include <base/ext/img_codecs/common.h>
#include <cstddef>

namespace Base
{
	class IMAGE_CODECS_INTERFACE WebPEncoder
	{
	public:		
		class IMAGE_CODECS_INTERFACE Container
		{
		public:
			Container(void* ptr, size_t size);
			Container(const Container&) = delete;
			Container(Container&&) = delete;
			~Container();
			void* get();
			[[nodiscard]] size_t size();
		private:
			void* _ptr;
			size_t _size;
		};
		[[nodiscard]] Container encode(const void* rgb, unsigned width, unsigned height);
	};
}

#endif