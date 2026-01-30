#include <gtest/gtest.h>
#include <caches/LFU/LFU.hpp>
#include <string>

TEST(LFU_Capacity, Full)
{
	cache::LFU<int, int> cache(1);

	EXPECT_FALSE(cache.full());

	cache.insert(1, 10);
	EXPECT_TRUE(cache.full());

	cache.clear();
	EXPECT_FALSE(cache.full());
	cache.clear();
	EXPECT_FALSE(cache.full());
}

TEST(LFU_Capacity, Insert)
{
	cache::LFU<int, int> cache(3);

	cache.insert(1, 10);
	cache.insert(1, 10);
	EXPECT_EQ(cache.size(), 1);

	int b = 1392;
	cache.insert(13, 3);
	cache.insert(13, b);
	EXPECT_EQ(cache.size(), 2);

	cache.insert(31, 3);
	cache.insert(3145, 1);
	EXPECT_TRUE(cache.full());
}

TEST(LFU_Capacity, Emplace)
{
	cache::LFU<int, std::pair<int, int>> cache(3);

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

TEST(LFU_Capacity, Erase)
{
	cache::LFU<int, std::string> cache(2);

	cache.insert(8, "Hello World");
	cache.insert(8, "Hello World");
	cache.insert(5, "Something");
	cache.insert(1, "LFU");

	EXPECT_TRUE(cache.erase(8));
	EXPECT_TRUE(cache.erase(1));
	EXPECT_FALSE(cache.erase(5));
}

TEST(LFU_Capacity, ChangeCapacity)
{
	cache::LFU<int, std::string> cache(2);

	cache.insert(8, "Hello World");
	cache.insert(5, "Something");
	cache.insert(5, "Something");

	cache.set_capacity(4);

	cache.insert(9, "Hello World1");
	cache.insert(1, "Something1");

	EXPECT_EQ(cache.size(), 4);
	EXPECT_TRUE(cache.full());

	cache.set_capacity(2);

	EXPECT_EQ(cache.size(), 2);
	EXPECT_TRUE(cache.full());

	cache.set_capacity(0);
	cache.set_capacity(0);
	EXPECT_TRUE(cache.full());
	EXPECT_EQ(cache.size(), 0);
}

TEST(LFU_Capacity, CheckPrior)
{
	cache::LFU<int, std::string> cache(2);

	cache.insert(5, "Something");
	cache.insert(5, "Something");
	cache.insert(5, "Something");
	cache.insert(5, "Something");

	EXPECT_EQ(cache.size(), 1);
	cache.insert(9, "Hello World1");

	EXPECT_EQ(cache.size(), 2);
	cache.insert(94, "Hello World1");
	EXPECT_ANY_THROW(cache.get(9));
}
