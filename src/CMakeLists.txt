
add_subdirectory(Engine)


add_executable(SamsEngine
    main.cpp
)

target_include_directories(SamsEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(SamsEngine PRIVATE glfw)

find_package(glad REQUIRED)
target_link_libraries(SamsEngine PRIVATE glad::glad)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/StaticLibs/Includes)
add_library(DataStructures STATIC IMPORTED)


set_target_properties(DataStructures PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/StaticLibs/DataStructures.lib
)

target_link_libraries(SamsEngine PRIVATE DataStructures)
