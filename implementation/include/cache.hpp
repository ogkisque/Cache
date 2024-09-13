#pragma once

#include <list>
#include <unordered_map>
#include <iterator>

namespace cache
{
    using CountT = size_t;

    template <typename T, typename F, typename ListElemT, typename KeyT = int>
    class Cache
    {
protected:
        using ListIter = typename std::list<ListElemT>::iterator;
        size_t size_;
        std::list<ListElemT> cache_;
        std::unordered_map<KeyT, ListIter> hash_map_;

        bool is_full()
        {
            return cache_.size() == size_;
        }

public:
        Cache(size_t size) : size_(size) {}

        virtual bool find_update(KeyT key, F slow_get_page) = 0;
    };

} // namespace cache
