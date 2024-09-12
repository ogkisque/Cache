#include <iostream>
#include <cassert>

#include "cache.hpp"

int slow_get_page(int key);

int main()
{
    int hits = 0;
    size_t n;
    size_t m;

/*
    std::cin >> m >> n;
    assert(std::cin.good());
    cache::LFU<int, int(int)> cache{m};

    for (int i = 0; i < n; i++)
    {
        int data;
        std::cin >> data;
        assert(std::cin.good());

        if (cache.find_update(data, slow_get_page))
            hits++;
    }

    std::cout << hits << std::endl;
*/
    cache::Perfect<int, int(int)> cache{m, n};
}

int slow_get_page(int key)
{
    return key;
}