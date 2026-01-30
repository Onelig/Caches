#include <gtest/gtest.h>
#include <caches/LFU/LFU.hpp>

TEST(LFU_SFINAE, HashableKey)
{
	static_assert(cache::has_hash<int>::value);
	cache::LFU<int, int> cds(2);
}

TEST(LFU_SFINAE, NonHashableKey)
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
	cache::LFU<NoHash, int> cds(2);

	cds.insert(NoHash{2}, 312);
	cds.insert(NoHash{6}, 13);
	cds.insert(NoHash{94}, 13);
	EXPECT_FALSE(cds.contains(NoHash{2}));
}
