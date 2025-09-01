set(target week1)
add_executable(week1 main.cpp)

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