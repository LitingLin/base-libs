#pragma once

#include <optional>
#include <string>
#include <tuple>
#include <functional>
#include <type_traits>

namespace Base
{
	namespace Details {
		template<typename C>
		struct IsType1Iterator {
		private:
			template<typename T>
			static constexpr auto check(T*)
				->std::bool_constant<
				std::is_lvalue_reference_v<
				decltype(std::declval<T>().get(std::declval<size_t>()))> &&
				std::is_same_v<decltype(std::declval<T>().size()), size_t>
				>;

			template<typename>
			static constexpr std::false_type check(...);

			typedef decltype(check<C>(0)) type;

		public:
			static constexpr bool value = type::value;
		};

		template<typename C>
		struct IsType1Generator {
		private:
			template<typename T>
			static constexpr auto check(T*)
				->std::bool_constant<
				!std::is_lvalue_reference_v<
				decltype(std::declval<T>().get(std::declval<size_t>()))> &&
				std::is_same_v<decltype(std::declval<T>().size()), size_t>>;

			template<typename>
			static constexpr std::false_type check(...);

			typedef decltype(check<C>(0)) type;

		public:
			static constexpr bool value = type::value;
		};

		template<typename C>
		struct IsType2Iterator {
		private:
			template<typename T>
			static constexpr auto check(T*)
				->std::bool_constant <
				std::is_lvalue_reference_v<
				decltype(std::declval<T>().current())> &&
				std::is_same_v<decltype(std::declval<T>().moveNext()), bool>>;

			template<typename>
			static constexpr std::false_type check(...);

			typedef decltype(check<C>(0)) type;

		public:
			static constexpr bool value = type::value;
		};

		template<typename C>
		struct IsType2Generator {
		private:
			template<typename T>
			static constexpr auto check(T*)
				->std::bool_constant<
				!std::is_lvalue_reference_v<
				decltype(std::declval<T>().current())> &&
				std::is_same_v<decltype(std::declval<T>().moveNext()), bool>>;

			template<typename>
			static constexpr std::false_type check(...);

			typedef decltype(check<C>(0)) type;

		public:
			static constexpr bool value = type::value;
		};
	}

	template <typename, typename = void>
	class ConstIteratorWrapper;

	template <typename, typename = void>
	class IteratorWrapper;

	template <typename List>
	class ConstIteratorWrapper<List, std::enable_if_t<Details::IsType1Iterator<List>::value>>
	{
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef typename List::value_type value_type;
		typedef size_t difference_type;
		typedef value_type* pointer;
		typedef const value_type& reference;

	protected:
		std::optional<List> _list;
		size_t _index;
	public:
		ConstIteratorWrapper(size_t index, std::nullopt_t) : _list(std::nullopt), _index(index) {}

		template <class... ArgumentTypes>
		ConstIteratorWrapper(size_t index, std::in_place_t, ArgumentTypes&&...arguments) : _list(std::in_place, std::forward<ArgumentTypes>(arguments)...), _index(index) {}

		reference operator*() const
		{
			return _list.value().get(_index);
		}

		const value_type* operator->() const
		{
			return &_list.value().get(_index);
		}

		reference operator[](difference_type off) const
		{
			return _list.value().get(off + _index);
		}

		//Increment / Decrement
		ConstIteratorWrapper& operator++()
		{
			++_index;  return *this;
		}

		ConstIteratorWrapper operator++(int)
		{
			auto tmp = *this;
			++* this;
			return tmp;
		}

		ConstIteratorWrapper& operator--()
		{
			--_index; return *this;
		}

		ConstIteratorWrapper operator--(int)
		{
			auto tmp = *this;
			--* this;
			return tmp;
		}

		//Arithmetic
		ConstIteratorWrapper& operator+=(difference_type off)
		{
			_index += off; return *this;
		}

		ConstIteratorWrapper operator+(difference_type off) const
		{
			auto tmp = *this;
			tmp._index += off;
			return tmp;
		}

		friend ConstIteratorWrapper operator+(difference_type off, const ConstIteratorWrapper& right)
		{
			auto tmp = right;
			tmp._index += off;
			return tmp;
		}

		ConstIteratorWrapper& operator-=(difference_type off)
		{
			_index -= off; return *this;
		}

		ConstIteratorWrapper operator-(difference_type off) const
		{
			auto tmp = *this;
			tmp._index -= off;
			return tmp;
		}

		difference_type operator-(const ConstIteratorWrapper& right) const
		{
			return _index - right._index;
		}

		//Comparison operators
		bool operator==(const ConstIteratorWrapper& r)  const
		{
			return _index == r._index;
		}

		bool operator!=(const ConstIteratorWrapper& r)  const
		{
			return _index != r._index;
		}

		bool operator<(const ConstIteratorWrapper& r)  const
		{
			return _index < r._index;
		}

		bool operator<=(const ConstIteratorWrapper& r)  const
		{
			return _index <= r._index;
		}

		bool operator>(const ConstIteratorWrapper& r)  const
		{
			return _index > r._index;
		}

		bool operator>=(const ConstIteratorWrapper& r)  const
		{
			return _index >= r._index;
		}
	};

	template <class List>
	class IteratorWrapper<List, std::enable_if_t<Details::IsType1Iterator<List>::value>>
		: public ConstIteratorWrapper<List>
	{
	public:
		IteratorWrapper(size_t index, std::nullopt_t) : ConstIteratorWrapper(index, std::nullopt) {}

		template <class... ArgumentTypes>
		IteratorWrapper(size_t index, std::in_place_t, ArgumentTypes&&...arguments) : ConstIteratorWrapper(index, std::in_place, std::forward<ArgumentTypes>(arguments)...) {}

		typedef std::random_access_iterator_tag iterator_category;
		typedef typename List::value_type value_type;
		typedef size_t difference_type;
		typedef value_type* pointer;
		typedef value_type& reference;

		//Pointer like operators
		reference operator*()  const
		{
			return this->_list.value().get();
		}

		value_type* operator->() const
		{
			return this->_list.value().get();
		}

		reference operator[](difference_type off) const
		{
			return this->_list.value()[off + this->_index];
		}

		//Increment / Decrement
		IteratorWrapper& operator++()
		{
			++this->_index; return *this;
		}

		IteratorWrapper operator++(int)
		{
			auto tmp = *this; ++* this; return tmp;
		}

		IteratorWrapper& operator--()
		{
			--this->_index; return *this;
		}

		IteratorWrapper operator--(int)
		{
			auto tmp = *this; --* this; return tmp;
		}

		// Arithmetic
		IteratorWrapper& operator+=(difference_type off)
		{
			this->_index += off;  return *this;
		}

		IteratorWrapper operator+(difference_type off) const
		{
			auto tmp = *this;
			tmp._index += off;
			return tmp;
		}

		friend IteratorWrapper operator+(difference_type off, const IteratorWrapper& right)
		{
			auto tmp = right;
			tmp._index += off;
			return tmp;
		}

		IteratorWrapper& operator-=(difference_type off)
		{
			this->_index -= off; return *this;
		}

		IteratorWrapper operator-(difference_type off) const
		{
			auto tmp = *this;
			tmp._index -= off;
			return tmp;
		}

		difference_type operator-(const ConstIteratorWrapper<List>& right) const
		{
			return static_cast<const ConstIteratorWrapper<List>&>(*this) - right;
		}
	};

	template <typename List>
	class IteratorWrapper<List, std::enable_if_t<Details::IsType1Generator<List>::value>>
	{
	public:
		typedef typename List::value_type value_type;
		typedef size_t difference_type;

	protected:
		std::optional<List> _list;
		size_t _index;
	public:
		IteratorWrapper(size_t index, std::nullopt_t) : _list(std::nullopt), _index(index) {}


		template <class... ArgumentTypes>
		IteratorWrapper(size_t index, std::in_place_t, ArgumentTypes&&...arguments) : _list(std::in_place, std::forward<ArgumentTypes>(arguments)...), _index(index) {}

		value_type operator*() const
		{
			return _list.value().get(_index);
		}

		value_type operator[](difference_type off) const
		{
			return _list.value().get(off + _index);
		}

		//Increment / Decrement
		IteratorWrapper& operator++()
		{
			++_index;
			return *this;
		}

		IteratorWrapper operator++(int)
		{
			auto tmp = *this;
			++* this;
			return tmp;
		}

		IteratorWrapper& operator--()
		{
			--_index;
			return *this;
		}

		IteratorWrapper operator--(int)
		{
			auto tmp = *this;
			--* this;
			return tmp;
		}

		//Arithmetic
		IteratorWrapper& operator+=(difference_type off)
		{
			_index += off; return *this;
		}

		IteratorWrapper operator+(difference_type off) const
		{
			auto tmp = *this;
			tmp._index += off;
			return tmp;
		}

		friend IteratorWrapper operator+(difference_type off, const IteratorWrapper& right)
		{
			auto tmp = right;
			tmp._index += off;
			return tmp;
		}

		IteratorWrapper& operator-=(difference_type off)
		{
			_index -= off; return *this;
		}

		IteratorWrapper operator-(difference_type off) const
		{
			auto tmp = *this;
			tmp._index -= off;
			return tmp;
		}

		difference_type operator-(const IteratorWrapper& right) const
		{
			return _index - right._index;
		}

		//Comparison operators
		bool operator==(const IteratorWrapper& r)  const
		{
			return _index == r._index;
		}

		bool operator!=(const IteratorWrapper& r)  const
		{
			return _index != r._index;
		}

		bool operator<(const IteratorWrapper& r)  const
		{
			return _index < r._index;
		}

		bool operator<=(const IteratorWrapper& r)  const
		{
			return _index <= r._index;
		}

		bool operator>(const IteratorWrapper& r)  const
		{
			return _index > r._index;
		}

		bool operator>=(const IteratorWrapper& r)  const
		{
			return _index >= r._index;
		}
	};

	template <class Enumerator>
	class ConstIteratorWrapper<Enumerator, std::enable_if_t<Details::IsType2Iterator<Enumerator>::value>>
	{
	public:
		typedef std::input_iterator_tag iterator_category;
		typedef typename Enumerator::value_type value_type;
		typedef size_t difference_type;
		typedef value_type* pointer;
		typedef const value_type& reference;

	protected:
		std::optional<Enumerator> _enumerator;
	public:
		ConstIteratorWrapper(std::nullopt_t) : _enumerator(std::nullopt) {}

		template <class ...ParameterTypes>
		ConstIteratorWrapper(std::in_place_t, ParameterTypes... parameterTypes) : _enumerator(std::in_place, std::forward<ParameterTypes>(parameterTypes)...) {}

		reference operator*() const
		{
			return _enumerator.value().current();
		}

		const value_type* operator->() const
		{
			return &_enumerator.value().current();
		}

		//Increment / Decrement
		ConstIteratorWrapper& operator++()
		{
			if (!_enumerator.value().moveNext()) _enumerator.reset();
			return *this;
		}

		//Comparison operators
		bool operator==(const ConstIteratorWrapper& r) const
		{
			return _enumerator == r._enumerator;
		}

		bool operator!=(const ConstIteratorWrapper& r) const
		{
			return _enumerator != r._enumerator;
		}
	};

	template <class Enumerator>
	class IteratorWrapper<Enumerator, std::enable_if_t<Details::IsType2Iterator<Enumerator>::value>>
		: public ConstIteratorWrapper<Enumerator>
	{
	public:
		IteratorWrapper(std::nullopt_t) : ConstIteratorWrapper(std::nullopt) {}

		template <class ...ParameterTypes>
		IteratorWrapper(std::in_place_t, ParameterTypes... parameterTypes) : ConstIteratorWrapper(std::in_place, std::forward<ParameterTypes>(parameterTypes)...) {}

		typedef std::input_iterator_tag iterator_category;
		typedef typename Enumerator::value_type value_type;
		typedef size_t difference_type;
		typedef value_type* pointer;
		typedef value_type& reference;

		//Pointer like operators
		reference operator*()  const
		{
			return this->_enumerator.value().current();
		}

		value_type* operator->() const
		{
			return &this->_enumerator.value().current();
		}

		//Increment / Decrement
		IteratorWrapper& operator++()
		{
			if (!this->_enumerator.value().moveNext()) this->_enumerator.reset();
			return *this;
		}

		//Comparison operators
		bool operator==(const IteratorWrapper& r)  const
		{
			return this->_enumerator == r._enumerator;
		}

		bool operator!=(const IteratorWrapper& r)  const
		{
			return this->_enumerator != r._enumerator;
		}
	};

	template <typename Enumerator>
	class IteratorWrapper<Enumerator, std::enable_if_t<Details::IsType2Generator<Enumerator>::value>>
	{
	public:
		using value_type = typename Enumerator::value_type;

	protected:
		std::optional<Enumerator> _enumerator;
	public:
		IteratorWrapper(std::nullopt_t) : _enumerator(std::nullopt) {}


		template <class... ArgumentTypes>
		IteratorWrapper(std::in_place_t, ArgumentTypes&&...arguments) : _enumerator(std::in_place, std::forward<ArgumentTypes>(arguments)...) {}

		value_type operator*() const
		{
			return _enumerator.value().current();
		}

		//Increment / Decrement
		IteratorWrapper& operator++()
		{
			if (!this->_enumerator.value().moveNext()) this->_enumerator.reset();
			return *this;
		}

		//Comparison operators
		bool operator==(const IteratorWrapper& r)  const
		{
			return _enumerator == r._enumerator;
		}

		bool operator!=(const IteratorWrapper& r)  const
		{
			return _enumerator != r._enumerator;
		}
	};

	template <typename T>
	class Type1IteratorPtrWrapper
	{
	public:
		Type1IteratorPtrWrapper(T* pointer) : _pointer(pointer) {}
		auto get() { return _pointer->get(); }
		size_t size() { return _pointer->size(); }
	private:
		T* _pointer;
	};

	template <typename, typename = void>
	class RangeWrapper;

	template <typename T>
	class RangeWrapper<T, std::enable_if_t<Details::IsType1Iterator<T>::value>>
	{
	public:
		template <typename... IteratorParameters>
		RangeWrapper(IteratorParameters...parameters) :
			_iterator(std::forward<IteratorParameters>(parameters)...) {}
		typedef ConstIteratorWrapper<Type1IteratorPtrWrapper<T>> const_iterator;
		typedef IteratorWrapper<Type1IteratorPtrWrapper<T>> iterator;

		const_iterator begin() const
		{
			return const_iterator(std::in_place, 0, &_iterator);
		}
		const_iterator end() const
		{
			return const_iterator(std::nullopt, _iterator.size());
		}

		iterator begin()
		{
			return iterator(std::in_place, 0, &_iterator);
		}

		iterator end()
		{
			return iterator(std::nullopt, _iterator.size());
		}
	private:
		T _iterator;
	};

	template <typename T>
	class RangeWrapper<T, std::enable_if_t<Details::IsType1Generator<T>::value>>
	{
	public:
		template <typename... IteratorParameters>
		RangeWrapper(IteratorParameters&&...parameters) :
			_iterator(std::forward<IteratorParameters>(parameters)...) {}
		typedef IteratorWrapper<Type1IteratorPtrWrapper<T>> iterator;

		iterator begin()
		{
			return iterator(std::in_place, 0, &_iterator);
		}

		iterator end()
		{
			return iterator(std::nullopt, _iterator.size());
		}
	private:
		T _iterator;
	};

	template <typename T>
	class RangeWrapper<T, std::enable_if_t<Details::IsType2Iterator<T>::value>>
	{
	public:
		typedef ConstIteratorWrapper<T> const_iterator;
		typedef IteratorWrapper<T> iterator;

		struct IteratorConstructorImpl
		{
			template <typename Tuple, size_t... I>
			static constexpr const_iterator construct_const(Tuple& parameters, std::index_sequence<I...>)
			{
				return const_iterator(std::in_place, std::get<I>(parameters)...);
			}

			template <typename Tuple, size_t... I>
			static constexpr iterator construct(Tuple& parameters, std::index_sequence<I...>)
			{
				return iterator(std::in_place, std::get<I>(parameters)...);
			}
		};

		template <typename... IteratorParameters>
		RangeWrapper(IteratorParameters&&... parameters)
		{
			using TupleType = std::tuple<std::decay_t<IteratorParameters>...>;

			_parameters = new TupleType(std::forward<IteratorParameters>(parameters)...);

			_deleter = [](RangeWrapper* pointer) {delete (TupleType*)pointer->_parameters; };
			_constIteratorGetter = [](RangeWrapper* pointer) { return IteratorConstructorImpl::construct_const(*((TupleType*)pointer->_parameters), std::make_index_sequence<std::tuple_size<TupleType>::value>()); };
			_iteratorGetter = [](RangeWrapper* pointer) { return IteratorConstructorImpl::construct(*((TupleType*)pointer->_parameters), std::make_index_sequence<std::tuple_size<TupleType>::value>()); };
		}

		~RangeWrapper()
		{
			_deleter(this);
		}

		const_iterator begin() const
		{
			return _constIteratorGetter(this);
		}
		const_iterator end() const
		{
			return const_iterator(std::nullopt);
		}

		iterator begin()
		{
			return _iteratorGetter(this);
		}

		iterator end()
		{
			return iterator(std::nullopt);
		}
	private:
		void* _parameters;
		std::function<void(RangeWrapper*)> _deleter;
		std::function<const_iterator> _constIteratorGetter;
		std::function<iterator> _iteratorGetter;
	};

	template <typename T>
	class RangeWrapper<T, std::enable_if_t<Details::IsType2Generator<T>::value>>
	{
	public:
		typedef IteratorWrapper<T> iterator;

		struct IteratorConstructorImpl
		{
			template <typename Tuple, size_t... I>
			static constexpr iterator construct(Tuple& parameters, std::index_sequence<I...>)
			{
				return iterator(std::in_place, std::get<I>(parameters)...);
			}
		};

		template <typename... IteratorParameters>
		RangeWrapper(IteratorParameters&&... parameters)
		{
			using TupleType = std::tuple<std::decay_t<IteratorParameters>...>;

			_parameters = new TupleType(std::forward<IteratorParameters>(parameters)...);

			_deleter = [](RangeWrapper* pointer) {delete (TupleType*)pointer->_parameters; };
			_iteratorGetter = [](RangeWrapper* pointer) { return IteratorConstructorImpl::construct(*((TupleType*)pointer->_parameters), std::make_index_sequence<std::tuple_size<TupleType>::value>()); };
		}

		~RangeWrapper()
		{
			_deleter(this);
		}

		iterator begin()
		{
			return _iteratorGetter(this);
		}

		iterator end()
		{
			return iterator(std::nullopt);
		}
	private:
		void* _parameters;
		std::function<void(RangeWrapper*)> _deleter;
		std::function<iterator> _iteratorGetter;
	};
}