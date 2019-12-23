#ifndef _ASIOHTTPD_REQUEST
#define _ASIOHTTPD_REQUEST

#include <string>
#include <memory>

#include <boost/function.hpp>
#include <boost/asio.hpp>

#include "path.h"
#include "httpheader.h"

namespace asio_httpd {

class Server;

using namespace boost::asio;

class Request
    {
    public:
    Request(Server& server) : m_server(server) {}
    virtual ~Request() {}

    void set_path(const std::string & path);
    void set_protocol(const std::string & protocol);

    void add_http_header(const HttpHeader& header);

    virtual std::size_t expected_payload_bytes() {return 0;}
    virtual void add_payloadblock(std::shared_ptr<std::vector<uint8_t>>) {throw std::runtime_error("This request does not expect any payload data");}

    virtual void get_output_block_async(std::array<boost::asio::streambuf, 2>& buffer_pair, boost::function<void (boost::system::error_code)>& callback) = 0;
    virtual bool output_left() = 0;

    bool keepalive_requested() {return m_keepalive;}

    protected:
    // The following can be overridden in derived classes to define further actions
    // when one of these things is set
    virtual void _set_path(const Path&) {}
    virtual void _set_parameters(const std::string&) {}
    virtual void _set_protocol(const std::string&) {} // HTTP 1.1, 1.0 or 0.9
    virtual void _add_http_header(const HttpHeader&) {}

    Path m_path;
    std::string m_params;
    std::string m_protocol;
    std::string m_vhost;
    bool m_keepalive = false;
    bool m_chunked_output = false;

    Server& m_server;

    std::string url_decode(const std::string& url);
    };

}

#endif
