#ifndef _ASIOHTTPD_HELLOWORLDPOSTHANDLER_H
#define _ASIOHTTPD_HELLOWORLDPOSTHANDLER_H

// Test implementation of a post handler

#include "posthandler.h"

namespace asio_httpd {

class HelloWorldPostHandler : public PostHandler
    {
    public:
    HelloWorldPostHandler(boost::asio::io_context& io) : PostHandler(io) {}
    ~HelloWorldPostHandler() {}

    void set_subpath(const Path& path);

    bool content_length_known() {return false;}
    std::string content_type() {return "text/html";}
    bool charset_known() {return true;}
    std::string charset() {return "utf-8";}

    uint16_t get_returncode();
    std::string get_retmessage();

    bool output_left();
    void get_output_block_async(boost::asio::streambuf& buffer, boost::function<void (boost::system::error_code)>& callback);

    void set_payload_complete();

    private:
    bool m_found = false;
    bool m_message_complete = false;
    };

};

#endif
