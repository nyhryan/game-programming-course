set(target week1-copy)
add_executable(${target} main.cpp)

target_sources(${target}
    PRIVATE
        ${COMMON_HEADER_DIR}/logger.cpp
)

set_target_properties(${target} PROPERTIES 
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
    CXX_EXTENSIONS OFF
)

target_include_directories(${target} 
    PRIVATE
        ${GLAD_INCLUDE_DIR}
        ${COMMON_HEADER_DIR}
)

target_link_libraries(${target} PRIVATE
    glfw
    glad
    glm::glm
    spdlog::spdlog
)