#ifndef _ASIOHTTPD_GETREQUEST
#define _ASIOHTTPD_GETREQUEST

#include "request.h"
#include "path.h"

namespace asio_httpd {

class GetHandler;

class GetRequest : public Request
    {
    public:
    GetRequest(Server& server) : Request(server) {}
    virtual ~GetRequest() {}

    void get_output_block_async(std::array<boost::asio::streambuf, 2>& buffer_pair, boost::function<void (boost::system::error_code)>& callback);
    bool output_left();

    private:
    std::unique_ptr<GetHandler> m_handler;
    bool m_http_headers_written = false;
    bool m_final_chunk_written = false;

    void _set_path(const Path&);

    void handle_get_response_block(const boost::system::error_code& err, boost::function<void (boost::system::error_code)>& callback);
    void handle_get_response_block(const boost::system::error_code& err, boost::function<void (boost::system::error_code)>& callback, std::array<boost::asio::streambuf, 2>& buffer_pair);
    };

}

#endif
