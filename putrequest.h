#ifndef _ASIOHTTPD_PUTREQUEST
#define _ASIOHTTPD_PUTREQUEST

#include "request.h"

#include <boost/filesystem.hpp>

namespace asio_httpd {

class PutRequest : public Request
    {
    public:
    PutRequest(Server& server) : Request(server) {}
    ~PutRequest() {}

    private:
    };

}

#endif
