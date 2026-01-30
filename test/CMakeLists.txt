include(FetchContent)

FetchContent_Declare(
     googletest
     URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
     DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

add_executable(caches_tests
        main.cc

        # LRU
        LRU-test/lru_capacity.cc
        LRU-test/lru_contains.cc
        LRU-test/lru_SFINAE.cc

        # LFU
        LFU-test/lfu_capacity.cc
        LFU-test/lfu_contains.cc
        LFU-test/lfu_SFINAE.cc
)

target_link_libraries(caches_tests PRIVATE
     gtest
     gtest_main
     caches
)

add_test(
    NAME caches-all
    COMMAND caches_tests
)