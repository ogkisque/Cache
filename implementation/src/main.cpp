#include <cstring>
#include <iostream>
#include "cache.hpp"
#include "lru.hpp"
#include "lfu.hpp"
#include "perfect.hpp"

int parse_cmd(int num_args, char* args[]);

int main(int argc, char* argv[])
{
    return parse_cmd(argc, argv);
}

int parse_cmd(int num_args, char* args[])
{
    if (num_args < 2)
    {
        use_lfu();
        return 0;
    }
    else if (num_args > 2)
    {
        return 1;
    }
    else
    {
        if (strcmp(args[1], "lfu") == 0)
            use_lfu();
        else if (strcmp(args[1], "lru") == 0)
            use_lru();
        else if (strcmp(args[1], "perfect") == 0)
            use_perfect();
        else
            return 1;
        return 0;
    }
}