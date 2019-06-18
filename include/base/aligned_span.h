#pragma once

#include <cstddef>

namespace Base
{
	class AlignedSpan
	{
	public:
		AlignedSpan(size_t size, unsigned alignment);
		AlignedSpan(const AlignedSpan&);
		AlignedSpan(AlignedSpan&&) = delete;
		~AlignedSpan();
		void* get();
		size_t size();
		unsigned alignment();
		void resize(size_t size, unsigned alignment);
	private:
		void* _ptr;
		size_t _size;
		unsigned _alignment;
	};
}