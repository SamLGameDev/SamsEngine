

add_executable(SamsEngine
    main.cpp
)

target_include_directories(SamsEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(SamsEngine PRIVATE glfw)
