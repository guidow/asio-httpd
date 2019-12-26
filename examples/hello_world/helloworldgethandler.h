#ifndef _ASIOHTTPD_HELLOWORLDGETHANDLER_H_
#define _ASIOHTTPD_HELLOWORLDGETHANDLER_H_

// Test implementation of a get handler that just sends hello world

#include <asio-httpd/gethandler.h>

namespace httpd_hello_world {
using namespace asio_httpd;

class HelloWorldGetHandler : public GetHandler
    {
    public:
    HelloWorldGetHandler(boost::asio::io_context& io) : GetHandler(io) {}
    ~HelloWorldGetHandler() {}

    void set_subpath(const Path& path);

    bool content_length_known() {return false;}
    std::string content_type() {return "text/html";}
    bool charset_known() {return true;}
    std::string charset() {return "utf-8";}

    uint16_t get_returncode();
    std::string get_retmessage();

    bool output_left();
    void get_output_block_async(boost::asio::streambuf& buffer, boost::function<void (boost::system::error_code)>& callback);

    private:
    bool m_found = false;
    std::string m_msg;
    };

};

#endif
