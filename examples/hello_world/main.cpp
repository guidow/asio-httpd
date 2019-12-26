#include <memory>

#include <asio-httpd/server.h>

#include "helloworldendpoint.h"

int main(int, char**) 
    {
    boost::asio::io_context io;

    auto endpoint = std::make_shared<httpd_hello_world::HelloWorldEndpoint>(io);

    asio_httpd::Server server(io, 8080);
    server.register_endpoint(asio_httpd::Path("/"), endpoint);
    server.start();

    io.run();

    return 0;
    }
