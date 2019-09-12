#include <base/memory_alignment.h>

#include <base/logging.h>
#include <base/cpu_info.h>

#ifdef _WIN32
#include <malloc.h>
#else
#include <stdlib.h>
#endif
#include <cstring>

namespace Base
{
	void *alignedMemoryAllocation(size_t size, unsigned alignment)
	{
		void* ptr;
#ifdef _WIN32
		ptr = _aligned_malloc(size, alignment);
		L_CHECK_STDCAPI(ptr);
#else
		ptr = aligned_alloc(alignment, size);
		L_CHECK_STDCAPI(ptr);
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

	unsigned getSIMDMemoryAlignmentRequirement()
	{
		if (hasAVX512f())
			return 512;
		else if (hasAVX())
			return 256;
		else
			return 128;
	}

	bool isAligned(const void* ptr, unsigned alignment)
	{
		std::ptrdiff_t ptr_ = (std::ptrdiff_t)ptr;
		return ptr_ % alignment == 0;
	}

	bool isAlignedWithSIMDMemoryAlignmentRequirement(const void* ptr)
	{
		return isAligned(ptr, getSIMDMemoryAlignmentRequirement());
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
