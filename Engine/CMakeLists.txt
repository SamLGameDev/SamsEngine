cmake_minimum_required(VERSION 3.3)

# see https://vcpkg.io/en/docs/users/buildsystems/cmake-integration.html#visual-studio if using visual studio
# On Linux-based distrubtions, use your package manager for dependeices

# VCPKG integration for Windows. See my guide on getting that setup
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

message(STATUS "VCPKG_ROOT = $ENV{VCPKG_ROOT}")
message(STATUS "CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")

# Use C++ 17
set(CMAKE_CXX_STANDARD 23)

set(Contents "${CMAKE_SOURCE_DIR}/Contents")

# Option support (faster build times by disabling features)
option(FAL_TESTS "Should we enable unit test support?" ON)
option(FAL_DOCS "Should we attempt to build documentation?" ON)


project(EngineCore LANGUAGES CXX)

include(GNUInstallDirs)

add_library(EngineCore STATIC
    "src/Engine/Rendering/Shaders/Shader.cpp"
    "src/Engine/Rendering/Shaders/BaseShader.cpp"
    "src/Engine/Rendering/Shaders/OpenGLShader.cpp"
    "src/Engine/Rendering/Shaders/VulkanShader.cpp"
    "src/Engine/Rendering/Textures/CubeMap.cpp"
    "src/Engine/Rendering/Textures/Texture.cpp"
    "src/Engine/Rendering/Textures/OpenGLTexture.cpp"
    "src/Engine/Rendering/Textures/VulkanTexture.cpp"
    "src/Engine/Rendering/Textures/BaseTexture.cpp"
    "src/Engine/DataStructures/Array.cpp"
    "src/Engine/DataStructures/LinkedList.cpp"
    "src/Engine/DataStructures/Item.cpp"
    "src/Engine/DataStructures/Vector2D.cpp"
    "src/Engine/DataStructures/Vector3D.cpp"
    "src/Engine/DataStructures/BaseDelegate.cpp"
    "src/Engine/DataStructures/Verticie.cpp"
    "src/Engine/DataStructures/Matrix.cpp"
    "src/Engine/Window/FirstWindow.cpp"
    "src/Engine/Window/Window.cpp"
    "src/Engine/Window/VulkanWindow.cpp"
    "src/Engine/SubsystemInitialiser.cpp"
    "src/Engine/GraphicsAPI/VulkanAPI/VulkanSubsystemInitialiser.cpp"
    "src/Engine/ErrorHandling/ErrorCodes.cpp"
    "src/Engine/Input/InputManager.cpp"
    "src/Engine/Input/InputActionMouse.cpp"
    "src/Engine/Input/InputAction.cpp"
    "src/Engine/Objects/Object.cpp"
    "src/Engine/Objects/ObjectFactory.cpp"
    "src/Engine/Objects/World/World.cpp"
    "src/Engine/Objects/World/WorldObject.cpp"
    "src/Engine/Camera/Camera.cpp"
    "src/Engine/Rendering/Renderer.cpp"
    "src/Engine/Rendering/BaseRenderer.cpp"
    "src/Engine/Rendering/InterfaceRenderer.cpp"
    "src/Engine/Rendering/VulkanRenderer.cpp"
    "src/Engine/Objects/Placement/Transform.cpp"
    "src/Engine/Rendering/Buffers/FrameBufferFactory.cpp"
    "src/Engine/Rendering/Buffers/UniformBufferFactory.cpp"
    "src/Engine/Objects/Placement/Meshes/Model.cpp"
    "src/Engine/Objects/Placement/Meshes/Mesh.cpp"
    "src/Engine/Objects/Placement/Debug/WireFrame/WireShapes.cpp"
    "src/Engine/Math/MathCore.cpp"
    "src/Engine/Triangulation/DelaunayTriangulation.cpp"
    "src/Engine/Physics/Collision/Casters/CollisionCast.cpp"
    "src/Engine/Lighting/BaseLight.cpp"
    "src/Engine/Lighting/LightManager.cpp"
    "src/Engine/Lighting/DirectionalLight.cpp"
    "src/Engine/Lighting/SpotLight.cpp"
    "src/Engine/Lighting/PointLight.cpp"
    "src/Engine/Components/StaticMeshComponent.cpp"
    "src/Engine/Fracturing/Voronoi.cpp"
    "src/Engine/Fracturing/Voronoi2D.cpp"
    "src/Engine/FileSystem/Path.cpp"
    "src/Engine/FileSystem/CorePaths.cpp"
    "src/PreprocessorDefinitions.cpp"
    "src/Engine/GraphicsAPI/VulkanAPI/RuntimeEngineVulkan.cpp"
    "src/Engine/RuntimeEngine.cpp"
    "src/Engine/GraphicsAPI/DataBuffers/DataBuffers.cpp"
    "src/Engine/GraphicsAPI/DataBuffers/BaseDataBuffers.cpp"
    "src/Engine/GraphicsAPI/DataBuffers/DataBuffersVulkan.cpp"
    "src/Engine/GraphicsAPI/DataBuffers/DataBuffersOpenGL.cpp"
    "src/Engine/GraphicsAPI/VulkanAPI/InitialiseVulkan.cpp"
    "src/Engine/GraphicsAPI/VulkanAPI/VulkanInstance.cpp"
    "src/Engine/GraphicsAPI/VulkanAPI/VulkanLogicalDevice.cpp"
    "src/Engine/GraphicsAPI/VulkanAPI/VulkanSwapChain.cpp"
    "src/Engine/GraphicsAPI/VulkanAPI/VulkanGraphicsCard.cpp"
    "src/Engine/GraphicsAPI/VulkanAPI/VulkanRenderPipeline.cpp"
    "src/Engine/GraphicsAPI/VulkanAPI/VulkanImageView.cpp"
    "src/Engine/GraphicsAPI/VulkanAPI/VulkanRenderPass.cpp"
    "src/Engine/GraphicsAPI/VulkanAPI/VulkanFrameBuffer.cpp"
    "src/Engine/GraphicsAPI/GraphicsAPIConstructor.cpp")

set_target_properties(EngineCore
                      PROPERTIES
                      CMAKE_CXX_STANDARD 23
                      CMAKE_CXX_STANDARD_REQUIRED ON
                      CMAKE_CXX_EXTENSIONS OFF)

target_include_directories(EngineCore
                           PUBLIC
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/DataStructures>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Camera>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Components>"                           
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/ErrorHandling>"                           
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Input>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Lighting>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Math>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Objects>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Objects/Placement>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Objects/Placement/Debug>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Objects/Placement/Debug/WireFrame>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Objects/Placement/Meshes>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Objects/World>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Physics>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Physics/Collision>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Physics/Collision/Casters>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Rendering>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Rendering/Buffers>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Rendering/Shaders>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Rendering/Textures>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Window>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/FileSystem>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Fracturing>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Triangulation>"
                 
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/GraphicsAPI>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/GraphicsAPI/VulkanAPI>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/GraphicsAPI/DataBuffers>"

                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

install(TARGETS EngineCore
        EXPORT EngineCoreTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES
    src/Engine/Rendering/Shaders/Shader.h 
    src/Engine/Rendering/Shaders/BaseShader.h
    src/Engine/Rendering/Shaders/OpenGLShader.h
    src/Engine/Rendering/Shaders/VulkanShader.h
    src/Engine/Rendering/Textures/Texture.h
    src/Engine/Rendering/Textures/VulkanTexture.h
    src/Engine/Rendering/Textures/OpenGLTexture.h
    src/Engine/Rendering/Textures/BaseTexture.h
    src/Engine/Rendering/Textures/CubeMap.h
    src/Engine/DataStructures/Array.h
    src/Engine/DataStructures/LinkedList.h
    src/Engine/DataStructures/Item.h
    src/Engine/DataStructures/Vector2D.h
    src/Engine/DataStructures/Vector3D.h
    src/Engine/DataStructures/BaseDelegate.h
    src/Engine/DataStructures/Matrix.h
    src/Engine/DataStructures/Verticie.h
    src/Engine/Window/FirstWindow.h
    src/Engine/Window/Window.h
    src/Engine/Window/VulkanWindow.h
    src/Engine/SubsystemInitialiser.h
    src/Engine/GraphicsAPI/VulkanAPI/VulkanSubsystemInitialiser.h
    src/Engine/ErrorHandling/ErrorCodes.h
    src/Engine/Input/InputManager.h
    src/Engine/Input/InputActionMouse.h
    src/Engine/Input/InputAction.h
    src/Engine/Objects/Object.h
    src/Engine/Objects/ObjectFactory.h
    src/Engine/Objects/World/World.h
    src/Engine/Objects/World/WorldObject.h
    src/Engine/Camera/Camera.h
    src/Engine/Rendering/VulkanRenderer.h
    src/Engine/Rendering/Renderer.h
    src/Engine/Rendering/BaseRenderer.h
    src/Engine/Rendering/InterfaceRenderer.h
    src/Engine/Objects/Placement/Transform.h
    src/Engine/Rendering/Buffers/FrameBufferFactory.h
    src/Engine/Rendering/Buffers/UniformBufferFactory.h
    src/Engine/Objects/Placement/Meshes/Model.h
    src/Engine/Objects/Placement/Meshes/Mesh.h
    src/Engine/Objects/Placement/Debug/WireFrame/WireShapes.h
    src/Engine/Math/MathCore.h
    src/Engine/Triangulation/DelaunayTriangulation.h
    src/Engine/Physics/Collision/Casters/CollisionCast.h
    src/Engine/Lighting/BaseLight.h
    src/Engine/Lighting/LightManager.h
    src/Engine/Lighting/DirectionalLight.h
    src/Engine/Lighting/SpotLight.h
    src/Engine/Lighting/PointLight.h
    src/Engine/Components/StaticMeshComponent.h
    src/Engine/FileSystem/Path.h
    src/Engine/FileSystem/CorePaths.h
    src/Engine/Fracturing/Voronoi.h
    src/Engine/Fracturing/Voronoi2D.h
    src/PreprocessorDefinitions.h
    src/Engine/RuntimeEngine.h
    src/Engine/GraphicsAPI/DataBuffers/DataBuffers.h
    src/Engine/GraphicsAPI/DataBuffers/BaseDataBuffers.h
    src/Engine/GraphicsAPI/DataBuffers/DataBuffersOpenGL.h
    src/Engine/GraphicsAPI/DataBuffers/DataBuffersVulkan.h
    src/Engine/GraphicsAPI/VulkanAPI/RuntimeEngineVulkan.h
    src/Engine/GraphicsAPI/VulkanAPI/InitialiseVulkan.h
    src/Engine/GraphicsAPI/VulkanAPI/VulkanGraphicsCard.h
    src/Engine/GraphicsAPI/VulkanAPI/VulkanLogicalDevice.h
    src/Engine/GraphicsAPI/VulkanAPI/VulkanSwapChain.h
    src/Engine/GraphicsAPI/VulkanAPI/VulkanImageView.h
    src/Engine/GraphicsAPI/VulkanAPI/VulkanRenderPipeline.h
    src/Engine/GraphicsAPI/VulkanAPI/VulkanRenderPass.h
    src/Engine/GraphicsAPI/VulkanAPI/VulkanFrameBuffer.h
    src/Engine/GraphicsAPI/GraphicsAPIConstructor.h
    src/Engine/GraphicsAPI/VulkanAPI/VulkanInstance.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(EngineCore PRIVATE glfw)

find_package(glad REQUIRED)
target_link_libraries(EngineCore PRIVATE glad::glad)

find_package(glm REQUIRED)
target_link_libraries(EngineCore PRIVATE glm::glm)

find_package(assimp REQUIRED)
target_link_libraries(EngineCore PRIVATE assimp::assimp)

find_package(Stb REQUIRED)
target_include_directories(EngineCore PRIVATE ${Stb_INCLUDE_DIR})

find_package(Vulkan REQUIRED)
target_link_libraries(EngineCore PUBLIC Vulkan::Vulkan)

target_compile_definitions(EngineCore PUBLIC CONTENTS_DIR="${Contents}")

    install(EXPORT EngineCoreTargets FILE EngineCoreTargets.cmake NAMESPACE EngineCore:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EngineCore)

    include(CMakePackageConfigHelpers)

    configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
                                  "${CMAKE_CURRENT_BINARY_DIR}/EngineCoreConfig.cmake"
                                  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EngineCore)

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/EngineCoreConfig.cmake" DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EngineCore)
    # Project details start here
    

    export(EXPORT EngineCoreTargets
       FILE "${CMAKE_CURRENT_BINARY_DIR}/EngineCoreTargets.cmake"
       NAMESPACE EngineCore::)


# integration for command-line users
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})#

message(STATUS "FAL_TESTS = ${FAL_TESTS}")
# Testing support
if (FAL_TESTS)
	include(CTest)
    enable_testing()
	add_subdirectory(tests)
endif()

add_subdirectory( src )




# Documentation support
if (FAL_DOCS)
	find_package(Doxygen)
	if (Doxygen_FOUND)
		add_subdirectory( docs )
	else()
		message(STATUS "Doxygen not found, no docs will be built")
	endif()
endif()

target_compile_definitions(EngineCore PRIVATE DEBUG=0)

target_compile_definitions(EngineCore PRIVATE RELEASE=0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(EngineCore PRIVATE DEBUG=1)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(EngineCore PRIVATE RELEASE=1)
endif()

