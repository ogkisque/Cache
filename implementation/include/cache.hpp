#pragma once

#include <list>
#include <unordered_map>
#include <iterator>

namespace cache
{
    
    template <typename T, typename KeyT = int> class Cache
    {
        size_t size_;
        std::list<std::pair<KeyT, T>> cache_;

        using ListIter = typename std::list<std::pair<KeyT, T>>::iterator;
        std::unordered_map<KeyT, ListIter> hash_;

        public:
        Cache(size_t size) : size_(size) {}

        template <typename F> bool find_update(KeyT key, F slow_get_page) {
            auto hit = hash_.find(key);
            if (hit == hash_.end())
            {
                if (is_full())
                {
                    hash_.erase(cache_.back().first);
                    cache_.pop_back();
                }
                cache_.emplace_front(key, slow_get_page(key));
                hash_.emplace(key, cache_.begin());
                return false;
            }

            auto elem = hit->second;
            if (elem != cache_.begin())
                cache_.splice(cache_.begin(), cache_, elem, std::next(elem));
            
            return true;
        }

        private:
        bool is_full()
        {
            return cache_.size() == size_;
        }
    };

} // namespace cache
