cmake_minimum_required(VERSION 3.16)
project(jsontool VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json REQUIRED)
find_package(fmt REQUIRED)
find_package(CLI11 REQUIRED)

add_executable(jsontool src/main.cpp)

target_link_libraries(jsontool PRIVATE
    nlohmann_json::nlohmann_json
    fmt::fmt
    CLI11::CLI11
)

install(TARGETS jsontool DESTINATION bin)
