#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <unordered_map>
#include <list>

#include "cache.hpp"
#include "slow_get_page.hpp"

void use_perfect();

namespace cache
{
    template <typename T, typename F, typename KeyT = int>
    class Perfect : public Cache<T, F, std::pair<KeyT, T>, KeyT>
    {
        using ListElemT = typename std::pair<KeyT, T>;
        using cache::Cache<T, F, ListElemT, KeyT>::is_full;
        using cache::Cache<T, F, ListElemT, KeyT>::hash_map_;
        using cache::Cache<T, F, ListElemT, KeyT>::cache_;

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
                {
                    if (*it == *request_it)
                    {
                        tmp_list.erase(it);
                        break;
                    }
                }
                
                request_it = std::next(request_it);
            }

            return *it;
        }

public:
        Perfect(size_t size, std::vector<KeyT> requests) : cache::Cache<T, F, ListElemT, KeyT>(size),
                                                           requests_(requests) {}

        bool find_update(KeyT key, F slow_get_page) override
        {
            auto hit = hash_map_.find(key);
            
            if (hit == hash_map_.end())
            {
                if (is_full())
                {
                    KeyT del_key = find_last();
                    auto del_it = hash_map_.find(del_key);

                    hash_map_.erase(del_key);
                    cache_.erase(del_it->second);
                }
                
                cache_.emplace_front(key, slow_get_page(key));
                hash_map_.emplace(key, cache_.begin());

                return false;
            }

            return true;
        }
    };

} // namespace cache