#include <gtest/gtest.h>
#include <caches/LRU/LRU.hpp>
#include <string>

TEST(LRU_Capacity, Full)
{
	cache::LRU<int, int> cache(1);

	EXPECT_FALSE(cache.full());

	cache.insert(1, 10);
	EXPECT_TRUE(cache.full());

	cache.clear();
	EXPECT_FALSE(cache.full());
	cache.clear();
	EXPECT_FALSE(cache.full());
}

TEST(LRU_Capacity, Insert)
{
	cache::LRU<int, int> cache(3);

	cache.insert(1, 10);
	cache.insert(1, 10);
	EXPECT_EQ(cache.size(), 1);

	int b = 1392;
	cache.insert(13, 3);
	cache.insert(13, b);
	EXPECT_EQ(cache.size(), 2);

	cache.insert(31, 3);
	cache.insert(31, 3);
	EXPECT_TRUE(cache.full());
}

TEST(LRU_Capacity, Emplace)
{
	cache::LRU<int, std::pair<int, int>> cache(3);

	cache.emplace(1, 10, 3);
	cache.insert(1, std::pair<int, int>(213, 321));
	EXPECT_EQ(cache.size(), 1);

	int b = 1392;
	cache.emplace(6, 10, 3);
	cache.emplace(b, 10, 3);
	EXPECT_EQ(cache.size(), 3);

	cache.emplace(31, 3, 1);
	cache.emplace(91, 3, 3);
	EXPECT_EQ(cache.size(), 3);
	EXPECT_TRUE(cache.full());
}

TEST(LRU_Capacity, Erase)
{
	cache::LRU<int, std::string> cache(2);

	cache.insert(8, "Hello World");
	cache.insert(5, "Something");
	cache.insert(1, "LRU");

	EXPECT_EQ(cache.erase(8), false);
	EXPECT_EQ(cache.erase(1), true);
	EXPECT_EQ(cache.erase(5), true);
}

TEST(LRU_Capacity, ChangeCapacity)
{
	cache::LRU<int, std::string> cache(2);

	cache.insert(8, "Hello World");
	cache.insert(5, "Something");

	cache.set_capacity(4);

	cache.insert(9, "Hello World1");
	cache.insert(1, "Something1");

	EXPECT_EQ(cache.size(), 4);
	EXPECT_EQ(cache.full(), true);

	cache.set_capacity(2);

	EXPECT_EQ(cache.size(), 2);
	EXPECT_EQ(cache.full(), true);
}

TEST(LRU_Capacity, Empty)
{
	cache::LRU<int, std::string> cache(2);

	cache.insert(8, "Hello World");
	cache.insert(5, "Something");

	EXPECT_EQ(cache.empty(), false);
	cache.clear();
	EXPECT_EQ(cache.empty(), true);
}

