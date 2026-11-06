cmake_minimum_required(VERSION 3.8...3.27)
project(cli_install_consumer LANGUAGES CXX)

find_package(cli CONFIG REQUIRED)
add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE cli::cli)
