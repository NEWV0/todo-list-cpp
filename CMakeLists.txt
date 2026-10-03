cmake_minimum_required(VERSION 3.16)

project(todo-list-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

option(BUILD_TESTS "Build unit tests" ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_executable(todo-app
    main.cpp
    sources/task.cpp
    sources/todolistapp.cpp
    headers/task.h
    headers/todolistapp.h
)

target_include_directories(todo-app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

target_link_libraries(todo-app PRIVATE
    Qt6::Core
    Qt6::Widgets
)

if(MSVC)
    target_compile_options(todo-app PRIVATE /W4)
else()
    target_compile_options(todo-app PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(BUILD_TESTS)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
    )

    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(todo-tests
        tests/test_task.cpp
        sources/task.cpp
    )

    target_include_directories(todo-tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/headers
    )

    target_link_libraries(todo-tests PRIVATE
        GTest::gtest_main
        Qt6::Core
    )

    if(MSVC)
        target_compile_options(todo-tests PRIVATE /W4)
    else()
        target_compile_options(todo-tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    include(GoogleTest)
    gtest_discover_tests(todo-tests)
endif()
