#include "posthandler.h"

namespace asio_httpd
{

bool PostHandler::content_length_known()
    {
    return false;
    }

uint64_t PostHandler::content_length()
    {
    throw HandlerBase::LengthUnknown("Content Length not known");
    }

bool PostHandler::charset_known()
    {
    return false;
    }

std::string PostHandler::charset()
    {
    throw HandlerBase::CharsetUnknown("Charset is not known");
    }

std::string PostHandler::content_type()
    {
    return "text/html";
    }

void PostHandler::add_payloadblock(std::shared_ptr<std::vector<uint8_t>> block)
    {
    m_payload.insert(m_payload.end(), block->begin(), block->end());
    }

};
