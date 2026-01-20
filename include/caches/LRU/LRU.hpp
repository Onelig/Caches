#pragma once
#include <stdexcept>
#include <unordered_map>

namespace cache
{
	class KeyNotFound : public std::invalid_argument
	{
	public:
		KeyNotFound()
			: std::invalid_argument("Key not found")
		{ }
	};

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
		void deleteNode(Node* nodeToRemove);

	public:
		LRU(unsigned capacity_);
		~LRU();

		void insert(const Key& key, const Value& value);
		void insert(const Key& key, Value&& value);
		template<class... Args>
		void emplace(const Key& key, Args&&... args);

		Value& get(const Key& key);
		const Value& peek(const Key& key) const;

		bool erase(const Key& key);
		void clear();
		void set_capacity(unsigned newCap);

		bool contains(const Key& key) const;
		bool empty() const;
		bool size() const;
		unsigned capacity() const;
		bool full() const;
	private:
		LRU(const LRU&) = delete;
		LRU& operator=(const LRU&) = delete;

		std::unordered_map<Key, Node*> cacheUMap;
		Node* begin = new Node(-1, -1);
		Node* end   = new Node(-1, -1);
		unsigned capacity_;
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
	void LRU<Key, Value>::deleteNode(Node* nodeToRemove)
	{
		nodeToRemove->prev->next = nodeToRemove->next;
		nodeToRemove->next->prev = nodeToRemove->prev;

		delete(nodeToRemove);
	}

	template<typename Key, typename Value>
	LRU<Key, Value>::LRU(unsigned capacity_)
		: capacity_(capacity_ == 0 ? 1 : capacity_)
	{
		begin->next = end;
		end->prev   = begin;
	}

	template<typename Key, typename Value>
	LRU<Key, Value>::~LRU()
	{
		clear();
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
			if (cacheUMap.size() == capacity_)
			{
				cacheUMap.erase(end->prev->val.first);
				deleteNode(end->prev);
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
			if (cacheUMap.size() == capacity_)
			{
				cacheUMap.erase(end->prev->val.first);
				deleteNode(end->prev);
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
			if (cacheUMap.size() == capacity_)
			{
				cacheUMap.erase(end->prev->val.first);
				deleteNode(end->prev);
			}

			Node* node = new Node(key, std::forward<Args>(args)...);
			insertNode(node);
			cacheUMap[key] = node;
		}
	}

	template<typename Key, typename Value>
	Value& LRU<Key, Value>::get(const Key &key)
	{
		auto findIter = cacheUMap.find(key);
		if (findIter == cacheUMap.end())
			throw KeyNotFound();

		Node* nodeTmp = findIter->second;

		moveNodeToFront(nodeTmp);
		return nodeTmp->val;
	}

	template<typename Key, typename Value>
	const Value& LRU<Key, Value>::peek(const Key& key) const
	{
		auto findIter = cacheUMap.find(key);
		if (findIter == cacheUMap.end())
			throw KeyNotFound();

		Node* nodeTmp = findIter->second;

		return nodeTmp->val;
	}

	template<typename Key, typename Value>
	bool LRU<Key, Value>::erase(const Key &key)
	{
		auto findIter = cacheUMap.find(key);
		if (findIter == cacheUMap.end())
			return false;

		deleteNode(findIter->second);
		cacheUMap.erase(findIter);
		return true;
	}

	template<typename Key, typename Value>
	void LRU<Key, Value>::clear()
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
	void LRU<Key, Value>::set_capacity(unsigned newCap)
	{
		capacity_ = (newCap == 0 ? 1 : newCap);

		while (cacheUMap.size() > capacity_)
		{
			Node* node = end->prev;
			cacheUMap.erase(node->val.first);
			deleteNode(node);
		}
	}

	template<typename Key, typename Value>
	bool LRU<Key, Value>::contains(const Key &key) const
	{
		return cacheUMap.contains(key);
	}

	template<typename Key, typename Value>
	bool LRU<Key, Value>::empty() const
	{
		return cacheUMap.empty();
	}

	template<typename Key, typename Value>
	bool LRU<Key, Value>::size() const
	{
		return cacheUMap.size();
	}

	template<typename Key, typename Value>
	unsigned LRU<Key, Value>::capacity() const
	{
		return capacity_;
	}

	template<typename Key, typename Value>
	bool LRU<Key, Value>::full() const
	{
		return size() == capacity();
	}
}
