# ===========================================================================
# ImportDependencies.cmake
#
# Downloads GLFW + GLEW for the desktop build via FetchContent.
# Android and Emscripten have built-in GL support and skip this.
# ===========================================================================

include(FetchContent)

function(importDependencies)

    message(STATUS "[ImportDependencies] Fetching GLM ...")
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        master
        GIT_SHALLOW    TRUE
    )
    set(GLM_BUILD_TESTS     OFF CACHE BOOL "" FORCE)
    set(GLM_ENABLE_TESTS    OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING       OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glm)

    if(ANDROID OR EMSCRIPTEN)
        message(STATUS "[ImportDependencies] Skipping GLFW and GLEW (using platform built-ins).")
        return()
    endif()

    message(STATUS "[ImportDependencies] Fetching GLFW ...")
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        master
        GIT_SHALLOW    TRUE
    )
    set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)

    message(STATUS "[ImportDependencies] Fetching GLEW ...")
    FetchContent_Declare(
        glew
        GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
        GIT_TAG        glew-cmake-2.3.1
        GIT_SHALLOW    FALSE
    )
    set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(glew-cmake_BUILD_STATIC ON  CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glew)

    message(STATUS "[ImportDependencies] All desktop dependencies ready.")

endfunction()
