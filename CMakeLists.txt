cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #生成 compile_commands.json 以便 clangd 正确读取头文件
set(CMAKE_CXX_EXTENSIONS OFF)

# if (NOT CMAKE_TOOLCHAIN_FILE)
#     set(CMAKE_TOOLCHAIN_FILE "vcpkg/scripts/buildsystems/vcpkg.cmake")
# endif()

project(FNAF LANGUAGES CXX VERSION 0.1)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

if (WIN32)
    add_definitions(-DNOMINMAX -D_USE_MATH_DEFINES)
endif ()

if (NOT MSVC)
    find_program(CCACHE_PROGRAM ccache)
    if (CCACHE_PROGRAM)
        message(STATUS "Found CCache: ${CCACHE_PROGRAM}")
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_PROGRAM})
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_PROGRAM})
    endif ()
endif ()

add_definitions(-DPROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
add_definitions(-DPROJECT_NAME="${PROJECT_NAME}")


file(GLOB IMGUI_SRC
        ${CMAKE_SOURCE_DIR}/3rd_party/imgui-master/*.cpp
        ${CMAKE_SOURCE_DIR}/3rd_party/imgui-master/*.h
        ${CMAKE_SOURCE_DIR}/3rd_party/imgui-master/backends/imgui_impl_glfw.cpp
        ${CMAKE_SOURCE_DIR}/3rd_party/imgui-master/backends/imgui_impl_opengl3.cpp
)

set (IMGUI_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/3rd_party/imgui-master
        ${CMAKE_SOURCE_DIR}/3rd_party/imgui-master/backends
)

file(GLOB IMPLOT_SRC
        ${CMAKE_SOURCE_DIR}/3rd_party/implot-master/*.cpp
        ${CMAKE_SOURCE_DIR}/3rd_party/implot-master/*.h
)

set(IMPLOT_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/3rd_party/implot-master
)

set(3rd_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/3rd_party
)


file(GLOB FNAF_Game_SRC CONFIGURE_DEPENDS
        src/FNAF-Game/Game/*.h
        src/FNAF-Game/Game/*.cpp
        src/FNAF-Game/Objects/*.h
        src/FNAF-Game/Objects/*.cpp
        src/FNAF-Game/Window/*.h
        src/FNAF-Game/Window/*.cpp
)

set(FNAF_Game_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/src/FNAF-Game
)

file(GLOB CORE_SRC
        src/Core/AttriblessRendering.h
        src/Core/AttriblessRendering.cpp
        src/Core/InitShader.h
        src/Core/InitShader.cpp
        src/Core/LoadMesh.h
        src/Core/LoadMesh.cpp
        src/Core/LoadTexture.h
        src/Core/LoadTexture.cpp
        src/Core/GlEnumToString.h
        src/Core/GlEnumToString.cpp
        src/Core/Shader.h
        src/Core/Shader.cpp
        src/Core/UniformGui.h
        src/Core/UniformGui.cpp
        src/Core/DebugCallback.h
        src/Core/DebugCallback.cpp
)

set(CORE_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/src/Core
)


#add_subdirectory(src/FNAF-GL-DEMO)
add_subdirectory(src/FNAF-VR-DEMO)


if (EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    file(COPY "${CMAKE_BINARY_DIR}/compile_commands.json"
            DESTINATION "${CMAKE_SOURCE_DIR}")
endif ()

