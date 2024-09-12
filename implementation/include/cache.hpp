#pragma once

#include <list>
#include <unordered_map>
#include <iterator>
#include <iostream>

namespace cache
{
    
    template <typename T, typename F, typename KeyT = int>
    class Cache
    {
protected:
        size_t size_;
        using CountT = size_t;
        std::list<std::tuple<KeyT, CountT, T>> cache_;

        using ListIter = typename std::list<std::tuple<KeyT, CountT, T>>::iterator;
        std::unordered_map<KeyT, ListIter> hash_map_;

        bool is_full()
        {
            return cache_.size() == size_;
        }

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
        Cache(size_t size) : size_(size) {}

        virtual bool find_update(KeyT key, F slow_get_page) { }
    
        friend std::ostream& operator <<(std::ostream& cout, const Cache& cache) 
        {
            std::cout << "Cache dump:\n";
            for (auto it = cache.cache_.begin(); it != cache.cache_.end(); it = std::next(it))
                cout << "data: " << get_data(it) << " count: " << get_count(it) << '\n'; 
            return cout; 
        } 
    };


    template <typename T, typename F, typename KeyT = int>
    class LFU : public Cache<T, F, KeyT>
    {
public:
        using cache::Cache<T, F>::Cache;
        using cache::Cache<T, F>::is_full;
        using cache::Cache<T, F>::hash_map_;
        using cache::Cache<T, F>::cache_;
        using cache::Cache<T, F>::get_key;
        using cache::Cache<T, F>::get_count;

        bool find_update(KeyT key, F slow_get_page) {
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
    };


    template <typename T, typename F, typename KeyT = int>
    class LRU : public Cache<T, F, KeyT>
    {
public:
        using cache::Cache<T, F>::Cache;
        using cache::Cache<T, F>::is_full;
        using cache::Cache<T, F>::hash_map_;
        using cache::Cache<T, F>::cache_;
        using cache::Cache<T, F>::get_key;

        bool find_update(KeyT key, F slow_get_page) {
            auto hit = hash_map_.find(key);

            if (hit == hash_map_.end())
            {
                if (is_full())
                {
                    hash_map_.erase(get_key(cache_.begin()));
                    cache_.pop_front();
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


    template <typename T, typename F, typename KeyT = int>
    class Perfect : public Cache<T, F, KeyT>
    {
public:
        using cache::Cache<T, F>::Cache;

        bool find_update(KeyT key, F slow_get_page) {
            auto hit = this->hash_map_.find(key);

            if (hit == this->hash_map_.end())
            {
                if (this->is_full())
                {
                    this->hash_map_.erase(std::get<0>(*this->cache_.begin()));
                    this->cache_.pop_front();
                }

                this->cache_.emplace_front(key, 0, slow_get_page(key));
                this->hash_map_.emplace(key, this->cache_.begin());

                return false;
            }

            auto elem = hit->second;
            if (elem != this->cache_.begin())
                this->cache_.splice(this->cache_.begin(), this->cache_, elem, std::next(elem));
            
            return true;
        }
    };

} // namespace cache
