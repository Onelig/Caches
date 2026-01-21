cmake_minimum_required(VERSION 3.20)

project(caches
    VERSION 1.0
    LANGUAGES CXX
)

add_library(caches INTERFACE
        test/LRU-test/lru_capacity.cc)

target_include_directories(caches
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(caches INTERFACE cxx_std_14)

option(CACHES_BUILD_TESTS "Build caches tests" ON)

if (CACHES_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()