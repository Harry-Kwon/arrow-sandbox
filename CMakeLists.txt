cmake_minimum_required(VERSION 3.22)

project(arrow-sandbox
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(Arrow REQUIRED)

add_subdirectory(src)
