
project(project-unit)

add_executable(project-unit
    project-unit.cpp
)

target_link_libraries(project-unit
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        GTest::gtest_main GTest::gmock GTest::gmock_main
        EngineCore
)

include(GoogleTest)
add_test(Math project-unit.cpp)
