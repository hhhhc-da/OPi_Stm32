cmake_minimum_required(VERSION 3.5)

# Compile Commands

# cmake -G "Unix Makefiles" .
# make

project(project)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(SRC_LIST
    MyProject.cpp
    MyProject.h
    picojson/picojson.h
)

add_executable(${PROJECT_NAME} ${SRC_LIST})

# after exectable file
target_link_libraries(${PROJECT_NAME} /home/orangepi/File/C++/Project/libxl/lib-aarch64/libxl.so)
target_link_libraries(${PROJECT_NAME} wiringPi)

add_compile_options(-Wall -o0)

message(${PROJECT_NAME})