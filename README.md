В данном проекте представлена реализация кэша с 3 алгоритмами кэширования: LRU, LFU, "идеальный кэш, знающий будущее".

# Сборка
Из папки implementation:

```
cmake -S . -B build
cmake --build build
```

# Запуск
Из корневой папки проекта:
```
./implementation/build/src/main [type of cache]
```
[type of cache] - lru, lfu, perfect (задаёт тип алгоритма кэширования, с которым запускается программа)

# Запуск тестов
Из корневой папки проекта:
```
./run_tests_lru.sh
./run_tests_lfu.sh
./run_tests_perfect.sh
```
