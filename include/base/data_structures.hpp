#pragma once

#include <vector>
#include <stdexcept>

namespace Base {
	template <typename Type>
	class CircularQueue
	{
	public:
		CircularQueue(size_t capacity);
		void push(const Type& item);
		void push(Type&& item);
		void pop();
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
		std::vector<Type> _array;
		size_t _front_index;
		size_t _back_index;
	};

	template <typename Type>
	CircularQueue<Type>::CircularQueue(size_t capacity)
		: _array(capacity), _front_index(0), _back_index(0)
	{
	}

	template <typename Type>
	void CircularQueue<Type>::push(const Type& item)
	{
		if (_back_index - _front_index == _array.size())
			throw std::runtime_error("queue is full");

		_array[_back_index % _array.size()] = item;
		_back_index++;

		shrink_index();
	}
	
	template <typename Type>
	void CircularQueue<Type>::push(Type&& item)
	{
		if (_back_index - _front_index == _array.size())
			throw std::runtime_error("queue is full");

		_array[_back_index % _array.size()] = std::move(item);
		_back_index++;

		shrink_index();
	}

	template <typename Type>
	void CircularQueue<Type>::pop()
	{
		if (_back_index == _front_index)
			throw std::runtime_error("queue is empty");

		_front_index++;
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
		return _array[_front_index % _array.size()];
	}

	template <typename Type>
	Type& CircularQueue<Type>::back()
	{
		if (_back_index == _front_index) throw std::runtime_error("queue is empty");
		return _array[(_back_index - 1) % _array.size()];
	}

	template <typename Type>
	const Type& CircularQueue<Type>::front() const
	{
		if (_back_index == _front_index) throw std::runtime_error("queue is empty");
		return _array[_front_index % _array.size()];
	}

	template <typename Type>
	const Type& CircularQueue<Type>::back() const
	{
		if (_back_index == _front_index) throw std::runtime_error("queue is empty");
		return _array[(_back_index - 1) % _array.size()];
	}

	template<typename Type>
	Type& CircularQueue<Type>::operator[](size_t index)
	{
		if (index >= size()) throw std::out_of_range("index out of range");
		return _array[(_front_index + index) % _array.size()];
	}

	template<typename Type>
	const Type& CircularQueue<Type>::operator[](size_t index) const
	{
		if (index >= size()) throw std::out_of_range("index out of range");
		return _array[(_front_index + index) % _array.size()];
	}

	template<typename Type>
	void CircularQueue<Type>::clear()
	{
		_back_index = 0;
		_front_index = 0;
	}

	template <typename Type>
	size_t CircularQueue<Type>::capacity() const
	{
		return _array.size();
	}

	template <typename Type>
	void CircularQueue<Type>::shrink_index()
	{
		if (_back_index >= 2 * _array.size())
		{
			_back_index -= _array.size();
			_front_index -= _array.size();
		}
	}

	template <typename Type>
	class FixedVector
	{
	public:
		typedef const Type* const_iterator;
		typedef Type* iterator;
		FixedVector(size_t reserved);
		~FixedVector();
		template <class... Args>
		void emplace_back(Args... args);
		template <class... Args>
		void replace(size_t index, Args... args);
		void erase_back();
		Type& operator[](size_t index);
		const Type& operator[](size_t index) const;
		size_t size() const;
		const_iterator begin() const;
		iterator begin();
		const_iterator end() const;
		iterator end();
	private:
		size_t _reserved;
		size_t _size;
		Type* _ptr;
	};

	template <typename Type>
	FixedVector<Type>::FixedVector(size_t reserved)
		: _reserved(reserved), _size(0), _ptr((Type*)malloc(sizeof(Type) * reserved))
	{
		if (!_ptr)
			throw std::bad_alloc();
	}

	template <typename Type>
	FixedVector<Type>::~FixedVector()
	{
		for (size_t i = _size; i > 0; --i)
		{
			_ptr[i - 1].~Type();
		}
		free(_ptr);
	}

	template <typename Type>
	template <class ... Args>
	void FixedVector<Type>::emplace_back(Args... args)
	{
		if (_size >= _reserved)
			throw std::out_of_range("can not exceed reserved size");
		new (&_ptr[_size]) Type(args...);
		++_size;
	}

	template <typename Type>
	template <class ... Args>
	void FixedVector<Type>::replace(size_t index, Args... args)
	{
		if (index >= _size)
			throw std::out_of_range("");
		_ptr[index].~Type();
		new (&_ptr[index]) Type(args...);
	}

	template <typename Type>
	void FixedVector<Type>::erase_back()
	{
		if (_size == 0)
			throw std::out_of_range("");
		_ptr[_size - 1].~Type();
		--_size;
	}

	template <typename Type>
	Type& FixedVector<Type>::operator[](size_t index)
	{
		if (index >= _size)
			throw std::out_of_range("");
		return _ptr[index];
	}

	template <typename Type>
	const Type& FixedVector<Type>::operator[](size_t index) const
	{
		if (index >= _size)
			throw std::out_of_range("");
		return _ptr[index];
	}

	template <typename Type>
	size_t FixedVector<Type>::size() const
	{
		return _size;
	}

	template <typename Type>
	typename FixedVector<Type>::const_iterator FixedVector<Type>::begin() const
	{
		return _ptr;
	}

	template <typename Type>
	typename FixedVector<Type>::iterator FixedVector<Type>::begin()
	{
		return _ptr;
	}

	template <typename Type>
	typename FixedVector<Type>::const_iterator FixedVector<Type>::end() const
	{
		return _ptr + _size;
	}

	template <typename Type>
	typename FixedVector<Type>::iterator FixedVector<Type>::end()
	{
		return _ptr + _size;
	}
}
