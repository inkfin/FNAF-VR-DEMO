cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

project(FNAF_VR_DEMO LANGUAGES CXX VERSION 0.1)

if (PROJECT_BINARY_DIR STREQUAL PROJECT_SOURCE_DIR)
    message(SEND_ERROR "The binary directory of CMake cannot be the same as source directory!")
endif ()

if (MSVC)
    # Static library
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
endif (MSVC)


add_definitions(-DXR_USE_GRAPHICS_API_OPENGL
        -DXR_USE_PLATFORM_WIN32
        -D_CONSOLE
)


file(GLOB XR_COMMON_SRC
        xr_common/*.h
        xr_common/*.cpp
        xr_common/*.c
)

file(GLOB XR_CORE_SRC
        xr_core/*.h
        xr_core/*.cpp
)

file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS
        VR/*.h
        VR/*.cpp
)

# message(STATUS "Source files: ${SOURCE}")

add_executable(${PROJECT_NAME} ${SOURCE}
        ${CORE_SRC}
        ${FNAF_Game_SRC}
        ${XR_COMMON_SRC}
        ${XR_CORE_SRC}
        ${IMGUI_SRC}
        ${IMPLOT_SRC}
        ../FNAF-Game/Game/JsonConfig.cpp
        ../FNAF-Game/Game/JsonConfig.h
        ../FNAF-Game/Objects/EventComponent.cpp
        ../FNAF-Game/Objects/EventComponent.h
        ../FNAF-Game/Objects/EventManager.cpp
        ../FNAF-Game/Objects/EventManager.h
        ../FNAF-Game/Objects/TitleMesh.cpp
        ../FNAF-Game/Objects/TitleMesh.h
)

target_include_directories(${PROJECT_NAME} PUBLIC
        .
        ${FNAF_Game_INCLUDE_DIR}
        ${CORE_INCLUDE_DIR}
        ${IMGUI_INCLUDE_DIR}
        ${IMPLOT_INCLUDE_DIR}
        ${3rd_INCLUDE_DIR}
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_link_directories(${PROJECT_NAME} PUBLIC ../../lib/Debug)

    target_link_libraries(${PROJECT_NAME} PUBLIC
            FreeImage.lib
            assimp-vc143-mt.lib
            glfw3.lib
            glew32.lib
            openxr-gfxwrapper.lib
            openxr_loaderd.lib
            opengl32.lib)
elseif (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_link_directories(${PROJECT_NAME} PUBLIC ../../lib/Release)

    target_link_libraries(${PROJECT_NAME} PUBLIC
            FreeImage.lib
            assimp-vc143-mt.lib
            glfw3.lib
            glew32.lib
            openxr-gfxwrapper.lib
            openxr_loader.lib
            opengl32.lib
    )
endif ()


# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}/Release)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}/Debug)
if (WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}/Debug)
elseif (UNIX)
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}/Release)
else () # Debug
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}/Debug)
endif ()
message(STATUS "Set binary output directory to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ../assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
#        COMMENT "Copy assets to binary directory" VERBATIM
#)
#
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ../shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
#        COMMENT "Copy shaders to binary directory" VERBATIM
#)

file(GLOB DLLS_SRC ${CMAKE_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}/*.dll)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLLS_SRC} $<TARGET_FILE_DIR:${PROJECT_NAME}>/
        COMMENT "Copy dlls to binary directory" VERBATIM
)
