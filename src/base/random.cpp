#include <base/random.h>

Base::UnrepeatableRandomIntegerGenerator::UnrepeatableRandomIntegerGenerator(unsigned size,
	std::default_random_engine::result_type seed)
	: _rng(seed), _values(size, 0), _consumed(0)
{
}

bool Base::UnrepeatableRandomIntegerGenerator::hasNext()
{
	return _values.size() != _consumed;
}

unsigned Base::UnrepeatableRandomIntegerGenerator::get()
{
	unsigned index = _rng.get<unsigned>(0, _values.size() - _consumed);
	unsigned value = _values[index];
	if (value == 0)	_values[index] = value = index + 1;
	unsigned swap_index = _values.size() - _consumed - 1;
	if (_values[swap_index] == 0) _values[swap_index] = swap_index + 1;
	std::swap(_values[index], _values[swap_index]);
	return value - 1;
}

void Base::UnrepeatableRandomIntegerGenerator::reset()
{
	_consumed = 0;
	memset(_values.data(), 0, sizeof(_values[0]) * _values.size());
}
