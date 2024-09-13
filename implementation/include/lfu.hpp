#pragma once

#include <iostream>
#include <cassert>

#include "cache.hpp"
#include "slow_get_page.hpp"

void use_lfu();

namespace cache
{
    template <typename T, typename F, typename KeyT = int>
    class LFU : public Cache<T, F, std::tuple<KeyT, CountT, T>, KeyT>
    {
        using ListElemT = typename std::tuple<KeyT, CountT, T>;
        using ListIter = typename std::list<ListElemT>::iterator;
        using cache::Cache<T, F, ListElemT>::Cache;
        using cache::Cache<T, F, ListElemT>::is_full;
        using cache::Cache<T, F, ListElemT>::hash_map_;
        using cache::Cache<T, F, ListElemT>::cache_;

        KeyT get_key(ListIter iter)
        {
            return std::get<0>(*iter);
        }

        CountT& get_count(ListIter iter)
        {
            return std::get<1>(*iter);
        }

        T get_data(ListIter iter)
        {
            return std::get<2>(*iter);
        }

public:

        bool find_update(KeyT key, F slow_get_page) override
        {
            auto hit = hash_map_.find(key);

            if (hit == hash_map_.end())
            {
                if (is_full())
                {
                    hash_map_.erase(get_key(cache_.begin()));
                    cache_.pop_front();
                }

                cache_.emplace_front(key, 1, slow_get_page(key));
                hash_map_.emplace(key, cache_.begin());

                return false;
            }

            auto list_it = hit->second;
            size_t cur_count = ++get_count(list_it);
            auto next_it = std::next(list_it);
            
            if (next_it != cache_.begin() && next_it != cache_.end())
            {
                size_t next_count = get_count(next_it);

                if (cur_count > next_count)
                    cache_.splice(list_it, cache_, next_it, std::next(next_it));
            }
            return true;
        }

        friend std::ostream& operator <<(std::ostream& cout, const LFU& cache)
        {
            std::cout << "LFU cache dump:\n";
            for (auto it = cache.cache_.begin(); it != cache.cache_.end(); it = std::next(it))
                cout << "data: " << get_data(it) << " count: " << get_count(it) << '\n'; 
            return cout; 
        } 
    };

} // namespace cache