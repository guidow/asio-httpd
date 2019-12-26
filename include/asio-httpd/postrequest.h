#ifndef _ASIOHTTPD_POSTREQUEST
#define _ASIOHTTPD_POSTREQUEST

#include "request.h"

namespace asio_httpd {

using namespace boost::asio;

class PostHandler;

class PostRequest : public Request
    {
    public:
    PostRequest(Server& server) : Request(server) {}
    ~PostRequest() {}

    std::size_t expected_payload_bytes() {return m_expected_payload_bytes;}
    void add_payloadblock(std::shared_ptr<std::vector<uint8_t>> block);

    void get_output_block_async(std::array<boost::asio::streambuf, 2>& buffer_pair, boost::function<void (boost::system::error_code)>& callback);
    bool output_left();

    private:
    std::unique_ptr<PostHandler> m_handler;
    std::size_t m_expected_payload_bytes = 0;
    std::string m_payload_content_type;
    bool m_http_headers_written = false;
    bool m_final_chunk_written = false;

    void _set_path(const Path&);
    void _add_http_header(const HttpHeader& header);

    void handle_get_response_block(const boost::system::error_code& err, boost::function<void (boost::system::error_code)>& callback);
    void handle_get_response_block(const boost::system::error_code& err, boost::function<void (boost::system::error_code)>& callback, std::array<boost::asio::streambuf, 2>& buffer_pair);
    };

}

#endif
