cmake_minimum_required(VERSION 3.17)
project(superfs C)

set(CMAKE_C_STANDARD 90)

add_executable(superfs main.c physdisk.c filesystem.h)