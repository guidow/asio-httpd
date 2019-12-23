#ifndef _ASIOHTTPD_DELETEREQUEST
#define _ASIOHTTPD_DELETEREQUEST

#include "request.h"

#include <boost/filesystem.hpp>

namespace asio_httpd {

class DeleteRequest : public Request
    {
    public:
    DeleteRequest(Server& server) : Request(server) {}
    ~DeleteRequest() {}

    private:
    };

}

#endif
