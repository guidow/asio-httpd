#include "helloworldgethandler.h"

#include "asio-httpd/error_category.h"

#include <boost/bind.hpp>

namespace asio_httpd {

extern asio_httpd::error_category ec;

void HelloWorldGetHandler::set_subpath(const Path& path)
    {
    asio_httpd::HandlerBase::set_subpath(path);

    if(path.num_elements() == 0)
        {
        m_found = true;
        m_msg = "<html><head></head><body>"
                "<h1>Hello World</h1>"
                "<p><form action='/' method='post'>"
                "<input type='text' name='value'/>"
                "<input type='text' name='value2'/>"
                "<input type='submit'/>"
                "</post></p>"
                "</body></html>";
        }
    else
        {
        m_found = false;
        m_msg = "<html><head></head><body>The document you requested was not found</body></html>";
        }
    }

uint16_t HelloWorldGetHandler::get_returncode()
    {
    if(m_found)
        return 200;
    else
        return 404;
    }

std::string HelloWorldGetHandler::get_retmessage()
    {
    if(m_found)
        return "OK";
    else
        return "Not Found";
    }

bool HelloWorldGetHandler::output_left()
    {
    return !m_msg.empty();
    }

void HelloWorldGetHandler::get_output_block_async(boost::asio::streambuf& buffer, boost::function<void (boost::system::error_code)>& callback)
    {
    // Return the output one character at a time
    // This is ridiculously inefficient. We only do this to demonstrate that chunked transfer encoding works
    std::ostream out(&buffer);
    std::string c = m_msg.substr(0, 1);
    m_msg.erase(0, 1);
    out << c;

    boost::system::error_code err(0, ec);
    m_io.post(boost::bind(callback, err));
    }

};
