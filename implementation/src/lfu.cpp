#include "lfu.hpp"

void use_lfu()
{
    int hits = 0;
    size_t n;
    size_t m;

    std::cin >> m >> n;
    assert(std::cin.good());
    cache::LFU<int, int(int)> cache{m};

    for (size_t i = 0; i < n; i++)
    {
        int data;
        std::cin >> data;
        assert(std::cin.good());

        if (cache.find_update(data, slow_get_page))
            hits++;
    }

    std::cout << hits << std::endl;
}