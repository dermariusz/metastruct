cmake_minimum_required (VERSION 3.20)
cmake_policy(VERSION 3.20)

project(metastruct C)

set (CMAKE_C_STANDARD 11)

set( METASTRUCT-SRC
     src/metastruct.c
     src/main.c
     include/metastruct.h
)
     
add_executable(metastruct ${METASTRUCT-SRC})

target_include_directories(metastruct PUBLIC include/)
