cmake_minimum_required(VERSION 3.13)

project(asio-httpd)

if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif(CMAKE_COMPILER_IS_GNUCC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.70.0 COMPONENTS system REQUIRED)
find_package(Threads REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

add_library(asio-httpd SHARED
            path.cpp error_category.cpp connection.cpp server.cpp
            request.cpp getrequest.cpp postrequest.cpp
            gethandler.cpp posthandler.cpp)

target_include_directories(asio-httpd PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(asio-httpd ${Boost_LIBRARIES})

add_subdirectory(examples)
