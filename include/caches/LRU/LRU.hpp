#pragma once
#include "caches/cache_utils.hpp"
#include <unordered_map>
#include <map>
#include <mutex>
#include <type_traits>

namespace cache
{
	template<typename Key, typename Value, class LockT = NullLock>
	class LRU
	{
		static_assert(
			has_hash<Key>::value || has_less_comp<Key>::value,
			"Key must be hashable (unordered_map) or less-comparable (map)"
		);

	private: // List
		struct Node
		{
			std::pair<Key, Value> val;
			Node* next;
			Node* prev;

			Node()
				: prev(nullptr), next(nullptr)
			{ }

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

		void eraseFullNode(Node* temp);

		using Guard = std::lock_guard<LockT>;
		using mapT  = std::conditional_t<has_hash<Key>::value,
						std::unordered_map<Key, Node*>,
						std::map<Key, Node*>>;
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
		unsigned size() const;
		unsigned capacity() const;
		bool full() const;

		Value& operator[](const Key& key);
		const Value& operator[](const Key& key) const;

	private:
		LRU(const LRU&) = delete;
		LRU& operator=(const LRU&) = delete;

		mutable LockT lock_;
		mapT cache_;
		Node* begin = new Node();
		Node* end   = new Node();
		unsigned capacity_;
	};


	template<typename Key, typename Value, class lock>
	void LRU<Key, Value, lock>::moveNodeToFront(Node *temp)
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

	template<typename Key, typename Value, class lock>
	void LRU<Key, Value, lock>::insertNode(Node *newNode)
	{
		newNode->prev = begin;
		newNode->next = begin->next;

		begin->next->prev = newNode;
		begin->next		  = newNode;
	}

	template<typename Key, typename Value, class lock>
	void LRU<Key, Value, lock>::deleteNode(Node* nodeToRemove)
	{
		nodeToRemove->prev->next = nodeToRemove->next;
		nodeToRemove->next->prev = nodeToRemove->prev;

		delete(nodeToRemove);
	}

	template<typename Key, typename Value, class LockT>
	void LRU<Key, Value, LockT>::eraseFullNode(Node *temp)
	{
		cache_.erase(temp->val.first);
		deleteNode(temp);
	}

	template<typename Key, typename Value, class lock>
	LRU<Key, Value, lock>::LRU(unsigned capacity_)
		: capacity_(capacity_)
	{
		begin->next = end;
		end->prev   = begin;
	}

	template<typename Key, typename Value, class lock>
	LRU<Key, Value, lock>::~LRU()
	{
		Node* cur = begin;

		while (cur)
		{
			Node* temp = cur;
			cur = cur->next;
			delete temp;
		}
	}

	template<typename Key, typename Value, class lock>
	void LRU<Key, Value, lock>::insert(const Key& key, const Value& value)
	{
		Guard g(lock_);
		if (capacity_ == 0)
			return;

		auto iter = cache_.find(key);

		if (iter != cache_.end())
		{
			moveNodeToFront(iter->second);
			iter->second->val.second = value;
		}
		else
		{
			if (cache_.size() == capacity_)
				eraseFullNode(end->prev);

			Node* node = new Node(key, value);
			insertNode(node);
			cache_[key] = node;
		}
	}

	template<typename Key, typename Value, class lock>
	void LRU<Key, Value, lock>::insert(const Key& key, Value&& value)
	{
		Guard g(lock_);
		if (capacity_ == 0)
			return;

		auto iter = cache_.find(key);

		if (iter != cache_.end())
		{
			moveNodeToFront(iter->second);
			iter->second->val.second = std::move(value);
		}
		else
		{
			if (cache_.size() == capacity_)
				eraseFullNode(end->prev);

			Node* node = new Node(key, std::move(value));
			insertNode(node);
			cache_[key] = node;
		}
	}

	template<typename Key, typename Value, class lock>
	template<class... Args>
	void LRU<Key, Value, lock>::emplace(const Key &key, Args&&... args)
	{
		Guard g(lock_);
		if (capacity_ == 0)
			return;

		auto iter = cache_.find(key);

		if (iter != cache_.end())
		{
			moveNodeToFront(iter->second);
			iter->second->val.second = Value(std::forward<Args>(args)...);
		}
		else
		{
			if (cache_.size() == capacity_)
				eraseFullNode(end->prev);

			Node* node = new Node(key, std::forward<Args>(args)...);
			insertNode(node);
			cache_[key] = node;
		}
	}

	template<typename Key, typename Value, class lock>
	Value& LRU<Key, Value, lock>::get(const Key &key)
	{
		Guard g(lock_);

		auto findIter = cache_.find(key);
		if (findIter == cache_.end())
			throw KeyNotFound();

		Node* nodeTmp = findIter->second;

		moveNodeToFront(nodeTmp);
		return nodeTmp->val.second;
	}

	template<typename Key, typename Value, class lock>
	const Value& LRU<Key, Value, lock>::peek(const Key& key) const
	{
		Guard g(lock_);
		auto findIter = cache_.find(key);
		if (findIter == cache_.end())
			throw KeyNotFound();

		Node* nodeTmp = findIter->second;

		return nodeTmp->val.second;
	}

	template<typename Key, typename Value, class lock>
	bool LRU<Key, Value, lock>::erase(const Key &key)
	{
		Guard g(lock_);
		auto findIter = cache_.find(key);
		if (findIter == cache_.end())
			return false;

		eraseFullNode(findIter->second);
		return true;
	}

	template<typename Key, typename Value, class lock>
	void LRU<Key, Value, lock>::clear()
	{
		Guard g(lock_);
		Node* cur = begin->next;

		while (cur != end)
		{
			Node* temp = cur;
			cur = cur->next;
			delete temp;
		}
		begin->next = end;
		end->prev = begin;

		cache_.clear();
	}

	template<typename Key, typename Value, class lock>
	void LRU<Key, Value, lock>::set_capacity(unsigned newCap)
	{
		Guard g(lock_);
		capacity_ = newCap;

		while (cache_.size() > capacity_)
		{
			Node* node = end->prev;
			cache_.erase(node->val.first);
			deleteNode(node);
		}
	}

	template<typename Key, typename Value, class lock>
	bool LRU<Key, Value, lock>::contains(const Key &key) const
	{
		Guard g(lock_);
		return cache_.find(key) != cache_.end();
	}

	template<typename Key, typename Value, class lock>
	bool LRU<Key, Value, lock>::empty() const
	{
		Guard g(lock_);
		return cache_.empty();
	}

	template<typename Key, typename Value, class lock>
	unsigned LRU<Key, Value, lock>::size() const
	{
		Guard g(lock_);
		return cache_.size();
	}

	template<typename Key, typename Value, class lock>
	unsigned LRU<Key, Value, lock>::capacity() const
	{
		Guard g(lock_);
		return capacity_;
	}

	template<typename Key, typename Value, class lock>
	bool LRU<Key, Value, lock>::full() const
	{
		Guard g(lock_);
		return cache_.size() == capacity_;
	}

	template<typename Key, typename Value, class lock>
	Value& LRU<Key, Value, lock>::operator[](const Key& key)
	{
		return get(key);
	}

	template<typename Key, typename Value, class lock>
	const Value & LRU<Key, Value, lock>::operator[](const Key& key) const
	{
		return peek(key);
	}
}
