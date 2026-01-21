#include <gtest/gtest.h>
#include <caches/LRU/LRU.hpp>
#include <vector>

TEST(LRU_Contains, Get)
{
	cache::LRU<int, std::vector<int>> cache(4);

	cache.insert(2, std::vector<int>{ 2, 4, 6, 1, 6 });
	cache.insert(1, std::vector<int>{ 4, 44 });
	cache.insert(8, std::vector<int>{ 3, 13, 66, 1 });
	cache.insert(7, std::vector<int>{ 321, 13, 66, 1 });
	cache.insert(3, std::vector<int>{ 9134, 13, 66, 1 });

	EXPECT_THROW(cache.get(2), cache::KeyNotFound);
	EXPECT_NO_THROW(cache.get(1));

	cache.insert(2, std::vector<int>{ 91, 6, 1 });

	EXPECT_THROW(cache.get(8), cache::KeyNotFound);
	EXPECT_EQ(cache.get(2), (std::vector<int>{ 91, 6, 1 }));
}

TEST(LRU_Contains, Peek)
{
	cache::LRU<int, std::vector<int>> cache(4);

	cache.insert(2, std::vector<int>{ 2, 4, 6, 1, 6 });
	cache.insert(1, std::vector<int>{ 4, 44 });
	cache.insert(8, std::vector<int>{ 3, 13, 66, 1 });
	cache.insert(7, std::vector<int>{ 321, 13, 66, 1 });
	cache.insert(3, std::vector<int>{ 9134, 13, 66, 1 });

	EXPECT_THROW(cache.get(2), cache::KeyNotFound);
	EXPECT_NO_THROW(cache.peek(1));

	cache.insert(2, std::vector<int>{ 91, 6, 1 });

	EXPECT_EQ(cache.get(8), (std::vector<int>{ 3, 13, 66, 1 }));
	EXPECT_THROW(cache.get(1), cache::KeyNotFound);
}

TEST(LRU_Contains, Contains)
{
	cache::LRU<int, std::vector<int>> cache(4);

	cache.insert(2, std::vector<int>{ 2, 4, 6, 1, 6 });
	cache.insert(1, std::vector<int>{ 4, 44 });
	cache.insert(8, std::vector<int>{ 3, 13, 66, 1 });
	cache.insert(7, std::vector<int>{ 321, 13, 66, 1 });
	cache.insert(3, std::vector<int>{ 9134, 13, 66, 1 });

	EXPECT_FALSE(cache.contains(2));
	EXPECT_TRUE(cache.contains(1));

	cache.insert(2, std::vector<int>{ 91, 6, 1 });
	cache.insert(12, std::vector<int>{ 91, 6, 1 });
	cache.insert(4, std::vector<int>{ 91, 6, 1 });

	EXPECT_FALSE(cache.contains(8));
	EXPECT_FALSE(cache.contains(1));
	EXPECT_TRUE(cache.contains(3));
}

TEST(LRU_Contains, Empty)
{
	cache::LRU<int, std::string> cache(2);

	cache.insert(8, "Hello World");
	cache.insert(5, "Something");

	EXPECT_FALSE(cache.empty());
	cache.clear();
	EXPECT_TRUE(cache.empty());
}