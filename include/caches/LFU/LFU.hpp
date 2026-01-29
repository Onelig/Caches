#pragma once
#include "caches/cache_utils.hpp"
#include <list>
#include <unordered_map>

namespace cache
{
	template<typename Key, typename Value>
	class LFU
	{
	private:
		struct data
		{
			Value value;
			std::size_t freqS;
			typename std::list<Key>::iterator iter;
		};

	public:
		LFU(std::size_t capacity);

		void insert(const Key& key, const Value& value);
		void insert(const Key& key, Value&& value);

	private:
		std::size_t capacity_;
		std::size_t minFreq;

		std::unordered_map<Key, data> mp;
		std::unordered_map<std::size_t, std::list<Key>> freq;
	};


	template<typename Key, typename Value>
	LFU<Key, Value>::LFU(std::size_t capacity)
		: capacity_(capacity), minFreq(0)
	{ }

	template<typename Key, typename Value>
	void LFU<Key, Value>::insert(const Key &key, const Value &value)
	{
		if (capacity_ == 0)
			return;

		auto mpIter = mp.find(key);
		if (mpIter != mp.end())
		{
			mpIter->second.value = value;

			// Update level
			std::size_t oldFreq = mpIter->second.freq;
			std::list<Key>& oldList = freq[oldFreq];

			++mpIter->second.freq;
			std::list<Key>& newList = freq[mpIter->second.freq];

			newList.splice(newList.begin(), oldList, mpIter->second.iter);

			if (oldFreq == minFreq && oldList.empty())
				++minFreq;
		}
		else
		{
			if (capacity_ == mp.size())
			{
				// Remove element with min level
				auto& ListW = freq[minFreq];
				mp.erase(ListW.back());
				ListW.pop_back();
			}

			minFreq = 0;
			auto& ListW = freq[minFreq];
			ListW.push_front(key);
			mp[key] = data{value, minFreq, ListW.begin()};
		}
	}

	template<typename Key, typename Value>
	void LFU<Key, Value>::insert(const Key &key, Value &&value)
	{
		if (capacity_ == 0)
			return;

		auto mpIter = mp.find(key);
		if (mpIter != mp.end())
		{
			mpIter->second.value = std::move(value);

			// Update level
			std::size_t oldFreq = mpIter->second.freq;
			std::list<Key>& oldList = freq[oldFreq];

			++mpIter->second.freq;
			std::list<Key>& newList = freq[mpIter->second.freq];

			newList.splice(newList.begin(), oldList, mpIter->second.iter);

			if (oldFreq == minFreq && oldList.empty())
				++minFreq;
		}
		else
		{
			if (capacity_ == mp.size())
			{
				// Remove element with min level
				auto& ListW = freq[minFreq];
				mp.erase(ListW.back());
				ListW.pop_back();
			}

			minFreq = 0;
			auto& ListW = freq[minFreq];
			ListW.push_front(key);
			mp[key] = data{std::move(value), minFreq, ListW.begin()};
		}
	}
}
