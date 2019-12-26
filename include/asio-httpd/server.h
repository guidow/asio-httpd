#ifndef _ASIOHTTPD_HTTPSERVER
#define _ASIOHTTPD_HTTPSERVER

#include <memory>

#include <boost/asio.hpp>

#include "endpoint.h"
#include "path.h"

namespace asio_httpd {

using namespace boost::asio;

class Connection;

class Server
    {
    public:
    Server(io_context& iocontext, uint16_t port);
    ~Server();

    void start();
    void quit();

    void register_endpoint(const Path& path, std::shared_ptr<Endpoint> handler);

    void accept4();
    void accept6();

    io_context& get_io_context();

    std::pair<Path, std::shared_ptr<Endpoint> > get_endpoint_for(const Path& path) const;

    class HandlerNotFound : public std::runtime_error
        {
        public: HandlerNotFound(const std::string& msg) : runtime_error(msg) {}
        };

    private:
    void accept_handler4(const boost::system::error_code& error, std::shared_ptr<Connection> conn);
    void accept_handler6(const boost::system::error_code& error, std::shared_ptr<Connection> conn);

    io_context& m_iocontext;
    uint16_t m_listenport;
    ip::tcp::acceptor m_acceptor4;
    ip::tcp::acceptor m_acceptor6;
    std::map<Path, std::shared_ptr<Endpoint>> m_mappings;
    };

}

#endif
