#include <gtest/gtest.h>
#include <caches/LFU/LFU.hpp>
#include <vector>

TEST(LFU_Contains, Get)
{
	cache::LFU<int, std::vector<int>> cache(4);

	cache.insert(2, std::vector<int>{ 2, 4, 6, 1, 6 });
	cache.insert(1, std::vector<int>{ 4, 44 });
	cache.insert(8, std::vector<int>{ 3, 13, 66, 1 });
	cache.insert(7, std::vector<int>{ 321, 13, 66, 1 });
	EXPECT_NO_THROW(cache.get(2));
	cache.insert(3, std::vector<int>{ 9134, 13, 66, 1 });

	EXPECT_THROW(cache.get(1), cache::KeyNotFound);
	EXPECT_NO_THROW(cache.get(8));

	cache.insert(10, std::vector<int>{ 91, 6, 1 });

	EXPECT_THROW(cache.get(7), cache::KeyNotFound);
	EXPECT_EQ(cache.get(10), (std::vector<int>{ 91, 6, 1 }));
}

TEST(LFU_Contains, Priority)
{
	cache::LFU<int, short> cache(3);

	cache.insert(1, 41);
	EXPECT_NO_THROW(cache.get(1));
	EXPECT_NO_THROW(cache.get(1));
	EXPECT_NO_THROW(cache.get(1));
	// Prior should equal 3

	cache.insert(2, 51);
	EXPECT_NO_THROW(cache.get(2));
	EXPECT_NO_THROW(cache.get(2));
	EXPECT_NO_THROW(cache.get(2));
	// Prior should equal 3

	cache.insert(4, 1091);
	EXPECT_NO_THROW(cache.get(4));
	EXPECT_NO_THROW(cache.get(4));
	EXPECT_NO_THROW(cache.get(4));
	// Prior should equal 3

	cache.insert(8, 1);

	EXPECT_THROW(cache.get(1), cache::KeyNotFound);
}

TEST(LFU_Contains, Peek)
{
	cache::LFU<int, std::vector<int>> cache(4);

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

TEST(LFU_Contains, Contains)
{
	cache::LFU<int, std::vector<int>> cache(4);

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

TEST(LFU_Contains, Empty)
{
	cache::LFU<std::string, std::string> cache(2);

	cache.insert("s", "Hello World");
	cache.insert("ASD", "Something");

	EXPECT_FALSE(cache.empty());
	cache.clear();
	EXPECT_TRUE(cache.empty());
}

TEST(LFU_Contains, ZeroSize)
{
	cache::LFU<std::string, std::string> cache(0);

	cache.insert("Sd", "ddd");
	cache.emplace("asd", 2, 's');

	EXPECT_TRUE(cache.empty());
	EXPECT_TRUE(cache.full());

	cache.set_capacity(2);
	cache.insert("Sd", "ddd");
	cache.emplace("asd", 2, 's');

	cache.set_capacity(0);

	EXPECT_TRUE(cache.empty());
}