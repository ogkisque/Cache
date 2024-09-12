#pragma once

#include <list>
#include <unordered_map>
#include <iterator>
#include <iostream>
#include <cassert>

namespace cache
{
    using CountT = size_t;

    template <typename T, typename F, typename KeyT = int, typename ListElemT = int>
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

        virtual bool find_update(KeyT key, F slow_get_page) { }
    };

//===============================================================================
//===================================== LFU =====================================
//===============================================================================

    template <typename T, typename F, typename KeyT = int>
    class LFU : public Cache<T, F, KeyT, std::tuple<KeyT, CountT, T>>
    {
        using ListElemT = typename std::tuple<KeyT, CountT, T>;
        using ListIter = typename std::list<ListElemT>::iterator;
        using cache::Cache<T, F, KeyT, ListElemT>::Cache;
        using cache::Cache<T, F, KeyT, ListElemT>::is_full;
        using cache::Cache<T, F, KeyT, ListElemT>::hash_map_;
        using cache::Cache<T, F, KeyT, ListElemT>::cache_;

        KeyT get_key(ListIter iter)
        {
            return std::get<0>(*iter);
        }

        CountT get_count(ListIter iter)
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
            size_t cur_count = ++std::get<1>(*list_it);
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

//===============================================================================
//===================================== LRU =====================================
//===============================================================================

    template <typename T, typename F, typename KeyT = int>
    class LRU : public Cache<T, F, KeyT, std::pair<KeyT, T>>
    {
        using ListElemT = typename std::pair<KeyT, T>;
        using cache::Cache<T, F, KeyT, ListElemT>::Cache;
        using cache::Cache<T, F, KeyT, ListElemT>::is_full;
        using cache::Cache<T, F, KeyT, ListElemT>::hash_map_;
        using cache::Cache<T, F, KeyT, ListElemT>::cache_;

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

                cache_.emplace_front(key, 0, slow_get_page(key));
                hash_map_.emplace(key, cache_.begin());

                return false;
            }

            auto elem = hit->second;
            if (elem != cache_.begin())
                cache_.splice(cache_.begin(), cache_, elem, std::next(elem));
            
            return true;
        }
    };

//===============================================================================
//=================================== Perfect ===================================
//===============================================================================

    template <typename T, typename F, typename KeyT = int>
    class Perfect : public Cache<T, F, KeyT, std::pair<KeyT, T>>
    {
        using ListElemT = typename std::pair<KeyT, T>;
        using cache::Cache<T, F, KeyT, ListElemT>::is_full;
        using cache::Cache<T, F, KeyT, ListElemT>::hash_map_;
        using cache::Cache<T, F, KeyT, ListElemT>::cache_;
        std::list<KeyT> request_list_;
        size_t request_num_;
        
        void make_request()
        {
            KeyT key = 0;
            for (size_t i = 0; i < request_num_; i++)
            {
                std::cin >> key;
                assert(std::cin.good());
                request_list_.push_back(key);
            }
        }

        KeyT find_last()
        {
            std::list<KeyT> tmp_list;
                    
            for (auto it = cache_.begin(); it != cache_.end(); it = std::next(it))
                tmp_list.push_back(cache_.begin().first);

            auto request_it = request_list_.begin();
            auto it = tmp_list.begin();
            
            while (tmp_list.size() != 0 && request_it != request_list_.end())
            {
                for (it = tmp_list.begin(); it != tmp_list.end(); it = std::next(request_it))
                    if (*it == *request_it)
                        tmp_list.erase(it);
                
                request_it = std::next(request_it);
            }

            return *it;
        }

public:
        Perfect(size_t size, size_t request_num) : Cache<T, F, KeyT, ListElemT>(size),
                                                   request_num_(request_num)
        {
            make_request();
        }

        bool find_update(KeyT key_unused, F slow_get_page) override
        {
            while (request_list_.size() > 0)
            {
                KeyT request_key = *(request_list_.begin());
                request_list_.pop_front();
                auto hit = hash_map_.find(request_key);
                
                if (hit == hash_map_.end())
                {
                    if (is_full())
                    {
                        KeyT key = find_last();
                        
                        for (auto it = cache_.end(); it != cache_.end(); it = std::next(it))
                        {
                            if (key == std::get<0>(*it))
                            {
                                hash_map_.erase(std::get<0>(*it));
                                cache_.erase(it);
                            }
                        }
                    }
                    
                    cache_.emplace_front(request_key, slow_get_page(request_key), 1);
                    hash_map_.emplace(request_key, cache_.begin());
                }
            }
        }
    };

} // namespace cache
