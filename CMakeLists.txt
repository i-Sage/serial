cmake_minimum_required(VERSION 3.20)

project(ply_gnd)

find_package(Boost REQUIRED COMPONENTS asio)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_executable(main "src/main.cpp" "src/serial.hpp")
target_link_libraries(main PRIVATE Boost::asio)
