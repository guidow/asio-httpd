cmake_minimum_required(VERSION 3.13)

project(asio-httpd VERSION 0.0.1)

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
target_include_directories(asio-httpd PRIVATE .)
target_include_directories(asio-httpd PRIVATE include/asio-httpd/)
set_target_properties(asio-httpd PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(asio-httpd PROPERTIES SOVERSION 0)
set_target_properties(asio-httpd PROPERTIES PUBLIC_HEADER
            include/asio-httpd/deletehandler.h
            include/asio-httpd/deleterequest.h
            include/asio-httpd/error_category.h
            include/asio-httpd/gethandler.h
            include/asio-httpd/getrequest.h
            include/asio-httpd/handlerbase.h
            include/asio-httpd/httpheader.h
            include/asio-httpd/path.h
            include/asio-httpd/posthandler.h
            include/asio-httpd/postrequest.h
            include/asio-httpd/puthandler.h
            include/asio-httpd/putrequest.h
            include/asio-httpd/request.h
            include/asio-httpd/requesthandlerfactory.h
            include/asio-httpd/server.h)
target_include_directories(asio-httpd PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(asio-httpd ${Boost_LIBRARIES})

add_subdirectory(examples)
