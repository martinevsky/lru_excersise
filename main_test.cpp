#include <gtest/gtest.h>

#include "with_stl.h"
#include "with_boost_intrusive.h"

/*
LRUCache cache = new LRUCache( 2  );

cache.put(1, 1);
cache.put(2, 2);
cache.get(1);       // returns 1
cache.put(3, 3);    // evicts key 2
cache.get(2);       // returns -1 (not found)
cache.put(4, 4);    // evicts key 1
cache.get(1);       // returns -1 (not found)
cache.get(3);       // returns 3
cache.get(4);       // returns 4

*/

template <typename T>
class LruTest : public ::testing::Test 
{  
};

TYPED_TEST_CASE_P (LruTest);

TYPED_TEST_P (LruTest, Basic) 
{
    TypeParam cache (2);

    //©leetcode
    cache.Set (1, 1);
    cache.Set (2, 2);
    EXPECT_EQ (1, cache.Get (1));

    cache.Set (3, 3);    // evicts key 2
    EXPECT_EQ (-1, cache.Get (2)); 

    cache.Set (4, 4);    // evicts key 1
    EXPECT_EQ (-1, cache.Get (1));
    EXPECT_EQ (3, cache.Get (3));
    EXPECT_EQ (4, cache.Get (4));
}

REGISTER_TYPED_TEST_CASE_P (LruTest, Basic);

typedef ::testing::Types<LRUCacheWithStl, LRUCacheWithBoost> LruImpls;
INSTANTIATE_TYPED_TEST_CASE_P (My, LruTest, LruImpls);