#ifndef _ASIOHTTPD_GETHANDLER_H_
#define _ASIOHTTPD_GETHANDLER_H_

#include "handlerbase.h"

namespace asio_httpd {

class GetHandler : public HandlerBase
    {
    public:
    GetHandler(boost::asio::io_context& io) : HandlerBase(io) {}
    virtual ~GetHandler() {}

    // Override the following (always both, never just one!) if you know the content length
    virtual bool content_length_known();
    virtual uint64_t content_length();

    // Override the following if at all possible

    /* Returns the MIME-type of the returned data as a string.
       The default implementation returns "text/html". */
    virtual std::string content_type();

    /* Returns whether charset() will be able to tell the character
       encoding.
       The default implementation returns false */
    virtual bool charset_known();

    /* Returns the charset of the output.
       Only used if charset_known returns true.
       Default implementation throws CharsetUnknown */
    virtual std::string charset();
    };

}

#endif
