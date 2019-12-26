#ifndef _ASIOHTTPD_HELLOWORLDREQUESTHANDLERFACTORY_H_
#define _ASIOHTTPD_HELLOWORLDREQUESTHANDLERFACTORY_H_

#include <memory>

#include <asio-httpd/requesthandlerfactory.h>

#include "helloworldgethandler.h"
#include "helloworldposthandler.h"

namespace asio_httpd {

class HelloWorldRequestHandlerFactory : public RequestHandlerFactory
    {
    public:
    HelloWorldRequestHandlerFactory(boost::asio::io_context& io) : RequestHandlerFactory(io) {}
    virtual ~HelloWorldRequestHandlerFactory() {}

    std::unique_ptr<GetHandler> createGetHandler() {return std::make_unique<HelloWorldGetHandler>(m_io);}
    std::unique_ptr<PostHandler> createPostHandler() {return std::make_unique<HelloWorldPostHandler>(m_io);}
    };

}

#endif
