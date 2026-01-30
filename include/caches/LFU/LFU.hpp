#pragma once
#include "caches/cache_utils.hpp"
#include <list>
#include <unordered_map>

namespace cache
{
	template<typename Key, typename Value, class LockT = NullLock>
	class LFU
	{
		static_assert(
			has_hash<Key>::value || has_less_comp<Key>::value,
			"Key must be hashable (unordered_map) or less-comparable (map)"
		);

	private:
		struct data
		{
			Value value;
			std::size_t freqS;
			typename std::list<Key>::iterator iter;
		};
		
		using Guard = std::lock_guard<LockT>;
		using mapKeyData = std::conditional_t<has_hash<Key>::value,
								std::unordered_map<Key, data>,
								std::map<Key, data>>;
		
		using mapFreqList = std::conditional_t<has_hash<Key>::value,
								std::unordered_map<std::size_t, std::list<Key>>,
								std::map<std::size_t, std::list<Key>>>;

		void updateLevel(typename mapKeyData::iterator mpIter);
	public:
		LFU(std::size_t capacity);

		void insert(const Key& key, const Value& value);
		void insert(const Key& key, Value&& value);
		template<class... Args>
		void emplace(const Key& key, Args&& ... args);

		Value& get(const Key& key);
		const Value& peek(const Key& key) const;

		bool erase(const Key& key);
		void clear();
		void set_capacity(std::size_t newCap);

		bool contains(const Key& key) const;
		bool empty() const;
		std::size_t size() const;
		std::size_t capacity() const;
		bool full() const;

		Value& operator[](const Key& key);
		const Value& operator[](const Key& key) const;

	private:
		std::size_t capacity_;
		std::size_t minFreq;
		mutable LockT lock_;
		mapKeyData mp;
		mapFreqList freq;
	};


	template<typename Key, typename Value, class lock>
	void LFU<Key, Value, lock>::updateLevel(typename mapKeyData::iterator mpIter)
	{
		// Update level
		std::size_t oldFreq = mpIter->second.freqS;
		std::list<Key>& oldList = freq[oldFreq];

		++mpIter->second.freqS;
		std::list<Key>& newList = freq[mpIter->second.freqS];

		newList.splice(newList.begin(), oldList, mpIter->second.iter);

		if (oldFreq == minFreq && oldList.empty())
		{
			++minFreq;
			freq.erase(oldFreq);
		}
	}

	template<typename Key, typename Value, class lock>
	LFU<Key, Value, lock>::LFU(std::size_t capacity)
		: capacity_(capacity), minFreq(0)
	{ }

	template<typename Key, typename Value, class lock>
	void LFU<Key, Value, lock>::insert(const Key& key, const Value& value)
	{
		Guard g(lock_);
		if (capacity_ == 0)
			return;

		auto mpIter = mp.find(key);
		if (mpIter != mp.end())
		{
			mpIter->second.value = value;
			updateLevel(mpIter);
		}
		else
		{
			if (capacity_ == mp.size())
			{
				// Remove element with min level
				std::list<Key>& ListW = freq[minFreq];
				mp.erase(ListW.back());
				ListW.pop_back();

				if (ListW.empty())
					freq.erase(minFreq);
			}

			minFreq = 0;
			std::list<Key>& ListW = freq[minFreq];
			ListW.push_front(key);
			mp[key] = data{value, minFreq, ListW.begin()};
		}
	}

	template<typename Key, typename Value, class lock>
	void LFU<Key, Value, lock>::insert(const Key& key, Value&& value)
	{
		Guard g(lock_);
		if (capacity_ == 0)
			return;

		auto mpIter = mp.find(key);
		if (mpIter != mp.end())
		{
			mpIter->second.value = std::move(value);
			updateLevel(mpIter);
		}
		else
		{
			if (capacity_ == mp.size())
			{
				// Remove element with min level
				auto& ListW = freq[minFreq];
				mp.erase(ListW.back());
				ListW.pop_back();

				if (ListW.empty())
					freq.erase(minFreq);
			}

			minFreq = 0;
			auto& ListW = freq[minFreq];
			ListW.push_front(key);
			mp[key] = data{std::move(value), minFreq, ListW.begin()};
		}
	}

	template<typename Key, typename Value, class lock>
	template<class... Args>
	void LFU<Key, Value, lock>::emplace(const Key& key, Args&& ... args)
	{
		Guard g(lock_);
		if (capacity_ == 0)
			return;

		auto mpIter = mp.find(key);
		if (mpIter != mp.end())
		{
			mpIter->second.value = Value(std::forward<Args>(args)...);
			updateLevel(mpIter);
		}
		else
		{
			if (capacity_ == mp.size())
			{
				// Remove element with min level
				std::list<Key>& ListW = freq[minFreq];
				mp.erase(ListW.back());
				ListW.pop_back();

				if (ListW.empty())
					freq.erase(minFreq);
			}

			minFreq = 0;
			std::list<Key>& ListW = freq[minFreq];
			ListW.push_front(key);
			mp[key] = data{Value(std::forward<Args>(args)...), minFreq, ListW.begin()};
		}
	}

	template<typename Key, typename Value, class lock>
	Value& LFU<Key, Value, lock>::get(const Key& key)
	{
		Guard g(lock_);
		auto mpIter = mp.find(key);
		if (mpIter == mp.end())
			throw KeyNotFound();

		updateLevel(mpIter);

		return mpIter->second.value;
	}

	template<typename Key, typename Value, class lock>
	const Value& LFU<Key, Value, lock>::peek(const Key& key) const
	{
		Guard g(lock_);
		auto mpIter = mp.find(key);
		if (mpIter == mp.end())
			throw KeyNotFound();

		return mpIter->second.value;
	}

	template<typename Key, typename Value, class lock>
	bool LFU<Key, Value, lock>::erase(const Key& key)
	{
		Guard g(lock_);
		auto mpIter = mp.find(key);
		if (mpIter == mp.end())
			return false;

		std::size_t     oldFreq = mpIter->second.freqS;
		std::list<Key>& oldList = freq[oldFreq];

		oldList.erase(mpIter->second.iter);
		mp.erase(mpIter);

		// update lvl if needed
		if (oldFreq == minFreq && freq[oldFreq].empty())
		{
			freq.erase(oldFreq);

			if (!mp.empty())
			{
				minFreq = freq.begin()->first;
				for (auto& p : freq)
				{
					std::size_t f = p.first;
					if (f < minFreq) minFreq = f;
				}
			}
		}

		return true;
	}

	template<typename Key, typename Value, class lock>
	void LFU<Key, Value, lock>::clear()
	{
		Guard g(lock_);
		mp.clear();
		freq.clear();
		minFreq = 0;
	}

	template<typename Key, typename Value, class lock>
	void LFU<Key, Value, lock>::set_capacity(std::size_t newCap)
	{
		Guard g(lock_);
		capacity_ = newCap;

		// Remove element if actual capacity less previous
		while (mp.size() > capacity_)
		{
			std::list<Key>& ListW = freq[minFreq];

			while (!ListW.empty() && mp.size() > capacity_)
			{
				mp.erase(ListW.back());
				ListW.pop_back();
			}

			if (ListW.empty())
			{
				freq.erase(minFreq);

				if (!mp.empty())
				{
					minFreq = freq.begin()->first;
					for (auto& p : freq)
					{
						std::size_t f = p.first;
						if (f < minFreq) minFreq = f;
					}
				}
			}

		}
	}

	template<typename Key, typename Value, class lock>
	bool LFU<Key, Value, lock>::contains(const Key &key) const
	{
		Guard g(lock_);
		return mp.find(key) != mp.end();
	}

	template<typename Key, typename Value, class lock>
	bool LFU<Key, Value, lock>::empty() const
	{
		Guard g(lock_);
		return mp.empty();
	}

	template<typename Key, typename Value, class lock>
	std::size_t LFU<Key, Value, lock>::size() const
	{
		Guard g(lock_);
		return mp.size();
	}

	template<typename Key, typename Value, class lock>
	std::size_t LFU<Key, Value, lock>::capacity() const
	{
		Guard g(lock_);
		return capacity_;
	}

	template<typename Key, typename Value, class lock>
	bool LFU<Key, Value, lock>::full() const
	{
		Guard g(lock_);
		return capacity_ == mp.size();
	}

	template<typename Key, typename Value, class lock>
	Value& LFU<Key, Value, lock>::operator[](const Key &key)
	{
		return get(key);
	}

	template<typename Key, typename Value, class lock>
	const Value& LFU<Key, Value, lock>::operator[](const Key &key) const
	{
		return peek(key);
	}
}
