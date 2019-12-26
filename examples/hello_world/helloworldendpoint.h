#ifndef _ASIOHTTPD_HELLOWORLDREQUESTHANDLERFACTORY_H_
#define _ASIOHTTPD_HELLOWORLDREQUESTHANDLERFACTORY_H_

#include <memory>

#include <asio-httpd/endpoint.h>

#include "helloworldgethandler.h"
#include "helloworldposthandler.h"

namespace httpd_hello_world {

class HelloWorldEndpoint : public Endpoint
    {
    public:
    HelloWorldEndpoint(boost::asio::io_context& io) : Endpoint(io) {}
    virtual ~HelloWorldEndpoint() {}

    std::unique_ptr<GetHandler> createGetHandler() {return std::make_unique<HelloWorldGetHandler>(m_io);}
    std::unique_ptr<PostHandler> createPostHandler() {return std::make_unique<HelloWorldPostHandler>(m_io);}
    };

}

#endif
