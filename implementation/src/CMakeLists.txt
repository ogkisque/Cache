cmake_minimum_required(VERSION 3.11)
project(cache)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_library(page_lib STATIC slow_get_page.cpp)
target_include_directories(page_lib PUBLIC ${INCLUDE_DIR})

add_library(cache_lib STATIC lfu.cpp perfect.cpp lru.cpp)
target_include_directories(cache_lib PUBLIC ${INCLUDE_DIR})

add_executable(main main.cpp)
target_link_libraries(main cache_lib page_lib) 