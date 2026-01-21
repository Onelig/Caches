#include <gtest/gtest.h>
#include <caches/LRU/LRU.hpp>

TEST(LRU_SFINAE, HashableKey)
{
	static_assert(cache::has_hash<int>::value);
	cache::LRU<int, int> cds(2);
}

TEST(LRU_SFINAE, NonHashableKey)
{
	struct NoHash
	{
		int x;
		bool operator<(const NoHash& other) const
		{
			return x < other.x;
		}
	};

	static_assert(!cache::has_hash<NoHash>::value);
	cache::LRU<NoHash, int> cds(2);

	cds.insert(NoHash{2}, 312);
	cds.insert(NoHash{6}, 13);
	cds.insert(NoHash{94}, 13);
	EXPECT_FALSE(cds.contains(NoHash{2}));
}

