cmake_minimum_required(VERSION 3.5)

project(meowings)


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -O3)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/server)
include_directories(${CMAKE_SOURCE_DIR}/include/client)
include_directories(${CMAKE_SOURCE_DIR}/include/db)
include_directories(${CMAKE_SOURCE_DIR}/include/model)
include_directories(${CMAKE_SOURCE_DIR}/third_party)


add_subdirectory(src)