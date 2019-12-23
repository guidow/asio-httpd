#include "helloworldposthandler.h"

#include "error_category.h"

#include <iostream>

#include <boost/bind.hpp>

namespace asio_httpd {

extern asio_httpd::error_category ec;

void HelloWorldPostHandler::set_subpath(const Path& path)
    {
    asio_httpd::HandlerBase::set_subpath(path);

    if(path.num_elements() == 0)
        m_found = true;
    else
        m_found = false;
    }

uint16_t HelloWorldPostHandler::get_returncode()
    {
    if(m_found)
        return 200;
    else
        return 404;
    }

std::string HelloWorldPostHandler::get_retmessage()
    {
    if(m_found)
        return "OK";
    else
        return "Not Found";
    }

bool HelloWorldPostHandler::output_left()
    {
    return !m_message_complete;
    }

void HelloWorldPostHandler::get_output_block_async(boost::asio::streambuf& buffer, boost::function<void (boost::system::error_code)>& callback)
    {
    std::ostream out(&buffer);
    if(!m_found)
        out << "<html><head></head><body>Not found</body></html>";

    boost::system::error_code err(0, ec);
    m_io.post(boost::bind(callback, err));
    }

void HelloWorldPostHandler::set_payload_complete()
    {
    std::cerr << "Got payload: " << std::string(m_payload.begin(), m_payload.end()) << std::endl;
    }

};
