cmake_minimum_required(VERSION 3.16)
project(vacuum_gripper VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

set(TP ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

file(GLOB_RECURSE SRC src/*.cpp)

set(IMGUI_SRC
    ${TP}/imgui/imgui.cpp
    ${TP}/imgui/imgui_draw.cpp
    ${TP}/imgui/imgui_tables.cpp
    ${TP}/imgui/imgui_widgets.cpp
    ${TP}/imgui/backends/imgui_impl_sdl2.cpp
    ${TP}/imgui/backends/imgui_impl_opengl3.cpp
)

set(IMPLOT_SRC
    ${TP}/implot/implot.cpp
    ${TP}/implot/implot_items.cpp
)

add_executable(vacuum_gripper ${SRC} ${IMGUI_SRC} ${IMPLOT_SRC})

target_include_directories(vacuum_gripper PRIVATE
    src
    ${TP}/imgui
    ${TP}/imgui/backends
    ${TP}/implot
)

target_compile_definitions(vacuum_gripper PRIVATE
    THIRD_PARTY_DIR="${CMAKE_CURRENT_SOURCE_DIR}/third_party")

target_link_libraries(vacuum_gripper PRIVATE SDL2::SDL2 ${CMAKE_DL_LIBS})

if(UNIX AND NOT APPLE)
    target_link_libraries(vacuum_gripper PRIVATE GL)
endif()
