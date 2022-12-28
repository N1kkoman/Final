#include "Random.hpp"
#include <random>

unsigned __int64 GetRandomNumber()
{
	std::random_device rd;
	std::mt19937_64 gen(rd());
	return gen();
}