#pragma once

#include <random>
#include <limits>

namespace Base
{
	template <typename Type>
	class RNG
	{
	public:
		RNG(Type min = std::numeric_limits<Type>::min(), Type max = std::numeric_limits<Type>::max())
			: _distribution(min, max) {}
		Type get()
		{
			return _distribution(_engine);
		}
	private:
		std::default_random_engine _engine;
		std::uniform_int_distribution<Type> _distribution;
	};
}
