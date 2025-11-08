set(TARGET week6-hw)
add_executable(${TARGET} solarsystem_sun_2planets_hw.cpp)

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
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
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