include(FetchContent)

FetchContent_Declare(
     googletest
     URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(caches_tests
        main.cc
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