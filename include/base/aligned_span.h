#pragma once

namespace Base
{
	class AlignedSpan
	{
	public:
		AlignedSpan(size_t size, unsigned alignment);
		AlignedSpan(const AlignedSpan&) = delete;
		AlignedSpan(AlignedSpan&&) = delete;
		~AlignedSpan();
		void* get();
		void resize(size_t size, unsigned alignment);
	private:
		void* _ptr;
	};
}