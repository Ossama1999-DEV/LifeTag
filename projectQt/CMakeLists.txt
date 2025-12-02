cmake_minimum_required(VERSION 3.10)
project(projectQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(projectQt
    src/main.cpp
    src/ledController.cpp
)

# On laisse CMake trouver la bonne lib gpiod selon l’archi
find_library(GPIOD_LIB gpiod REQUIRED)

target_link_libraries(projectQt
    PRIVATE
        Qt5::Widgets
        ${GPIOD_LIB}
)
