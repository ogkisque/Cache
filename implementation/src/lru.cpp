#include "lru.hpp"

void use_lru()
{
    int hits = 0;
    size_t n = 0;
    size_t m = 0;

    std::cin >> m >> n;
    assert(std::cin.good());
    cache::LRU<int, int(int)> cache{m};

    for (size_t i = 0; i < n; i++)
    {
        int data = 0;
        std::cin >> data;
        assert(std::cin.good());

        if (cache.find_update(data, slow_get_page))
            hits++;
    }

    std::cout << hits << std::endl;
}