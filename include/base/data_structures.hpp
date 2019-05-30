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
}