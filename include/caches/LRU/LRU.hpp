#pragma once
#include <unordered_map>

namespace cache
{
	template<typename Key, typename Value>
	class LRU
	{
	private: // List
		struct Node
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

		void moveNodeToFront(Node *temp);
		void insertNode(Node* newNode);
	public:
		LRU(unsigned capacity);
		~LRU();

		void insert(const Key& key, const Value& value);
		void insert(const Key& key, Value&& v);
		// add emplace
	private:
		LRU(const LRU&) = delete;
		LRU& operator=(const LRU&) = delete;

		std::unordered_map<Key, Node*> cacheUMap;
		Node* begin = new Node(-1, -1);
		Node* end   = new Node(-1, -1);
		unsigned capacity;
	};


	template<typename Key, typename Value>
	void LRU<Key, Value>::moveNodeToFront(Node *temp)
	{
		if (temp->prev == begin)
			return;

		Node* nextNode = temp->next;
		Node* prevNode = temp->prev;

		prevNode->next = nextNode;
		nextNode->prev = prevNode;

		temp->next  = begin->next;
		begin->next->prev = temp;
		temp->prev  = begin;
		begin->next = temp;
	}

	template<typename Key, typename Value>
	void LRU<Key, Value>::insertNode(Node *newNode)
	{
		newNode->prev = begin;
		newNode->next = begin->next;

		begin->next->prev = newNode;
		begin->next		  = newNode;
	}

	template<typename Key, typename Value>
	LRU<Key, Value>::LRU(unsigned capacity)
		: capacity(capacity)
	{
		begin->next = end;
		end->prev   = begin;
	}

	template<typename Key, typename Value>
	LRU<Key, Value>::~LRU()
	{
		delete begin;
		// + other fields it future
		delete end;
	}

	template<typename Key, typename Value>
	void LRU<Key, Value>::insert(const Key& key, const Value& value)
	{
		auto iter = cacheUMap.find(key);

		if (iter != cacheUMap.end())
		{
			iter->second->val = value;
			moveNodeToFront(iter->second);
		}
		else
		{
			if (cacheUMap.size() == capacity)
			{}
			//	deleteLastNode();

			Node* node = new Node(key, value);
			insertNode(node);
		}
	}

	template<typename Key, typename Value>
	void LRU<Key, Value>::insert(const Key& key, Value&& v)
	{
		//  iter->second->val = std::move(value);
	}
}
