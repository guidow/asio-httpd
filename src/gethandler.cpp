#include "gethandler.h"

namespace asio_httpd
{

bool GetHandler::content_length_known()
    {
    return false;
    }

uint64_t GetHandler::content_length()
    {
    throw HandlerBase::LengthUnknown("Content Length not known");
    }

bool GetHandler::charset_known()
    {
    return false;
    }

std::string GetHandler::charset()
    {
    throw HandlerBase::CharsetUnknown("Charset is not known");
    }

std::string GetHandler::content_type()
    {
    return "text/html";
    }

};
