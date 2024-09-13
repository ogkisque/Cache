#include "perfect.hpp"

void use_perfect()
{
    size_t m = 0;
    size_t n = 0;
    size_t hits = 0;

    std::cin >> m >> n;

    std::vector<int> requests;

    for (size_t i = 0; i < n; i++)
    {
        int key = 0;
        std::cin >> key;
        requests.push_back(key);
    }

    cache::Perfect<int, int(int)> cache{m, requests};
    
    for (auto it = requests.begin(); it != requests.end(); it = std::next(it))
    {
        if (cache.find_update(*it, slow_get_page))
            hits++;
    }

    std::cout << hits << std::endl;
}