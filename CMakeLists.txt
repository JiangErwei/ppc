cmake_minimum_required(VERSION 3.21)
project(ppc)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -march=native -std=c++17")

add_executable(ppc
        # main.cpp
#        shortcut.cpp
#        shortcut_memopt.cpp
#        matrix.h
#        shortcut_v0.cpp
#        shortcut_v0.cpp
#        shortcut_v1.cpp
#        shortcut_v2.cpp
        shortcut_v3.cpp
#        demo.cpp
        vector_instructions.cpp
        # demo.cpp
)
