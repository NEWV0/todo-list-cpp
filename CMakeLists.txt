cmake_minimum_required(VERSION 3.16)

project(todo-list-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(todo-app
    main.cpp
    sources/task.cpp
    sources/todolistapp.cpp
    headers/task.h
    headers/todolistapp.h
)

target_include_directories(todo-app
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

target_link_libraries(todo-app
    PRIVATE
        Qt6::Widgets
)

if(MSVC)
    target_compile_options(todo-app PRIVATE /W4)
else()
    target_compile_options(todo-app PRIVATE -Wall -Wextra -Wpedantic)
endif()
