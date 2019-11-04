cmake_minimum_required(VERSION 3.15)
project(fst_label_push)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g -O3")

include_directories(/usr/local/include)

add_executable(fst_label_push main.cpp)
target_link_directories(fst_label_push PUBLIC /usr/local/lib)
target_link_libraries(fst_label_push fst)