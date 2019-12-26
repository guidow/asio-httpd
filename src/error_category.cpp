#include "error_category.h"

namespace asio_httpd {

error_category ec;

std::string error_category::message(int ev) const
    {
    if(ev == 0)
        return "No error";

    return "Unknown error";
    }

}
