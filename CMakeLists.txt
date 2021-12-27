cmake_minimum_required(VERSION 3.5)

project(meowings)


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SORUCE_DIR}/bin)

# message("${PROJECT_SOURCE_DIR}")
# message("${CMAKE_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/server)
include_directories(${CMAKE_SOURCE_DIR}/include/server/db)
include_directories(${CMAKE_SOURCE_DIR}/include/server/model)
include_directories(${CMAKE_SOURCE_DIR}/third_party)


add_subdirectory(src)