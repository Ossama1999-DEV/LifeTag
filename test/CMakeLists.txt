# Utiliser GTest installé sur le système
find_package(GTest REQUIRED)

add_executable(LifeTagTests test_main.cpp)

target_link_libraries(LifeTagTests
    PRIVATE
        GTest::GTest
        GTest::Main
        LifeTagLibShared
)

target_include_directories(LifeTagTests PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/inc
)

include(GoogleTest)
gtest_discover_tests(LifeTagTests)
