#pragma once

#include <random>
#include <limits>
#include <type_traits>

namespace Base
{
	class RNG
	{
	public:
		RNG(std::default_random_engine::result_type seed = std::random_device()()) : _engine(seed) {}
		// [a, b]
		template <typename Type,
			std::enable_if_t<std::is_same_v<Type, short> || std::is_same_v<Type, int> || std::is_same_v<Type, long> ||
			std::is_same_v<Type, long long> || std::is_same_v<Type, unsigned short> || std::is_same_v<Type, unsigned int> || std::is_same_v<Type, unsigned long> ||
			std::is_same_v<Type, unsigned long long>>* = nullptr>
			Type get(Type min = std::numeric_limits<Type>::min(), Type max = std::numeric_limits<Type>::max())
		{
			std::uniform_int_distribution<Type> distribution(min, max);
			return distribution(_engine);
		}

		// [a, b]
		template <typename Type,
			std::enable_if_t<std::numeric_limits<Type>::is_integer && std::is_signed_v<Type> &&
			!(std::is_same_v<Type, short> || std::is_same_v<Type, int> || std::is_same_v<Type, long> || std::is_same_v<Type, long long>)>* = nullptr>
			Type get(Type min = std::numeric_limits<Type>::min(), Type max = std::numeric_limits<Type>::max())
		{
			std::uniform_int_distribution<int> distribution(min, max);
			return Type(distribution(_engine));
		}

		// [a, b]
		template <typename Type,
			std::enable_if_t<std::numeric_limits<Type>::is_integer && std::is_unsigned_v<Type> &&
			!(std::is_same_v<Type, unsigned short> || std::is_same_v<Type, unsigned int> || std::is_same_v<Type, unsigned long> ||
				std::is_same_v<Type, unsigned long long>)>* = nullptr>
			Type get(Type min = std::numeric_limits<Type>::min(), Type max = std::numeric_limits<Type>::max())
		{
			std::uniform_int_distribution<unsigned> distribution(min, max);
			return Type(distribution(_engine));
		}

		// [a, b)
		// To create a distribution over the closed interval [a,b], std::nextafter(b, std::numeric_limits<RealType>::max()) may be used as the second parameter.
		template <typename Type,
			std::enable_if_t<std::is_floating_point<Type>::value>* = nullptr>
			Type get(Type min = std::numeric_limits<Type>::min(), Type max = std::numeric_limits<Type>::max())
		{
			std::uniform_real_distribution<Type> distribution(min, max);
			return distribution(_engine);
		}

		template <typename RealType>
		RealType get_normal(RealType min = std::numeric_limits<RealType>::min(), RealType max = std::numeric_limits<RealType>::max())
		{
			std::normal_distribution<RealType> distribution(min, max);
			return distribution(_engine);
		}
	private:
		std::default_random_engine _engine;
	};
}
