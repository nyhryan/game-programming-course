include(FetchContent)

cmake_minimum_required(VERSION 3.30)
project("game-programming")

set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED TRUE)
set(CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    add_compile_options(
        "/Zc:preprocessor"
        "/wd4819"
    )
endif()

# glfw
FetchContent_Declare(
    glfw
    GIT_REPOSITORY  https://github.com/glfw/glfw.git
    GIT_TAG         7b6aead9fb88b3623e3b3725ebb42670cbe4c579
    SOURCE_DIR      vendor/glfw
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# glm
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	    0af55ccecd98d4e5a8d1fad7de25ba429d60e863
    SOURCE_DIR vendor/glm
)

# glad 
set(GLAD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/vendor/glad/include)
add_subdirectory(vendor/glad)

# spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY	https://github.com/gabime/spdlog.git
	GIT_TAG 	    6fa36017cfd5731d617e1a934f0e5ea9c4445b13
    SOURCE_DIR vendor/spdlog
)

FetchContent_MakeAvailable(glfw glm spdlog)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/vendor/imgui)

set(COMMON_HEADER_DIR ${CMAKE_SOURCE_DIR}/projects/common)
set(RESOURCES_DIR ${CMAKE_SOURCE_DIR}/resources)

add_subdirectory(projects/week1)
add_subdirectory(projects/week1-copy)
add_subdirectory(projects/week2)
add_subdirectory(projects/week4)
add_subdirectory(projects/week5)
add_subdirectory(projects/week5-2)
add_subdirectory(projects/week7)
add_subdirectory(projects/week6-hw)
add_subdirectory(projects/week10)
add_subdirectory(projects/week10-hw)
add_subdirectory(projects/week11)
add_subdirectory(projects/week11-hw)
add_subdirectory(projects/week12)
add_subdirectory(projects/week13)
