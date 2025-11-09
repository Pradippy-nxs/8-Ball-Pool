cmake_minimum_required(VERSION 3.20)
project(BilliardGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BILLIARD_WITH_SFML "Enable SFML backend (adapters only, optional)" OFF)
option(BILLIARD_BUILD_TESTS "Build unit tests" OFF)
option(BILLIARD_WARN_AS_ERROR "Treat warnings as errors" OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wunreachable-code)
endif()

if(BILLIARD_WARN_AS_ERROR)
    if(MSVC)
        add_compile_options(/WX)
    else()
        add_compile_options(-Werror)
    endif()
endif()

add_library(billiard_core
    src/core/Version.cpp
)
target_include_directories(billiard_core PUBLIC include)

if(BILLIARD_WITH_SFML)
    target_compile_definitions(billiard_core PUBLIC BILLIARD_WITH_SFML=1)
endif()

add_executable(billiard_game
    src/main.cpp
)
target_link_libraries(billiard_game PRIVATE billiard_core)

if(BILLIARD_WITH_SFML)
    find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)
endif()

if(BILLIARD_BUILD_TESTS)
    enable_testing()
endif()