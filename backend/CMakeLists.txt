cmake_minimum_required(VERSION 3.18)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(THREAD_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

set(PROJECT_NAME "bvh-builder")

project(${PROJECT_NAME})

set(SOURCES main.cpp mesh.cpp aabb.cpp matrix3x3.cpp triangular.cpp divider.cpp bvh.cpp fifo.cpp tcp-server.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
	Threads::Threads
	)

