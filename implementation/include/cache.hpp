#pragma once

#include <list>
#include <unordered_map>
#include <iterator>

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

            if (hit == hash_map_.end())
            {
                if (is_full())
                {
                    auto min_it = cache_.begin();
                    size_t min_count = std::get<1>(*min_it);

                    for (auto it = cache_.begin(); it != cache_.end(); it = std::next(it))
                    {
                        size_t count = std::get<1>(*it);
                        if (count < min_count)
                        {
                            min_count = count;
                            min_it = it;
                        }
                    }
                
                    hash_map_.erase(std::get<0>(*min_it));
                    cache_.erase(min_it);
                }

                cache_.emplace_front(key, slow_get_page(key), 1);
                hash_map_.emplace(key, cache_.begin());

                return false;
            }

            auto list_it = hit->second;
            std::get<1>(*list_it)++;
            
            return true;
        }
    };

} // namespace cache
