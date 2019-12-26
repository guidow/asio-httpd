#include <memory>

#include <asio-httpd/server.h>

#include "helloworldrequesthandlerfactory.h"

int main(int, char**) 
    {
    boost::asio::io_context io;

    auto factory = std::make_shared<asio_httpd::HelloWorldRequestHandlerFactory>(io);

    asio_httpd::Server server(io, 8080);
    server.register_handler(asio_httpd::Path("/"), factory);
    server.start();

    io.run();

    return 0;
    }
