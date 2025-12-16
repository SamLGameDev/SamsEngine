include_directories(.)
add_executable(SamsEngine
            main.cpp
            MeshObject.h
            MeshObject.cpp
)


target_include_directories(SamsEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(SamsEngine PRIVATE glfw)

find_package(glad REQUIRED)
target_link_libraries(SamsEngine PRIVATE glad::glad)

find_package(glm REQUIRED)
target_link_libraries(SamsEngine PRIVATE glm::glm)

find_package(assimp REQUIRED)
target_link_libraries(SamsEngine PRIVATE assimp::assimp)

find_package(Vulkan REQUIRED)
target_link_libraries(SamsEngine PRIVATE Vulkan::Vulkan)

find_package(CGAL REQUIRED)
target_link_libraries(SamsEngine PRIVATE CGAL::CGAL)

set(CMAKE_FIND_DEBUG_MODE 1)
find_package(EngineCore REQUIRED)
target_link_libraries(SamsEngine PRIVATE EngineCore::EngineCore)

find_package(Stb REQUIRED)
target_include_directories(SamsEngine PRIVATE ${Stb_INCLUDE_DIR})
