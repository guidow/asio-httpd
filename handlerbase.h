#ifndef _ASIOHTTPD_HANDLERBASE_H_
#define _ASIOHTTPD_HANDLERBASE_H_

#include <boost/function.hpp>
#include <boost/system/error_code.hpp>

#include <stdint.h>

#include "path.h"
#include <boost/asio.hpp>

namespace asio_httpd {

class HandlerBase
    {
    public:
    HandlerBase(boost::asio::io_context& io) : m_io(io) {}
    ~HandlerBase() {}

    virtual void set_subpath(const Path& path) {m_subpath = path;}
    virtual void set_additional_params(const std::string& params) {m_additional_params = params;}

    virtual uint16_t get_returncode() = 0;
    virtual std::string get_retmessage() = 0;
    virtual bool content_length_known() = 0;
    virtual uint64_t content_length() = 0;
    virtual std::string content_type() = 0;
    virtual bool charset_known() = 0;
    virtual std::string charset() = 0;
    // Must be overwritten in actual handlers
    virtual bool output_left() = 0;
    virtual void get_output_block_async(boost::asio::streambuf& buffer, boost::function<void (boost::system::error_code)>& callback) = 0;

    class LengthUnknown : public std::runtime_error
        {public: LengthUnknown(const std::string& msg) : runtime_error(msg) {}};

    class CharsetUnknown : public std::runtime_error
        {public: CharsetUnknown(const std::string& msg) : runtime_error(msg) {}};

    protected:
    Path m_subpath;
    std::string m_additional_params;
    boost::asio::io_context& m_io;
    };

}

#endif
