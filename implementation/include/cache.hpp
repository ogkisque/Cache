#pragma once

#include <list>
#include <unordered_map>
#include <iterator>
#include <iostream>

namespace cache
{
    
    template <typename T, typename KeyT = int> class Cache
    {
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

        template <typename F> bool find_update(KeyT key, F slow_get_page) {
            auto hit = hash_map_.find(key);

            //std::cout << this;

            if (hit == hash_map_.end())
            {
                if (is_full())
                {
                    hash_map_.erase(std::get<0>(*cache_.begin()));
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
                size_t next_count = std::get<1>(*next_it);

                if (cur_count > next_count)
                    cache_.splice(list_it, cache_, next_it, std::next(next_it));
            }
            return true;
        }
    
        friend std::ostream& operator <<(std::ostream& cout, const Cache& cache) 
        {
            std::cout << "Cache dump:\n";
            for (auto it = cache.cache_.begin(); it != cache.cache_.end(); it = std::next(it))
                cout << "data: " << std::get<0>(*it) << " count: " << std::get<1>(*it) << '\n'; 
            return cout; 
        } 
    };

} // namespace cache
