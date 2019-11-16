#pragma once

#include <base/random.hpp>

namespace Base {
	class UnrepeatableRandomIntegerGenerator
	{
	public:
		UnrepeatableRandomIntegerGenerator(unsigned size, std::default_random_engine::result_type seed = std::random_device()());
		bool hasNext();
		unsigned get();
		void reset();
	private:
		RNG _rng;
		std::vector<unsigned> _values;
		unsigned _consumed;
	};
}