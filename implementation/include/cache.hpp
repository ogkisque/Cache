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
        std::list<std::tuple<KeyT, size_t, T>> cache_;

        using ListIter = typename std::list<std::tuple<KeyT, size_t, T>>::iterator;
        std::unordered_map<KeyT, ListIter> hash_map_;

        bool is_full()
        {
            return cache_.size() == size_;
        }
public:
        Cache(size_t size) : size_(size) {}

        virtual bool find_update(KeyT key, F slow_get_page) { }
    
        friend std::ostream& operator <<(std::ostream& cout, const Cache& cache) 
        {
            std::cout << "Cache dump:\n";
            for (auto it = cache.cache_.begin(); it != cache.cache_.end(); it = std::next(it))
                cout << "data: " << std::get<0>(*it) << " count: " << std::get<1>(*it) << '\n'; 
            return cout; 
        } 
    };


    template <typename T, typename F, typename KeyT = int>
    class LFU : public Cache<T, F, KeyT>
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

                this->cache_.emplace_front(key, 1, slow_get_page(key));
                this->hash_map_.emplace(key, this->cache_.begin());

                return false;
            }

            auto list_it = hit->second;
            size_t cur_count = ++std::get<1>(*list_it);
            auto next_it = std::next(list_it);
            
            if (next_it != this->cache_.begin() && next_it != this->cache_.end())
            {
                size_t next_count = std::get<1>(*next_it);

                if (cur_count > next_count)
                    this->cache_.splice(list_it, this->cache_, next_it, std::next(next_it));
            }
            return true;
        }
    };

    template <typename T, typename F, typename KeyT = int>
    class LRU : public Cache<T, F, KeyT>
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
