#include <base/memory_alignment.h>

#include <base/logging.h>

#ifdef _WIN32
#include <malloc.h>
#else
#include <stdlib.h>
#endif

namespace Base
{
	void *alignedMemoryAllocation(size_t size, unsigned alignment)
	{
		void* ptr;
#ifdef _WIN32
		ptr = _aligned_malloc(size, alignment);
		CHECK_STDCAPI(ptr);
#else
		ptr = aligned_alloc(alignment, size);
		CHECK_STDCAPI(ptr);
#endif
		return ptr;
	}

	void alignedMemoryFree(void *ptr)
	{
#ifdef _WIN32
		_aligned_free(ptr);
#else
		free(ptr);
#endif
	}
	
	AlignedMemorySpace::AlignedMemorySpace(size_t size, unsigned alignment)
	{
		_ptr = alignedMemoryAllocation(size, alignment);
	}
	
	AlignedMemorySpace::~AlignedMemorySpace()
	{
		alignedMemoryFree(_ptr);
	}

	void* AlignedMemorySpace::get()
	{
		return _ptr;
	}

	AlignedDynamicRawArray::AlignedDynamicRawArray()
		: _ptr(nullptr), _size(0), _alignment(0)
	{
	}

	AlignedDynamicRawArray::AlignedDynamicRawArray(size_t size, unsigned alignment)
		: _size(size), _alignment(alignment)
	{
		_ptr = alignedMemoryAllocation(size, alignment);
	}

	AlignedDynamicRawArray::AlignedDynamicRawArray(const AlignedDynamicRawArray& other)
		: _size(other._size), _alignment(other._alignment)
	{
		if (other._ptr) {
			_ptr = alignedMemoryAllocation(_size, _alignment);
			memcpy(_ptr, other._ptr, _size);
		}
	}

	AlignedDynamicRawArray::AlignedDynamicRawArray(AlignedDynamicRawArray&& other) noexcept
	{
		_ptr = other._ptr;
		_size = other._size;
		_alignment = other._alignment;

		other._ptr = nullptr;
	}

	AlignedDynamicRawArray::~AlignedDynamicRawArray()
	{
		if (_ptr)
			alignedMemoryFree(_ptr);
	}

	void* AlignedDynamicRawArray::get() const
	{
		return _ptr;
	}

	size_t AlignedDynamicRawArray::size() const
	{
		return _size;
	}

	unsigned AlignedDynamicRawArray::alignment() const
	{
		return _alignment;
	}

	void AlignedDynamicRawArray::resize(size_t size, unsigned alignment)
	{
		if (_ptr) {
			alignedMemoryFree(_ptr);
			_ptr = nullptr;
		}
		_size = 0;
		_alignment = 0;

		if (size)
		{
			_ptr = alignedMemoryAllocation(size, alignment);
		}

		_size = size;
		_alignment = alignment;
	}
}
