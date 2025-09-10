set(TARGET week2)
set(RESOURCES shader.vert shader.frag solarsystem_planet.fs solarsystem_planet.vs solarsystem_star.fs solarsystem_star.vs
2k_earth_daymap.jpg 2k_jupiter.jpg 2k_mars.jpg 2k_mercury.jpg 2k_moon.jpg 2k_neptune.jpg 2k_saturn.jpg 2k_sun.jpg 2k_uranus.jpg 2k_venus_atmosphere.jpg 2k_venus_surface.jpg)
# add_executable(${TARGET} main.cpp)
add_executable(${TARGET} solarsystem_sun_2planets_hw.cpp)

foreach(resource ${RESOURCES})
    configure_file(${resource} ${CMAKE_CURRENT_BINARY_DIR}/${resource} COPYONLY)
endforeach()

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