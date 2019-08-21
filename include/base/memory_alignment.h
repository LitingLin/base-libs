#pragma once

#include <base/common.h>
#include <cstddef>

namespace Base
{
	ATTRIBUTE_INTERFACE
	unsigned getSIMDMemoryAlignmentRequirement();
	ATTRIBUTE_INTERFACE
	bool isAligned(const void* ptr, unsigned alignment);
	ATTRIBUTE_INTERFACE
	bool isAlignedWithSIMDMemoryAlignmentRequirement(const void* ptr);

	class ATTRIBUTE_INTERFACE AlignedMemorySpace
	{
	public:
		AlignedMemorySpace(size_t size, unsigned alignment = getSIMDMemoryAlignmentRequirement());
		AlignedMemorySpace(const AlignedMemorySpace&) = delete;
		AlignedMemorySpace(AlignedMemorySpace&&) = delete;
		~AlignedMemorySpace();
		[[nodiscard]] void* get();
	private:
		void* _ptr;
	};

	template <typename Type>
	class AlignedSpan
	{
	public:
		AlignedSpan(size_t numberOfElements, unsigned alignment = getSIMDMemoryAlignmentRequirement()) : _alignedMemorySpace(numberOfElements * sizeof(Type), alignment) {}
		[[nodiscard]] Type* get() { return (Type*)_alignedMemorySpace.get(); }
	private:
		AlignedMemorySpace _alignedMemorySpace;
	};

	class ATTRIBUTE_INTERFACE AlignedDynamicRawArray
	{
	public:
		AlignedDynamicRawArray();
		AlignedDynamicRawArray(size_t size, unsigned alignment = getSIMDMemoryAlignmentRequirement());
		AlignedDynamicRawArray(const AlignedDynamicRawArray& other);
		AlignedDynamicRawArray(AlignedDynamicRawArray&& other) noexcept;
		~AlignedDynamicRawArray();
		[[nodiscard]] void* get() const;
		[[nodiscard]] size_t size() const;
		[[nodiscard]] unsigned alignment() const;
		void resize(size_t size, unsigned alignment = getSIMDMemoryAlignmentRequirement());
	private:
		void* _ptr;
		size_t _size;
		unsigned _alignment;
	};

	template <typename Type>
	class AlignedDynamicArray
	{
	public:
		AlignedDynamicArray() {}
		AlignedDynamicArray(size_t numberOfElements, unsigned alignment = getSIMDMemoryAlignmentRequirement()) : _array(numberOfElements * sizeof(Type), alignment) {}
		[[nodiscard]] Type* get() const { return (Type*)_array.get(); }
		[[nodiscard]] size_t elements() const { return _array.size() / sizeof(Type); }
		[[nodiscard]] size_t bytes() const { return _array.size(); }
		[[nodiscard]] unsigned alignment() const { return _array.alignment(); }
		void resize(size_t numberOfElements, unsigned alignment = getSIMDMemoryAlignmentRequirement()) { return _array.resize(numberOfElements * sizeof(Type), alignment); }
	private:
		AlignedDynamicRawArray _array;
	};
}
