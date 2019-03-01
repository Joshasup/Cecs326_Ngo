cmake_minimum_required(VERSION 3.1)
project(Cecs326_Ngo)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(ProbeA a.cpp)
add_executable(ProbeB b.cpp)
add_executable(ProbeC c.cpp)
add_executable(DataHub hub.cpp force_patch64.o)