#pragma once

#include <iterator>

namespace Base
{
	class EndIterationException : std::exception
	{
	};
	
	template <class List>
	class ConstRandomAccessListIteratorWrapper
	{
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef typename List::value_type value_type;
		typedef size_t difference_type;
		typedef value_type* pointer;
		typedef const value_type& reference;

	protected:
		List _list;
		size_t _index;
	public:
		ConstRandomAccessListIteratorWrapper(const List& list, size_t index = 0) : _list(list), _index(index) {}
		
		ConstRandomAccessListIteratorWrapper(size_t index = 0) : _index(index) {}
		
		reference operator*() const
		{
			return _list.get(_index);
		}

		const value_type* operator->() const
		{
			return &_list.get(_index);
		}

		reference operator[](difference_type off) const
		{
			return _list.get(off + _index);
		}

		//Increment / Decrement
		ConstRandomAccessListIteratorWrapper& operator++()
		{
			++_index;  return *this;
		}

		ConstRandomAccessListIteratorWrapper operator++(int)
		{
			auto tmp = *this;
			++* this;
			return tmp;			
		}

		ConstRandomAccessListIteratorWrapper& operator--()
		{
			--_index; return *this;
		}

		ConstRandomAccessListIteratorWrapper operator--(int)
		{
			auto tmp = *this;
			--* this;
			return tmp;
		}

		//Arithmetic
		ConstRandomAccessListIteratorWrapper& operator+=(difference_type off)
		{
			_index += off; return *this;
		}

		ConstRandomAccessListIteratorWrapper operator+(difference_type off) const
		{
			auto tmp = *this;
			tmp._index += off;
			return tmp;
		}

		friend ConstRandomAccessListIteratorWrapper operator+(difference_type off, const ConstRandomAccessListIteratorWrapper& right)
		{
			auto tmp = right;
			tmp._index += off;
			return tmp;
		}

		ConstRandomAccessListIteratorWrapper& operator-=(difference_type off)
		{
			_index -= off; return *this;
		}

		ConstRandomAccessListIteratorWrapper operator-(difference_type off) const
		{
			auto tmp = *this;
			tmp._index -= off;
			return tmp;
		}

		difference_type operator-(const ConstRandomAccessListIteratorWrapper& right) const
		{
			return _index - right._index;
		}

		//Comparison operators
		bool operator==(const ConstRandomAccessListIteratorWrapper& r)  const
		{
			return _index == r._index;
		}

		bool operator!=(const ConstRandomAccessListIteratorWrapper& r)  const
		{
			return _index != r._index;
		}

		bool operator<(const ConstRandomAccessListIteratorWrapper& r)  const
		{
			return _index < r._index;
		}

		bool operator<=(const ConstRandomAccessListIteratorWrapper& r)  const
		{
			return _index <= r._index;
		}

		bool operator>(const ConstRandomAccessListIteratorWrapper& r)  const
		{
			return _index > r._index;
		}

		bool operator>=(const ConstRandomAccessListIteratorWrapper& r)  const
		{
			return _index >= r._index;
		}
	};

	template <class List>
	class RandomAccessListIteratorWrapper
		: public ConstRandomAccessListIteratorWrapper<List>
	{
	public:
		RandomAccessListIteratorWrapper(const List& list, size_t index = 0)
			: ConstRandomAccessListIteratorWrapper<List>(list, index) {}

		RandomAccessListIteratorWrapper(size_t index = 0)
			: ConstRandomAccessListIteratorWrapper<List>(index)
		{}
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef typename List::value_type value_type;
		typedef size_t difference_type;
		typedef value_type* pointer;
		typedef value_type& reference;

		//Pointer like operators
		reference operator*()  const
		{
			return this->_list.get();
		}

		value_type* operator->() const
		{
			return this->_list.get();
		}

		reference operator[](difference_type off) const
		{
			return this->_list[off + this->_index];
		}

		//Increment / Decrement
		RandomAccessListIteratorWrapper& operator++()
		{
			++this->_index; return *this;
		}

		RandomAccessListIteratorWrapper operator++(int)
		{
			auto tmp = *this; ++* this; return tmp;
		}

		RandomAccessListIteratorWrapper& operator--()
		{
			--this->_index; return *this;
		}

		RandomAccessListIteratorWrapper operator--(int)
		{
			auto tmp = *this; --* this; return tmp;
		}

		// Arithmetic
		RandomAccessListIteratorWrapper& operator+=(difference_type off)
		{
			this->_index += off;  return *this;
		}

		RandomAccessListIteratorWrapper operator+(difference_type off) const
		{
			auto tmp = *this;
			tmp._index += off;
			return tmp;
		}

		friend RandomAccessListIteratorWrapper operator+(difference_type off, const RandomAccessListIteratorWrapper& right)
		{
			auto tmp = right;
			tmp._index += off;
			return tmp;
		}

		RandomAccessListIteratorWrapper& operator-=(difference_type off)
		{
			this->_index -= off; return *this;
		}

		RandomAccessListIteratorWrapper operator-(difference_type off) const
		{
			auto tmp = *this;
			tmp._index -= off;
			return tmp;
		}

		difference_type operator-(const ConstRandomAccessListIteratorWrapper<List>& right) const
		{
			return static_cast<const ConstRandomAccessListIteratorWrapper<List>&>(*this) - right;
		}
	};

	template <class Enumerator>
	 class ConstIteratorWrapper
	 {
	 public:
		 typedef std::input_iterator_tag iterator_category;
		 typedef typename Enumerator::value_type value_type;
		 typedef size_t difference_type;
		 typedef value_type* pointer;
		 typedef const value_type& reference;

	 protected:
		 Enumerator _enumerator;
		 bool _endState;
	 public:
		 template <typename ...EnumeratorParameterTypes>
		 ConstIteratorWrapper(std::tuple<EnumeratorParameterTypes...> enumeratorParameterTypes, bool endState = false) : _enumerator(std::make_from_tuple<Enumerator>(enumeratorParameterTypes...)), _endState(endState)
		 {}

		 ConstIteratorWrapper(const Enumerator& enumerator, bool endState = false) : _enumerator(enumerator), _endState(endState) {}

		 ConstIteratorWrapper(bool endState = false) : _endState(endState) {}
	 
		reference operator*() const
		{
			return _enumerator.current();
		}

		const value_type* operator->() const
		{
			return &_enumerator.current();
		}

		//Increment / Decrement
		ConstIteratorWrapper& operator++()
		{
			if (!_enumerator.moveNext()) _endState = true;
			return *this;
		}
		
		//Comparison operators
		bool operator==(const ConstIteratorWrapper& r)  const
		{
			return _endState == r._endState;
		}

		bool operator!=(const ConstIteratorWrapper& r)  const
		{
			return _endState != r._endState;
		}
	};
	
	template <class Enumerator>
	class IteratorWrapper
		: public ConstIteratorWrapper<Enumerator>
	{
	public:
		template <typename ...EnumeratorParameterTypes>
		IteratorWrapper(std::tuple<EnumeratorParameterTypes...> enumeratorParameterTypes, bool endState = false) : ConstIteratorWrapper<Enumerator>(enumeratorParameterTypes, endState)
		{}

		IteratorWrapper(const Enumerator& enumerator, bool endState = false) : ConstIteratorWrapper<Enumerator>(enumerator, endState) {}

		IteratorWrapper(bool endState = false) : ConstIteratorWrapper<Enumerator>(endState) {}
		typedef std::input_iterator_tag iterator_category;
		typedef typename Enumerator::value_type value_type;
		typedef size_t difference_type;
		typedef value_type* pointer;
		typedef value_type& reference;

		//Pointer like operators
		reference operator*()  const
		{
			return this->_enumerator.current();
		}

		value_type* operator->() const
		{
			return &this->_enumerator.current();
		}

		reference operator[](difference_type off) const
		{
			return this->_list[off + this->_index];
		}

		//Increment / Decrement
		IteratorWrapper& operator++()
		{
			++this->_index; return *this;
		}
	};

	class 
	
	class EnumeratorIteratorWrapper
	{
	public:
		
	};
}