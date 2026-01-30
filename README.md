[![C++14](https://img.shields.io/badge/C%2B%2B-14-00599C?style=flat-square&logo=c%2B%2B)](https://en.cppreference.com/w/cpp/14) [![Language](https://img.shields.io/badge/lang-English🇬🇧-white)](README.md) [![Язык](https://img.shields.io/badge/язык-Русский🇷🇺-white)](README-RU.md) [![GitHub](https://img.shields.io/badge/github-gray)](https://github.com/Onelig) [![License](https://img.shields.io/badge/license-MIT-blue)](LICENSE)
# Caches

Cache System is a C++14 project implementing high-performance key-value caches. One of them is an **LRU** and **LFU** cache.

---
# LRU Cache — Least Recently Used Cache (C++14)
LRU Cache is a least recently used cache that stores key-value pairs and automatically removes the least recently accessed elements when the cache reaches its capacity.

# LFU Cache — Least Frequently Used Cache (C++14)
LFU Cache stores key-value pairs and automatically removes the least frequently used elements when the cache reaches its capacity. Elements with higher access frequency remain in the cache longer, while new or rarely accessed elements are removed first.

---
The caches is implemented in C++14 using a doubly linked list for ordering and either ```std::unordered_map``` or ```std::map``` for fast lookups:
- If the keys are hashable — ```std::unordered_map``` for **O(1)** access.
- If the keys are not hashable — ```std::map``` for **O(log N)** access.

## Features
- Insert elements by copy, move, or emplace (construct in-place).
- Access elements either moving them to the front (```get```) or without changing their position (```peek```).
- Manual removal (```erase```) and clearing of the cache (```clear```).
- Dynamic resizing of capacity (```set_capacity```).
- Status checks: ```contains```, ```empty```, ```full```, ```size```, ```capacity```.
- Throws exceptions when accessing a non-existent key (```KeyNotFound```).

## Technologies and Approach
- C++14, templates, manual memory management.
- Own doubly linked list + map for maximum performance and flexibility.
- Choice of map depends on hash availability:
  - Hashable → ```std::unordered_map```
  - Non-hashable → ```std::map```

## Build and Test
```console
mkdir build && cd build
cmake ..
cmake --build .
ctest
```
