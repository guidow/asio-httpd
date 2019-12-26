#include "getrequest.h"
#include "server.h"
#include "error_category.h"
#include "gethandler.h"

#include <boost/bind.hpp>

namespace asio_httpd {

void GetRequest::get_output_block_async(std::array<boost::asio::streambuf, 2>& buffer_pair, boost::function<void (boost::system::error_code)>& callback)
    {
    boost::asio::streambuf& buffer = buffer_pair.at(1);

    if(!m_http_headers_written) {
        // Make response header
        uint16_t http_return_code = m_handler->get_returncode();
        std::string rmessage = m_handler->get_retmessage();
        boost::asio::streambuf& prefixbuffer = buffer_pair.at(0);
        std::ostream out(&prefixbuffer);
        out << m_protocol << " " << int(http_return_code) << " " << rmessage << "\r\n";
        if(m_handler->content_length_known())
            out << "Content-Length: " << m_handler->content_length() << "\r\n";
        else if(m_protocol == "HTTP/1.1" && m_keepalive) { // TODO Also for HTTP protocols > 1.1
            out << "Transfer-Encoding: chunked\r\n";
            m_chunked_output = true;
            }
        else
            m_keepalive = false;

        out << "Content-Type: " << m_handler->content_type();
        if(m_handler->charset_known())
            out << "; charset=" << m_handler->charset() << "\r\n";
        else
            out << "\r\n";
        if(!m_keepalive)
            out << "Connection: close\r\n";
        if(m_protocol != "HTTP/1.1" && m_keepalive)
            out << "Connection: keep-alive\r\n";
        out << "\r\n";

        m_http_headers_written = true;
        }

    if(!m_chunked_output && m_handler->output_left()) {
        boost::function<void (boost::system::error_code)>
            callback2 = boost::bind(&GetRequest::handle_get_response_block, this, _1, callback);
        m_handler->get_output_block_async(buffer, callback2);
        }
    else if(m_chunked_output && m_handler->output_left()) {
        boost::function<void (boost::system::error_code)>
            callback2 = boost::bind(&GetRequest::handle_get_response_block, this, _1, callback, boost::ref(buffer_pair));
        m_handler->get_output_block_async(buffer, callback2);
        }
    else if(m_chunked_output && !m_handler->output_left() && !m_final_chunk_written) {
        // Send a zero-length chunk to signify EOF
        boost::asio::streambuf& prefixbuffer = buffer_pair.at(0);
        std::ostream prefix_out(&prefixbuffer);
        prefix_out << "0\r\n";

        std::ostream out(&buffer);
        out << "\r\n";

        boost::system::error_code err(0, ec);
        m_server.get_io_context().post(boost::bind(callback, err));
        m_final_chunk_written = true;
        }
    }

void GetRequest::_set_path(const Path&)
    {
    auto path_and_factory = m_server.get_endpoint_for(m_path);
    m_handler = path_and_factory.second->createGetHandler();
    Path subtree = path_and_factory.first.difference(m_path);
    m_handler->set_subpath(subtree);
    }

bool GetRequest::output_left()
    {
    return m_handler->output_left() || (m_chunked_output && !m_final_chunk_written);
    }

void GetRequest::handle_get_response_block(const boost::system::error_code& err, boost::function<void (boost::system::error_code)>& callback)
    {
    m_server.get_io_context().post(boost::bind(callback, err));
    }

void GetRequest::handle_get_response_block(const boost::system::error_code& err, boost::function<void (boost::system::error_code)>& callback, std::array<boost::asio::streambuf, 2>& buffer_pair)
    {
    // Prefix the chunk with its size in hex
    std::size_t size = buffer_pair.at(1).size();
    boost::asio::streambuf& prefix_buffer = buffer_pair.at(0);
    std::ostream out(&prefix_buffer);
    out << std::hex << size << "\r\n";

    // Put a \r\n at the end of the data buffer
    boost::asio::streambuf& data_buffer = buffer_pair.at(1);
    std::ostream out2(&data_buffer);
    out2 << "\r\n";

    m_server.get_io_context().post(boost::bind(callback, err));
    }

}
