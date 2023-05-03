cmake_minimum_required(VERSION 3.9)

project(sync_qiuck_test)

# C standard can be overridden when this is used as a sub-project.
if(NOT CMAKE_C_STANDARD)
  # This project can use C11, but will gracefully decay down to C89.
  set(CMAKE_C_STANDARD 11)
  set(CMAKE_C_STANDARD_REQUIRED OFF)
  set(CMAKE_C_EXTENSIONS OFF)
endif(NOT CMAKE_C_STANDARD)

# C++ standard can be overridden when this is used as a sub-project.
if(NOT CMAKE_CXX_STANDARD)
  # This project requires C++11.
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif(NOT CMAKE_CXX_STANDARD)


include_directories("/opt/homebrew/Cellar/grpc/1.52.1/include")
include_directories("/opt/homebrew/Cellar/protobuf/21.12/include")

add_executable(sync_qiuck_test "main.cpp")


target_include_directories(sync_qiuck_test PUBLIC "${PROJECT_SOURCE_DIR}/include/")
target_include_directories(sync_qiuck_test PUBLIC "${PROJECT_SOURCE_DIR}/sync")
target_include_directories(sync_qiuck_test PUBLIC "${PROJECT_SOURCE_DIR}/proto")

target_link_libraries(
    sync_qiuck_test 
    "${PROJECT_SOURCE_DIR}/lib/libleveldb.a" 
    "${PROJECT_SOURCE_DIR}/lib/libnuraft.a"
)
