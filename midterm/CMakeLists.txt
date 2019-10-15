cmake_minimum_required(VERSION 3.14)
project(midterm)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(midterm
    cmp.c
    dlist.c
    dlist.h
    ls.c
    ls.h
        print.c
    Makefile)
