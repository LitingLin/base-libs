//
// Created by liting on 19-7-15.
//
#include "pch.h"

#include <base/random.hpp>
#include <iostream>

TEST(RANDOM, A)
{
    Base::RNG rng;
    std::cout << rng.get<double>() << std::endl;
    std::cout << rng.get<int>() << std::endl;
}