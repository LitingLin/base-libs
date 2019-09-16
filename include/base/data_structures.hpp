#pragma once

#include <vector>
#include <stdexcept>
#include <base/memory_alignment.h>

namespace Base {

	template <typename Type>
	class FixedVector
	{
	public:
		typedef const Type* const_iterator;
		typedef Type* iterator;
		FixedVector(size_t reserved);
		~FixedVector();
		template <class... Args>
		void emplace_back(Args&& ... args);
		template <class... Args>
		void replace(size_t index, Args&& ... args);
		void erase_back();
		Type& operator[](size_t index);
		const Type& operator[](size_t index) const;
		size_t size() const;
		const_iterator begin() const;
		iterator begin();
		const_iterator end() const;
		iterator end();
		size_t capacity() const;
	private:
		size_t _reserved;
		size_t _size;
		AlignedSpan<Type> _alignedSpan;
	};

	template <typename Type>
	FixedVector<Type>::FixedVector(size_t reserved)
		: _reserved(reserved), _size(0), _alignedSpan(reserved)
	{
	}

	template <typename Type>
	FixedVector<Type>::~FixedVector()
	{
		for (size_t i = _size; i > 0; --i)
		{
			_alignedSpan.get()[i - 1].~Type();
		}
	}

	template <typename Type>
	template <class ... Args>
	void FixedVector<Type>::emplace_back(Args&& ... args)
	{
		if (_size >= _reserved)
			throw std::out_of_range("can not exceed reserved size");
		new (_alignedSpan.get() + _size++) Type(std::forward<Args>(args)...);
	}

	template <typename Type>
	template <class ... Args>
	void FixedVector<Type>::replace(size_t index, Args&& ... args)
	{
		if (index >= _size)
			throw std::out_of_range("");
		_alignedSpan.get()[index].~Type();
		new (_alignedSpan.get()  + index) Type(std::forward<Args>(args)...);
	}

	template <typename Type>
	void FixedVector<Type>::erase_back()
	{
		if (_size == 0)
			throw std::out_of_range("");
		_alignedSpan.get()[_size - 1].~Type();
		--_size;
	}

	template <typename Type>
	Type& FixedVector<Type>::operator[](size_t index)
	{
		if (index >= _size)
			throw std::out_of_range("");
		return _alignedSpan.get()[index];
	}

	template <typename Type>
	const Type& FixedVector<Type>::operator[](size_t index) const
	{
		if (index >= _size)
			throw std::out_of_range("");
		return _alignedSpan.get()[index];
	}

	template <typename Type>
	size_t FixedVector<Type>::size() const
	{
		return _size;
	}

	template <typename Type>
	typename FixedVector<Type>::const_iterator FixedVector<Type>::begin() const
	{
		return _alignedSpan.get();
	}

	template <typename Type>
	typename FixedVector<Type>::iterator FixedVector<Type>::begin()
	{
		return _alignedSpan.get();
	}

	template <typename Type>
	typename FixedVector<Type>::const_iterator FixedVector<Type>::end() const
	{
		return _alignedSpan.get() + _size;
	}

	template <typename Type>
	typename FixedVector<Type>::iterator FixedVector<Type>::end()
	{
		return _alignedSpan.get() + _size;
	}

	template <typename Type>
	size_t FixedVector<Type>::capacity() const
	{
		return _reserved;
	}

	template <typename Type>
	class CircularQueue
	{
	public:
		CircularQueue(size_t capacity);
		~CircularQueue();
		void push(const Type& item);
		void push(Type&& item);
		Type&& pop();
		size_t size() const;
		Type& front();
		Type& back();
		const Type& front() const;
		const Type& back() const;
		Type& operator[](size_t index);
		const Type& operator[](size_t index) const;
		void clear();
		size_t capacity() const;
	private:
		void shrink_index();
		AlignedSpan<Type> _alignedSpan;
		size_t _capacity;
		size_t _front_index;
		size_t _back_index;
	};

	template <typename Type>
	CircularQueue<Type>::CircularQueue(size_t capacity)
		: _alignedSpan(capacity), _capacity(capacity), _front_index(0), _back_index(0)
	{
	}

	template <typename Type>
	CircularQueue<Type>::~CircularQueue()
	{
		clear();
	}

	template <typename Type>
	void CircularQueue<Type>::push(const Type& item)
	{
		if (_back_index - _front_index == _capacity)
			throw std::runtime_error("queue is full");

		new (_alignedSpan.get() + _back_index % _capacity) Type(item);
		_back_index++;

		shrink_index();
	}
	
	template <typename Type>
	void CircularQueue<Type>::push(Type&& item)
	{
		if (_back_index - _front_index == _capacity)
			throw std::runtime_error("queue is full");

		new (_alignedSpan.get() + _back_index % _capacity) Type(std::move(item));
		_back_index++;

		shrink_index();
	}

	template <typename Type>
	Type&& CircularQueue<Type>::pop()
	{
		if (_back_index == _front_index)
			throw std::runtime_error("queue is empty");

		return std::move(_alignedSpan.get()[(_front_index++) % _capacity]);
	}

	template <typename Type>
	size_t CircularQueue<Type>::size() const
	{
		return _back_index - _front_index;
	}

	template <typename Type>
	Type& CircularQueue<Type>::front()
	{
		if (_back_index == _front_index) throw std::runtime_error("queue is empty");
		return _alignedSpan.get()[_front_index % _capacity];
	}

	template <typename Type>
	Type& CircularQueue<Type>::back()
	{
		if (_back_index == _front_index) throw std::runtime_error("queue is empty");
		return _alignedSpan.get()[(_back_index - 1) % _capacity];
	}

	template <typename Type>
	const Type& CircularQueue<Type>::front() const
	{
		if (_back_index == _front_index) throw std::runtime_error("queue is empty");
		return _alignedSpan.get()[_front_index % _capacity];
	}

	template <typename Type>
	const Type& CircularQueue<Type>::back() const
	{
		if (_back_index == _front_index) throw std::runtime_error("queue is empty");
		return _alignedSpan.get()[(_back_index - 1) % _capacity];
	}

	template<typename Type>
	Type& CircularQueue<Type>::operator[](size_t index)
	{
		if (index >= size()) throw std::out_of_range("index out of range");
		return _alignedSpan.get()[(_front_index + index) % _capacity];
	}

	template<typename Type>
	const Type& CircularQueue<Type>::operator[](size_t index) const
	{
		if (index >= size()) throw std::out_of_range("index out of range");
		return _alignedSpan.get()[(_front_index + index) % _capacity];
	}

	template<typename Type>
	void CircularQueue<Type>::clear()
	{
		for (size_t i = _front_index; i < _back_index; ++i)
		{
			_alignedSpan.get()[i % _capacity].~Type();
		}
		_back_index = 0;
		_front_index = 0;
	}

	template <typename Type>
	size_t CircularQueue<Type>::capacity() const
	{
		return _capacity;
	}

	template <typename Type>
	void CircularQueue<Type>::shrink_index()
	{
		if (_back_index >= 2 * _capacity)
		{
			_back_index -= _capacity;
			_front_index -= _capacity;
		}
	}
}
