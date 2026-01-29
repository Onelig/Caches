#pragma once
#include <stdexcept>
#include <functional>

namespace cache
{
	class KeyNotFound : public std::invalid_argument
	{
	public:
		KeyNotFound()
			: std::invalid_argument("Key not found")
		{ }
	};

	class NullLock
	{
	public:
		void lock() { }
		void unlock() { }
		bool try_lock() { return true; }
	};

	template<typename T, typename = void>
	struct has_hash : std::false_type
	{ };

	template<typename T>
	struct has_hash<T, decltype(void(std::declval<std::hash<T>>()(std::declval<T>())))> : std::true_type
	{ };

	template<typename T, typename = void>
	struct has_less_comp : std::false_type
	{ };

	template<typename T>
	struct has_less_comp<T, decltype(void(std::declval<T&>() < std::declval<T&>()))> : std::true_type
	{ };
}