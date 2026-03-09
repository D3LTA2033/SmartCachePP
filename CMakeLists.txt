cmake_minimum_required(VERSION 3.10)
project(SmartCachePP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(SmartCachePP
    src/SmartCache.cpp
    src/LRU.cpp
    src/Metrics.cpp
    src/DiskPersistence.cpp
    src/ThreadPool.cpp
)

find_package(Threads REQUIRED)
target_link_libraries(SmartCachePP PUBLIC Threads::Threads)

add_executable(smartcache_example examples/main.cpp)
target_include_directories(smartcache_example PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(smartcache_example PRIVATE SmartCachePP)

add_executable(smartcache_test tests/test_cache.cpp)
target_include_directories(smartcache_test PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(smartcache_test PRIVATE SmartCachePP)