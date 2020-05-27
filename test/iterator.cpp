#include "pch.h"

#include <base/iterator.hpp>

class ToyType1Iterator
{
public:
	ToyType1Iterator() : _i(1) {}
	int& get(size_t i)
	{
		return _i;
	}
	size_t size()
	{
		return 22;
	}
private:
	int _i;
};

TEST(IteratorWrapper, ToyType1IteratorRangeFor)
{
	Base::RangeWrapper<ToyType1Iterator> range;

	size_t count = 0;
	for (auto i : range)
	{
		EXPECT_EQ(i, 1);
		++count;
	}

	EXPECT_EQ(count, 22);
}

TEST(IteratorWrapper, ToyType1Iterator)
{
	Base::IteratorWrapper<ToyType1Iterator> test1_begin(0, std::in_place);
	Base::IteratorWrapper<ToyType1Iterator> test1_end(10, std::nullopt);

	size_t count = 0;
	for (;test1_begin!=test1_end;++test1_begin)
	{
		EXPECT_EQ(*test1_begin, 1);
		++count;
	}

	EXPECT_EQ(count, 10);
}

TEST(IteratorWrapper, ToyType1IteratorModifyContent)
{
	Base::IteratorWrapper<ToyType1Iterator> test1_begin(0, std::in_place);

	EXPECT_EQ(*test1_begin, 1);
	*test1_begin = 2;
	EXPECT_EQ(*test1_begin, 2);
}

TEST(IteratorWrapper, ToyType1IteratorWrapperModifyContent)
{
	Base::RangeWrapper<ToyType1Iterator> range;

	EXPECT_EQ(*range.begin(), 1);
	*range.begin() = 2;

	EXPECT_EQ(*range.begin(), 2);
}

class ToyType1Generator1
{
public:
	int get(size_t i) { return 1; }
	size_t size()
	{
		return 22;
	}
};

TEST(IteratorWrapper, ToyType1Generator1RangeFor)
{
	Base::RangeWrapper<ToyType1Generator1> range;

	size_t count = 0;
	for (auto i : range)
	{
		EXPECT_EQ(i, 1);
		++count;
	}

	EXPECT_EQ(count, 22);
}

TEST(IteratorWrapper, ToyType1Generator1)
{
	Base::IteratorWrapper<ToyType1Generator1> test1_begin(0, std::in_place);
	Base::IteratorWrapper<ToyType1Generator1> test1_end(10, std::nullopt);

	size_t count = 0;
	for (; test1_begin != test1_end; ++test1_begin)
	{
		EXPECT_EQ(*test1_begin, 1);
		++count;
	}

	EXPECT_EQ(count, 10);
}

class OnlyMoveableObject
{
public:
	OnlyMoveableObject() : _i(1) {}
	OnlyMoveableObject(const OnlyMoveableObject&) = delete;
	OnlyMoveableObject(OnlyMoveableObject&& r) noexcept { _i = 1; r._i = 0; }
	int _i;
};

struct OnlyMoveableObjectArray
{
public:
	OnlyMoveableObjectArray() : _objects(10) {}
	std::vector<OnlyMoveableObject> _objects;
};

class ToyType1Generator2
{
public:
	OnlyMoveableObject get(size_t i) { return std::move(_array._objects[i]); }
	size_t size() { return _array._objects.size(); }
	OnlyMoveableObjectArray _array;
};

TEST(IteratorWrapper, ToyType1Generator2)
{
	Base::IteratorWrapper<ToyType1Generator2> test1_begin(0, std::in_place);
	Base::IteratorWrapper<ToyType1Generator2> test1_end(10, std::nullopt);

	size_t count = 0;
	for (; test1_begin != test1_end; ++test1_begin)
	{
		auto&& value = *test1_begin;
		EXPECT_EQ(value._i, 1);
		
		++count;
	}

	EXPECT_EQ(count, 10);
}

TEST(IteratorWrapper, ToyType1Generator2RangeFor)
{
	Base::RangeWrapper<ToyType1Generator2> range;

	for (auto &&item : range)
	{
		EXPECT_EQ(item._i, 1);
	}
}

class ToyType2Iterator
{
public:
	ToyType2Iterator() : vector(10, 1) {}
	int& current() { return vector[count]; }
	bool moveNext()
	{
		++count;
		if (count == 10)
			return false;
		return true;
	}
private:
	int count = 0;
	std::vector<int> vector;
};

class ToyType2Generator1
{
public:
	int current() { return 1; }
	bool moveNext()
	{
		++count;
		if (count == 10)
			return false;
		return true;
	}
private:
	int count = 0;
};

class ToyType2Generator2
{
public:
	OnlyMoveableObject current() { return std::move(_object); }
	bool moveNext()
	{
		++count;
		if (count == 10)
			return false;
		return true;
	}
private:
	OnlyMoveableObject _object;
	int count = 0;
};

TEST(IteratorWrapper, ToyType2Iterator)
{
	Base::IteratorWrapper<ToyType2Iterator> test_begin(std::in_place);
	Base::IteratorWrapper<ToyType2Iterator> test_end(std::nullopt);

	int count = 0;
	for (; test_begin != test_end; ++test_begin)
	{
		EXPECT_EQ(*test_begin, 1);
		*test_begin = 2;
		EXPECT_EQ(*test_begin, 2);
		++count;
	}

	EXPECT_EQ(count, 10);	
}

TEST(IteratorWrapper, ToyType2IteratorRangeFor)
{
	Base::RangeWrapper<ToyType2Iterator> range;

	for (auto &&item : range)
	{
		EXPECT_EQ(item, 1);		
	}
}

TEST(IteratorWrapper, ToyType2Generator1)
{
	Base::IteratorWrapper<ToyType2Generator1> test_begin(std::in_place);
	Base::IteratorWrapper<ToyType2Generator1> test_end(std::nullopt);

	int count = 0;
	for (; test_begin != test_end; ++test_begin)
	{
		EXPECT_EQ(*test_begin, 1);
		++count;
	}

	EXPECT_EQ(count, 10);
}

TEST(IteratorWrapper, ToyType2Generator1RangeFor)
{
	Base::RangeWrapper<ToyType2Generator1> range;

	for (auto&& item : range)
	{
		EXPECT_EQ(item, 1);
	}
}

TEST(IteratorWrapper, ToyType2Generator2)
{
	Base::IteratorWrapper<ToyType2Generator2> test_begin(std::in_place);
	Base::IteratorWrapper<ToyType2Generator2> test_end(std::nullopt);

	int count = 0;
	for (; test_begin != test_end; ++test_begin)
	{
		auto&& value = *test_begin;
		EXPECT_EQ(value._i, 1);
		++count;
	}

	EXPECT_EQ(count, 10);
}

TEST(IteratorWrapper, ToyType2Generator2RangeFor)
{
	Base::RangeWrapper<ToyType2Generator2> range;

	for (auto&& item : range)
	{
		EXPECT_EQ(item._i, 1);
	}
}