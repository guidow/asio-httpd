#ifndef _ASIOHTTPD_POSTHANDLER_H_
#define _ASIOHTTPD_POSTHANDLER_H_

#include "handlerbase.h"

namespace asio_httpd {

class PostHandler : public HandlerBase
    {
    public:
    PostHandler(boost::asio::io_service& io) : HandlerBase(io) {}
    virtual ~PostHandler() {}

    // Override the following (always both, never just one!) if you know the content length
    virtual bool content_length_known();
    virtual uint64_t content_length();

    // Override the following if at all possible
    virtual std::string content_type();
    virtual bool charset_known();
    virtual std::string charset();

    // The default implementation will append all blocks to m_payload
    // This involves copying of data and is not too efficient
    // TODO: Allow handlers to provide their own buffers
    virtual void add_payloadblock(std::shared_ptr<std::vector<uint8_t>> block);

    // Will be called after all payload blocks have been added
    virtual void set_payload_complete() = 0;

    protected:
    std::vector<uint8_t> m_payload;
    };

}

#endif
