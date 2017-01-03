cmake_minimum_required(VERSION 2.8)
project(src)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -m64 -mpclmul -msse4.2")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 -m64 -mpclmul -msse4.2 -O2")

set(SOURCE_FILES
        crc.c
        crc_parallel.c
        crc_parallel.h
        crc_s1024.c
        crc_s1024_p4_.c
        crc_s1024.h
        crc_s1024_p4_.h
        crc_s2048.h
        crc_s2048.c
        cal_crc32c.c)


add_executable(src test.c ${SOURCE_FILES})

add_executable(bench bench.cc ${SOURCE_FILES})
#add_executable(test2  test2.c ${SOURCE_FILES})
#
#
#add_executable(crc_s1024_p4 crc_s1024_p4_.c)
#
#add_executable(crc_s1024 crc_s1024.c)
#
#add_executable(generate_table generate_table.c)
#

