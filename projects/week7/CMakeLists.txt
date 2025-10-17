set(TARGET week7)
add_executable(${TARGET} j13.human.cpp)

target_compile_definitions(${TARGET} PRIVATE
    RESOURCE_PATH_PREFIX="${RESOURCES_DIR}/"
)

target_sources(${TARGET}
    PRIVATE
        ${COMMON_HEADER_DIR}/logger.hpp
        ${COMMON_HEADER_DIR}/logger.cpp
        ${COMMON_HEADER_DIR}/shader.hpp
        ${COMMON_HEADER_DIR}/shader.cpp
        ${COMMON_HEADER_DIR}/utility.hpp
        ${COMMON_HEADER_DIR}/type.hpp
        j13.human.h
)

set_target_properties(${TARGET} PROPERTIES 
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
    CXX_EXTENSIONS OFF
)

if(MSVC)
    target_compile_options(${TARGET} PRIVATE
        "/Zc:preprocessor"
        "/wd4819"
    )
endif()

target_include_directories(${TARGET} 
    PRIVATE
        ${GLAD_INCLUDE_DIR}
        ${COMMON_HEADER_DIR}
        ${IMGUI_DIR}
)

target_link_libraries(${TARGET} PRIVATE
    glfw
    glad
    glm::glm
    spdlog::spdlog
)