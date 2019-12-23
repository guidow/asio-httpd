add_executable(httpd_test helloworldgethandler.cpp helloworldposthandler.cpp main.cpp)

target_link_libraries(httpd_test asio-httpd Threads::Threads)
