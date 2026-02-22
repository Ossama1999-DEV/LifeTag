cmake_minimum_required(VERSION 3.10)
project(TagReader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(TagReader
    src/main.cpp
    src/ledController.cpp
)

target_include_directories(TagReader PRIVATE src)


target_link_libraries(TagReader PRIVATE Qt5::Widgets gpiod)