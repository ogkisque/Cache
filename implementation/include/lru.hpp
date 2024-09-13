#pragma once

#include <iostream>
#include <cassert>

#include "cache.hpp"
#include "slow_get_page.hpp"

void use_lru();

namespace cache
{
    template <typename T, typename F, typename KeyT = int>
    class LRU : public Cache<T, F, std::pair<KeyT, T>, KeyT>
    {
        using ListElemT = typename std::pair<KeyT, T>;
        using cache::Cache<T, F, ListElemT>::Cache;
        using cache::Cache<T, F, ListElemT>::is_full;
        using cache::Cache<T, F, ListElemT>::hash_map_;
        using cache::Cache<T, F, ListElemT>::cache_;

public:
        bool find_update(KeyT key, F slow_get_page) override
        {
            auto hit = hash_map_.find(key);

            if (hit == hash_map_.end())
            {
                if (is_full())
                {
                    hash_map_.erase(cache_.back().first);
                    cache_.pop_back();
                }

                cache_.emplace_front(key, slow_get_page(key));
                hash_map_.emplace(key, cache_.begin());

                return false;
            }

            auto elem = hit->second;
            if (elem != cache_.begin())
                cache_.splice(cache_.begin(), cache_, elem, std::next(elem));
            
            return true;
        }
    };

} // namespace cache