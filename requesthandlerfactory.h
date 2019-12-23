#ifndef _ASIOHTTPD_REQUESTHANDLERFACTORY_H_
#define _ASIOHTTPD_REQUESTHANDLERFACTORY_H_

#include <memory>

#include <boost/asio.hpp>

#include "gethandler.h"
#include "posthandler.h"
#include "puthandler.h"
#include "deletehandler.h"

namespace asio_httpd {

class RequestHandlerFactory
    {
    public:
    RequestHandlerFactory(boost::asio::io_context& io) : m_io(io) {}
    ~RequestHandlerFactory() {}

    virtual std::unique_ptr<GetHandler> createGetHandler() {throw MethodNotSupported("Get method not supported");}
    virtual std::unique_ptr<PostHandler> createPostHandler() {throw MethodNotSupported("Post method not supported");}
    virtual std::unique_ptr<PutHandler> createPutHandler() {throw MethodNotSupported("Put method not supported");}
    virtual std::unique_ptr<DeleteHandler> createDeleteHandler() {throw MethodNotSupported("Delete method not supported");}

    class MethodNotSupported : public std::runtime_error
        {
        public: MethodNotSupported(const std::string & msg) : runtime_error(msg) {}
        };

    protected:
    boost::asio::io_context& m_io;
    };

}

#endif
