include_directories(.)
include_directories(Engine)
include_directories(Engine/Rendering)
include_directories(Engine/Rendering/Shaders)
include_directories(Engine/Rendering/Textures)
include_directories(Engine/DataStructures)
include_directories(Engine/Window)
include_directories(Engine/ErrorHandling)
include_directories(Engine/Input)
include_directories(Engine/Objects)
include_directories(Engine/Objects/World)
include_directories(Engine/Camera)
include_directories(Engine/Objects/Placement)
include_directories(Engine/Rendering/Buffers)
include_directories(Engine/Objects/Placement/Meshes)
include_directories(Engine/Objects/Placement/Debug/WireFrame)
include_directories(Engine/Math)
include_directories(Engine/Physics)
include_directories(Engine/Physics/Collision)
include_directories(Engine/Physics/Collision/Casters)
include_directories(Engine/Lighting)
include_directories(Engine/Components)
include_directories(Game)

add_executable(SamsEngine
            main.cpp
            Game/MeshObject.h
            Game/MeshObject.cpp
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

find_package(EngineCore REQUIRED)
target_link_libraries(SamsEngine PRIVATE EngineCore::EngineCore)

find_package(Stb REQUIRED)
target_include_directories(SamsEngine PRIVATE ${Stb_INCLUDE_DIR})
