#pragma once
#include <unordered_map>

namespace cache
{
	template<typename Key, typename Value>
	class LRU
	{
	private:
		class Node
		{
			std::pair<Key, Value> val;
			Node* next;
			Node* prev;
			Node(Key key, Value value)
			{
				val.first = key;
				val.second = value;

				next = nullptr;
				prev = nullptr;
			}
		};

	public:
		LRU(unsigned capacity);
		~LRU();

	private:
		std::unordered_map<Key, Node*> cacheUMap;
		Node* begin = new Node(-1, -1);
		Node* end   = new Node(-1, -1);
		unsigned capacity;
	};


	template<typename Key, typename Value>
	LRU<Key, Value>::LRU(unsigned capacity)
		: capacity(capacity)
	{ }

	template<typename Key, typename Value>
	LRU<Key, Value>::~LRU()
	{
		delete begin;
		// + other fields it future
		delete end;
	}
}
