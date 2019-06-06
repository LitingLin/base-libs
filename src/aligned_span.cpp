#include <base/aligned_span.h>

#include <base/logging.h>

#ifdef _WIN32
#include <malloc.h>
#else
#include <stdlib.h>
#endif

namespace Base
{
	AlignedSpan::AlignedSpan(size_t size, unsigned alignment)
	{
#ifdef _WIN32
		_ptr = _aligned_malloc(size, alignment);
		CHECK_STDCAPI(_ptr);
#else
		_ptr = aligned_alloc(alignment, size);
		CHECK_STDCAPI(_ptr);
#endif
	}
	AlignedSpan::~AlignedSpan()
	{
#ifdef _WIN32
		_aligned_free(_ptr);
#else
		free(_ptr);
#endif
	}
	void* AlignedSpan::get()
	{
		return _ptr;
	}
	void AlignedSpan::resize(size_t size, unsigned alignment)
	{
		void* ptr;
#ifdef _WIN32
		ptr = _aligned_malloc(size, alignment);
		CHECK_STDCAPI(ptr);
#else
		ptr = aligned_alloc(alignment, size);
		CHECK_STDCAPI(ptr);
#endif
		this->~AlignedSpan();
		_ptr = ptr;
	}
}