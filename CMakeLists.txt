# ===========================================================================
# CMakeLists.txt – Desktop (Windows) build for SPACE
#
# Scene/ holds: main.cpp (entry point), Triangle.cpp/.h (self-contained renderer),
#               Platform.h (GL + windowing headers).
# The same Triangle logic is mirrored as a monolithic JNI file for Android:
#   android/app/src/main/cpp/NativeTemplate.cpp
#
# Usage:
#   cmake -S . -B build_desktop -G "Visual Studio 17 2022" -A x64
#   cmake --build build_desktop --config Debug
# ===========================================================================

cmake_minimum_required(VERSION 3.15)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
cmake_policy(VERSION 3.15)

project(SPACE)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(ImportDependencies)
importDependencies()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(SCENE_DIR ${CMAKE_CURRENT_LIST_DIR}/source)

file(GLOB_RECURSE SCENE_SRC 
    "${SCENE_DIR}/*.cpp"
)
file(GLOB_RECURSE SCENE_HEADER
    "${SCENE_DIR}/*.h"
)

source_group(TREE ${SCENE_DIR} PREFIX "Source Files" FILES ${SCENE_SRC})
source_group(TREE ${SCENE_DIR} PREFIX "Header Files" FILES ${SCENE_HEADER})

add_executable(SPACE ${SCENE_SRC} ${SCENE_HEADER})

target_include_directories(SPACE PRIVATE ${SCENE_DIR})

# --- Platform Specific Configuration ---

if(EMSCRIPTEN)
    # Web/Emscripten specific settings
    target_compile_definitions(SPACE PRIVATE USE_GLFW)
    
    # Emscripten-specific linker flags
    target_link_options(SPACE PRIVATE
        "-sUSE_GLFW=3"
        "-sUSE_WEBGL2=1"
        "-sFULL_ES3=1"
        "-sWASM=1"
        "-sALLOW_MEMORY_GROWTH=1"
        "--preload-file=${CMAKE_CURRENT_LIST_DIR}/data/assets/shaders@assets"
    )

    # Instruct Emscripten to generate index.html (which implicitly generates index.js, index.wasm, index.data)
    set_target_properties(SPACE PROPERTIES 
        OUTPUT_NAME "index"
        SUFFIX ".html"
    )

elseif(WIN32)
    # Windows/Desktop specific settings
    add_definitions(-DPLATFORM_WINDOWS -DGLEW_STATIC)
    target_compile_definitions(SPACE PRIVATE PLATFORM_WINDOWS GLEW_STATIC)
    
    target_link_libraries(SPACE
        PRIVATE glfw
        PRIVATE libglew_static
    )

    if(MSVC)
        target_compile_options(SPACE PRIVATE /W4 /WX- /permissive- /std:c++17)
    endif()

    set_target_properties(SPACE PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_PROPERTY:SPACE,BINARY_DIR>/$<CONFIG>"
    )

    # Desktop shader asset copy
    add_custom_target(CopyShaderAssets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_LIST_DIR}/data/assets"
            "$<TARGET_PROPERTY:SPACE,BINARY_DIR>/$<CONFIG>/assets"
        COMMENT "Copying shader assets to build directory"
    )
    add_dependencies(SPACE CopyShaderAssets)
endif()

# --- Common Dependencies ---

target_link_libraries(SPACE
    PRIVATE glm::glm
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SPACE)