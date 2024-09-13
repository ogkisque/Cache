#pragma once

#include <iostream>
#include <cassert>
#include <vector>

#include "cache.hpp"
#include "slow_get_page.hpp"

void use_perfect();

namespace cache
{
    template <typename T, typename F, typename KeyT = int>
    class Perfect : public Cache<T, F, KeyT, std::pair<KeyT, T>>
    {
        using ListElemT = typename std::pair<KeyT, T>;
        using cache::Cache<T, F, KeyT, ListElemT>::is_full;
        using cache::Cache<T, F, KeyT, ListElemT>::hash_map_;
        using cache::Cache<T, F, KeyT, ListElemT>::cache_;

        std::vector<KeyT> requests_;

        KeyT find_last()
        {
            std::list<KeyT> tmp_list;
                    
            for (auto it = cache_.begin(); it != cache_.end(); it = std::next(it))
                tmp_list.push_back(it->first);

            auto request_it = requests_.begin();
            auto it = tmp_list.begin();
            
            while (tmp_list.size() > 1 && request_it != requests_.end())
            {
                for (it = tmp_list.begin(); it != tmp_list.end(); it = std::next(it))
                    if (*it == *request_it)
                    {
                        tmp_list.erase(it);
                        break;
                    }
                
                request_it = std::next(request_it);
            }

            return *it;
        }

public:
        Perfect(size_t size, std::vector<KeyT> requests) : Cache<T, F, ListElemT, KeyT>(size),
                                                           requests_(requests) {}

        bool find_update(KeyT key, F slow_get_page) override
        {
            auto hit = hash_map_.find(key);
            
            if (hit == hash_map_.end())
            {
                if (is_full())
                {
                    KeyT key = find_last();
                    auto del_it = hash_map_.find(key);
                    hash_map_.erase(del_it);
                    cache_.erase(del_it.second);
                }
                
                cache_.emplace_front(key, slow_get_page(key));
                hash_map_.emplace(key, cache_.begin());
            }
        }
    };

} // namespace cache