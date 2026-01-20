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

			template<class... Args>
			Node(Key key, Args&&... args)
				: val(key, Value(std::forward<Args>(args)...)),
			      prev(nullptr),
				  next(nullptr)
			{ }
		};

		void moveNodeToFront(Node *temp);
		void insertNode(Node* newNode);
		void deleteLastNode();
	public:
		LRU(unsigned capacity);
		~LRU();

		void insert(const Key& key, const Value& value);
		void insert(const Key& key, Value&& value);
		template<class... Args>
		void emplace(const Key& key, Args&&... args);
		bool contains(const Key& key) const;
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
	void LRU<Key, Value>::deleteLastNode()
	{
		Node* nodeToRemove = end->prev;

		nodeToRemove->prev->next = end;
		end->prev = nodeToRemove->prev;

		delete(nodeToRemove);
	}

	template<typename Key, typename Value>
	LRU<Key, Value>::LRU(unsigned capacity)
		: capacity(capacity == 0 ? 1 : capacity)
	{
		begin->next = end;
		end->prev   = begin;
	}

	template<typename Key, typename Value>
	LRU<Key, Value>::~LRU()
	{
		Node* cur = begin;

		while (cur)
		{
			Node* temp = cur;
			cur = cur->next;
			delete temp;
		}
	}

	template<typename Key, typename Value>
	void LRU<Key, Value>::insert(const Key& key, const Value& value)
	{
		auto iter = cacheUMap.find(key);

		if (iter != cacheUMap.end())
		{
			moveNodeToFront(iter->second);
			iter->second->val.second = value;
		}
		else
		{
			if (cacheUMap.size() == capacity)
			{
				cacheUMap.erase(end->prev->val.first);
				deleteLastNode();
			}

			Node* node = new Node(key, value);
			insertNode(node);
			cacheUMap[key] = node;
		}
	}

	template<typename Key, typename Value>
	void LRU<Key, Value>::insert(const Key& key, Value&& value)
	{
		auto iter = cacheUMap.find(key);

		if (iter != cacheUMap.end())
		{
			moveNodeToFront(iter->second);
			iter->second->val.second = std::move(value);
		}
		else
		{
			if (cacheUMap.size() == capacity)
			{
				cacheUMap.erase(end->prev->val.first);
				deleteLastNode();
			}

			Node* node = new Node(key, std::move(value));
			insertNode(node);
			cacheUMap[key] = node;
		}
	}

	template<typename Key, typename Value>
	template<class... Args>
	void LRU<Key, Value>::emplace(const Key &key, Args&&... args)
	{
		auto iter = cacheUMap.find(key);

		if (iter != cacheUMap.end())
		{
			moveNodeToFront(iter->second);
			iter->second->val.second.~Value();
			new (&iter->second->val.second) Value(std::forward<Args>(args)...);
		}
		else
		{
			if (cacheUMap.size() == capacity)
			{
				cacheUMap.erase(end->prev->val.first);
				deleteLastNode();
			}

			Node* node = new Node(key, std::forward<Args>(args)...);
			insertNode(node);
			cacheUMap[key] = node;
		}
	}

	template<typename Key, typename Value>
	bool LRU<Key, Value>::contains(const Key &key) const
	{
		return cacheUMap.find(key) != cacheUMap.end();
	}
}
