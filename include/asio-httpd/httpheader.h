#ifndef _ASIOHTTPDHTTPHEADER_H
#define _ASIOHTTPDHTTPHEADER_H

#include <string>

namespace asio_httpd {

struct HttpHeader
    {
    HttpHeader(const std::string& key_, const std::string& value_)
        : key(key_), value(value_) {}

    std::string key;
    std::string value;
    };

}

#endif
