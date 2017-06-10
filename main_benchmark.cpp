#include <iostream>
#include <sstream>
#include <string>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <benchmark/benchmark.h>

#include "with_stl.h"
#include "with_boost_intrusive.h"

template <class Cache>
void BM_LRU (benchmark::State& state) 
{
	while (state.KeepRunning())
	{
		boost::random::mt11213b gen (42);
		boost::random::uniform_int_distribution<> dist;

		const auto generator = [&gen, &dist]() {return dist(gen); };

		Cache cache (1000);
		enum {iterations = 1000000};
		for (size_t ii = 0; ii < iterations; ++ii)
		{
			switch (generator() % 2)
			{
				case 0:
					cache.Set (generator() % 4000, generator());
				break;

				case 1:
					cache.Get (generator() % 2000);
				break;
			}
		}

		state.SetItemsProcessed (state.items_processed() + iterations);
	}
}

void BM_Boost(benchmark::State& state)
{
	BM_LRU<LRUCacheWithBoost> (state);
}

BENCHMARK (BM_Boost);

void BM_Stl (benchmark::State& state)
{
	BM_LRU<LRUCacheWithStl> (state);
}

BENCHMARK (BM_Stl);


BENCHMARK_MAIN();