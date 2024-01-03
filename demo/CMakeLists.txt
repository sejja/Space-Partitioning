cmake_minimum_required(VERSION 3.8)
project(octree_demo)

# Demos
add_executable(${PROJECT_NAME} main.cpp imgui.hpp scene.cpp scene.hpp)
target_link_libraries(${PROJECT_NAME} PUBLIC engine)

# IMGUI: vcpkg install imgui[glfw-binding,opengl3-binding]:x64-windows
find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)