cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

project(FNAF_VR_DEMO LANGUAGES CXX VERSION 0.1)

if (PROJECT_BINARY_DIR STREQUAL PROJECT_SOURCE_DIR)
    message(SEND_ERROR "The binary directory of CMake cannot be the same as source directory!")
endif()

add_definitions(-DPROJECT_NAME="${PROJECT_NAME}")

add_definitions(-DXR_USE_GRAPHICS_API_OPENGL
                -DXR_USE_PLATFORM_WIN32
                -D_CONSOLE
)

file(GLOB IMGUI_SRC ../imgui-master/*.cpp
                    ../imgui-master/*.h
                    ../imgui-master/backends/imgui_impl_glfw.cpp
                    ../imgui-master/backends/imgui_impl_opengl3.cpp
)
file(GLOB IMPLOT_SRC ../implot-master/*.cpp
                     ../implot-master/*.h)

file(GLOB XR_COMMON_SRC ../xr_common/*.c
                        ../xr_common/*.h
                        ../xr_common/*.cpp)
file(GLOB XR_CORE_SRC   ../xr_core/*.c
                        ../xr_core/*.h
                        ../xr_core/*.cpp)
file(GLOB CORE_SRC      ../Core/*.c
                        ../Core/*.h
                        ../Core/*.cpp)
file(GLOB LIBZIP_SRC    ../include/libzip/*.c
                        ../include/libzip/*.h)

message(STATUS "LIBZIP_SRC: ${LIBZIP_SRC}")

file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS
    "./*.h"
    "./*.cpp"
)

message(STATUS "Source files: ${SOURCE}")

add_executable(${PROJECT_NAME} ${SOURCE}
                            ${IMGUI_SRC}
                            ${IMPLOT_SRC}
                            ${CORE_SRC}
                            ${XR_COMMON_SRC}
                            ${XR_CORE_SRC}
                            ${LIBZIP_SRC}
)

target_include_directories(${PROJECT_NAME} PUBLIC . ..
                                                ../include
                                                ../include/libzip
                                                ../imgui-master
                                                ../imgui-master/backends
                                                ../implot-master
                                                ../xr_core
                                                ../xr_common
                                                ../Core
)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_directories(${PROJECT_NAME} PUBLIC ../lib/Release)
else()
    target_link_directories(${PROJECT_NAME} PUBLIC ../lib/Debug)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC FreeImage.lib
                                            assimp-vc143-mt.lib
                                            glfw3.lib
                                            glew32.lib
                                            openxr-gfxwrapper.lib
                                            openxr_loaderd.lib
                                            opengl32.lib)


# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}/Release)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}/Debug)
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY    ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}/Debug)
elseif(UNIX)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}/Release)
else() # Debug
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}/Debug)
endif()
message(STATUS "Set binary output directory to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "Copy assets to binary directory" VERBATIM
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
    COMMENT "Copy shaders to binary directory" VERBATIM
)
