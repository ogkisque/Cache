#include <iostream>
#include <cassert>

#include "cache.hpp"

int slow_get_page(int key);

int main()
{
    int hits = 0;
    int n;
    size_t m;

    std::cin >> m >> n;
    assert(std::cin.good());
    cache::Cache<int> cache{m};

    for (int i = 0; i < n; i++) {
        int data;
        std::cin >> data;
        assert(std::cin.good());
        if (cache.find_update(data, slow_get_page))
            hits += 1;
    }

    std::cout << hits << std::endl;
}

int slow_get_page(int key)
{
    return key;
}