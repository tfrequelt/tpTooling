cmake_minimum_required(VERSION 3.15)
project(MyTool)

set(CMAKE_CXX_STANDARD 17)

# Ajouter le chemin vers vcpkg
set(CMAKE_TOOLCHAIN_FILE "C:/Users/titou/RiderProjects/tpTooling/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

# Trouver et lier nlohmann-json
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(MyTool main.cpp)
target_link_libraries(MyTool PRIVATE nlohmann_json::nlohmann_json)
