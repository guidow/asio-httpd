#include "request.h"

#include <iostream>

#include "server.h"
#include "connection.h"

namespace asio_httpd {

void Request::set_path(const std::string& path)
    {
    // TODO Support full URIs as paths, as mandated by RFC 2616 section 5.1.1
    std::string path_copy = path;
    std::string::size_type pos = path_copy.find("#");
    if(pos != std::string::npos)
        path_copy.erase(pos);

    std::string params;
    pos = path_copy.find("?");
    if(pos != std::string::npos)
        {
        if(path_copy.size() > pos)
            params = path_copy.substr(pos + 1);
        path_copy.erase(pos);
        }

    m_path = url_decode(path_copy);
    _set_path(path_copy);
    m_params = params;
    _set_parameters(params);
    }

void Request::set_protocol(const std::string& protocol)
    {
    m_protocol = protocol;

    if(protocol == "HTTP/1.1") // HTTP 1.1 defaults to keepalive
        m_keepalive = true;
    else
        m_keepalive = false;

    _set_protocol(protocol);
    }

void Request::add_http_header(const HttpHeader& header)
    {
    if(m_protocol == "HTTP/1.1" && header.key == "connection" && header.value == "close")
        m_keepalive = false;
    if(m_protocol != "HTTP/1.1" && header.key == "keep-alive" && header.value == "true")
        m_keepalive = true;

    _add_http_header(header);
    }

std::string Request::url_decode(const std::string& url)
    {
    std::string out;

    for(std::string::const_iterator i = url.begin() ; i != url.end() ; ++i)
        {
        if(*i != '%' && *i != '+')
            out.append(1, *i);
        else if(*i == '+')
            out.append(1, ' ');
        else
            {
            char curchar = 0;
            ++i;
            if(i == url.end())
                throw Connection::BadRequest("% in URL not followed by two HEX digits");
            if(*i > '0' && *i <= '9')
                curchar = *i - '0';
            else if(*i > 'a' && *i <= 'f')
                curchar = (*i - 'a') + 10;
            else if(*i > 'A' && *i <= 'F')
                curchar = (*i - 'A') + 10;
            else
                throw Connection::BadRequest("% in URL not followed by two HEX digits");
            curchar = curchar << 4;

            // Second digit
            ++i;
            if(i == url.end())
                throw Connection::BadRequest("% in URL not followed by two HEX digits");
            if(*i > '0' && *i <= '9')
                curchar |= *i - '0';
            else if(*i > 'a' && *i <= 'f')
                curchar |= (*i - 'a') + 10;
            else if(*i > 'A' && *i <= 'F')
                curchar |= (*i - 'A') + 10;
            else
                throw Connection::BadRequest("% in URL not followed by two HEX digits");

            out.append(1, curchar);
            }
        }

    return out;
    }

}
