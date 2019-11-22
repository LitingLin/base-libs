#pragma once


namespace Base
{
	template <class Factory>
	class RandomAccessGeneratorIteratorWrapper
	{
	public:
		typedef typename Factory::value_type value_type;
		typedef size_t difference_type;

	protected:
		Factory _factory;
		size_t _index;
	public:
		RandomAccessGeneratorIteratorWrapper(const Factory& factory, size_t index = 0) : _factory(factory), _index(index) {}

		RandomAccessGeneratorIteratorWrapper(size_t index = 0) : _index(index) {}

		value_type operator*() const
		{
			return _factory.get(_index);
		}

		value_type operator[](difference_type off) const
		{
			return _factory.get(off + _index);
		}

		//Increment / Decrement
		RandomAccessGeneratorIteratorWrapper& operator++()
		{
			++_index;  return *this;
		}

		RandomAccessGeneratorIteratorWrapper& operator--()
		{
			--_index; return *this;
		}

		RandomAccessGeneratorIteratorWrapper operator--(int)
		{
			auto tmp = *this;
			--* this;
			return tmp;
		}

		//Arithmetic
		RandomAccessGeneratorIteratorWrapper& operator+=(difference_type off)
		{
			_index += off; return *this;
		}

		RandomAccessGeneratorIteratorWrapper operator+(difference_type off) const
		{
			auto tmp = *this;
			tmp._index += off;
			return tmp;
		}

		friend RandomAccessGeneratorIteratorWrapper operator+(difference_type off, const RandomAccessGeneratorIteratorWrapper& right)
		{
			auto tmp = right;
			tmp._index += off;
			return tmp;
		}

		RandomAccessGeneratorIteratorWrapper& operator-=(difference_type off)
		{
			_index -= off; return *this;
		}

		RandomAccessGeneratorIteratorWrapper operator-(difference_type off) const
		{
			auto tmp = *this;
			tmp._index -= off;
			return tmp;
		}

		difference_type operator-(const RandomAccessGeneratorIteratorWrapper& right) const
		{
			return _index - right._index;
		}

		//Comparison operators
		bool operator==(const RandomAccessGeneratorIteratorWrapper& r)  const
		{
			return _index == r._index;
		}

		bool operator!=(const RandomAccessGeneratorIteratorWrapper& r)  const
		{
			return _index != r._index;
		}

		bool operator<(const RandomAccessGeneratorIteratorWrapper& r)  const
		{
			return _index < r._index;
		}

		bool operator<=(const RandomAccessGeneratorIteratorWrapper& r)  const
		{
			return _index <= r._index;
		}

		bool operator>(const RandomAccessGeneratorIteratorWrapper& r)  const
		{
			return _index > r._index;
		}

		bool operator>=(const RandomAccessGeneratorIteratorWrapper& r)  const
		{
			return _index >= r._index;
		}
	};
}
