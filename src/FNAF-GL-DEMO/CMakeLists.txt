cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

project(FNAF_GL_DEMO LANGUAGES CXX VERSION 0.1)

if (PROJECT_BINARY_DIR STREQUAL PROJECT_SOURCE_DIR)
    message(SEND_ERROR "The binary directory of CMake cannot be the same as source directory!")
endif ()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        DemoGL/*.h
        DemoGL/*.cpp
        main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES}
        ${CORE_SRC}
        ${FNAF_Game_SRC}
        ${IMGUI_SRC}
        ${IMPLOT_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC
        .
        ${CORE_INCLUDE_DIR}
        ${FNAF_Game_INCLUDE_DIR}
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
            opengl32.lib)
elseif (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_link_directories(${PROJECT_NAME} PUBLIC ../../lib/Release)

    target_link_libraries(${PROJECT_NAME} PUBLIC
            FreeImage.lib
            assimp-vc143-mt.lib
            glfw3.lib
            glew32.lib
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


## shaders
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders)
#message(STATUS "Copy shaders to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders")
#
## assets
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)
#message(STATUS "Copy assets to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")


file(GLOB DLLS_SRC ${CMAKE_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}/*.dll)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLLS_SRC} $<TARGET_FILE_DIR:${PROJECT_NAME}>/
        COMMENT "Copy dlls to binary directory" VERBATIM
)
