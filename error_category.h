#ifndef _ASIOHTTPD_ERROR_CATEGORY
#define _ASIOHTTPD_ERROR_CATEGORY

#include <boost/system/error_code.hpp>

namespace asio_httpd {

class error_category : public boost::system::error_category
    {
    public:
    const char* name() const BOOST_SYSTEM_NOEXCEPT {return "asio_httpd";}
    std::string message(int ev) const;
    };

extern error_category ec;

}

#endif
