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


add_executable(SamsEngine
    Engine/Rendering/Shaders/Shader.h
    Engine/Rendering/Shaders/Shader.cpp
    Engine/Rendering/Textures/CubeMap.h
    Engine/Rendering/Textures/CubeMap.cpp
    Engine/Rendering/Textures/Texture.h
    Engine/Rendering/Textures/Texture.cpp
    Engine/DataStructures/Array.h
    Engine/DataStructures/Array.cpp
    Engine/DataStructures/LinkedList.h
    Engine/DataStructures/LinkedList.cpp
    Engine/DataStructures/Item.h
    Engine/DataStructures/Item.cpp
    Engine/DataStructures/Vector2D.h
    Engine/DataStructures/Vector2D.cpp
    Engine/DataStructures/Vector3D.h
    Engine/DataStructures/Vector3D.cpp
    Engine/DataStructures/BaseDelegate.h
    Engine/DataStructures/BaseDelegate.cpp
    Engine/DataStructures/Verticie.h
    Engine/DataStructures/Verticie.cpp
    Engine/Window/FirstWindow.h
    Engine/Window/FirstWindow.cpp
    Engine/SubsystemInitialiser.h
    Engine/SubsystemInitialiser.cpp
    Engine/ErrorHandling/ErrorCodes.h
    Engine/ErrorHandling/ErrorCodes.cpp
    Engine/Input/InputManager.h
    Engine/Input/InputManager.cpp
    Engine/Input/InputActionMouse.h
    Engine/Input/InputActionMouse.cpp
    Engine/Input/InputAction.h
    Engine/Input/InputAction.cpp
    Engine/Objects/Object.h
    Engine/Objects/Object.cpp
    Engine/Objects/ObjectFactory.h
    Engine/Objects/ObjectFactory.cpp
    Engine/Objects/World/World.h
    Engine/Objects/World/World.cpp
    Engine/Objects/World/WorldObject.h
    Engine/Objects/World/WorldObject.cpp
    Engine/Camera/Camera.h
    Engine/Camera/Camera.cpp
    PreprocessorDefinitions.h
    PreprocessorDefinitions.cpp
    main.cpp

)


target_include_directories(SamsEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(SamsEngine PRIVATE glfw)

find_package(glad REQUIRED)
target_link_libraries(SamsEngine PRIVATE glad::glad)

find_package(Stb REQUIRED)
target_include_directories(SamsEngine PRIVATE ${Stb_INCLUDE_DIR})
