#pragma once

#ifdef HAVE_LIB_WEBP

#include <base/common.h>
#include <cstddef>

namespace Base
{
	class ATTRIBUTE_INTERFACE WebPEncoder
	{
	public:		
		class ATTRIBUTE_INTERFACE Container
		{
		public:
			Container(void* ptr, size_t size);
			Container(const Container&) = delete;
			Container(Container&&) = delete;
			~Container();
			void* get();
			size_t size();
		private:
			void* _ptr;
			size_t _size;
		};
		Container encode(const void* rgb, int width, int height);
	};
}

#endif