#ifndef _ASIOHTTPD_GETHANDLER_H_
#define _ASIOHTTPD_GETHANDLER_H_

#include "handlerbase.h"

namespace asio_httpd {

class GetHandler : public HandlerBase
    {
    public:
    GetHandler(boost::asio::io_context& io) : HandlerBase(io) {}
    ~GetHandler() {}

    // Override the following (always both, never just one!) if you know the content length
    virtual bool content_length_known();
    virtual uint64_t content_length();

    // Override the following if at all possible
    virtual std::string content_type();
    virtual bool charset_known();
    virtual std::string charset();
    };

}

#endif
