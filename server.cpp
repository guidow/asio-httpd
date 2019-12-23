#include "server.h"

#include <boost/bind.hpp>

#include "connection.h"

namespace asio_httpd {

Server::Server(boost::asio::io_context& iocontext, uint16_t port)
    : m_iocontext(iocontext),
      m_listenport(port),
      m_acceptor4(iocontext, ip::tcp::endpoint(ip::tcp::v4(), port)),
      m_acceptor6(iocontext)
    {
    m_acceptor6.open(ip::tcp::v6());
    m_acceptor6.set_option(ip::v6_only(true));
    m_acceptor6.bind(ip::tcp::endpoint(ip::tcp::v6(), port));
    }

Server::~Server()
    {}

void Server::start()
    {
    m_acceptor4.listen();
    m_acceptor6.listen();
    accept4();
    accept6();
    }

void Server::quit()
    {
    // TODO
    }

void Server::register_handler(const Path& path, std::shared_ptr<RequestHandlerFactory> handler)
    {
    m_mappings.insert(std::make_pair(path, handler));
    }

void Server::accept_handler4(const boost::system::error_code& error, std::shared_ptr<Connection> conn)
    {
    if(!error)
        {
        conn->start();
        accept4();
        }
    else
        {}
    }

void Server::accept_handler6(const boost::system::error_code& error, std::shared_ptr<Connection> conn)
    {
    if(!error)
        {
        conn->start();
        accept6();
        }
    else
        {}
    }

void Server::accept4()
    {
    auto connection = std::make_shared<Connection>(m_iocontext, *this);
    m_acceptor4.async_accept(connection->socket(), boost::bind(&Server::accept_handler4, this, boost::asio::placeholders::error, connection));
    }

void Server::accept6()
    {
    auto connection = std::make_shared<Connection>(m_iocontext, *this);
    m_acceptor6.async_accept(connection->socket(), boost::bind(&Server::accept_handler6, this, boost::asio::placeholders::error, connection));
    }

boost::asio::io_context& Server::get_io_context()
    {
    return m_iocontext;
    }

std::pair<Path, std::shared_ptr<RequestHandlerFactory> > Server::get_rhf_for(const Path& path) const
    {
    // Paths with more components always compare as greater than paths with fewer components
    // By going through the sorted mapping backwards, we can guarantee that the first match
    // we find is also the longest match.
    for(std::map<Path, std::shared_ptr<RequestHandlerFactory> >::const_reverse_iterator i = m_mappings.rbegin() ; i != m_mappings.rend(); ++i)
        if(i->first.is_in_subtree(path))
            return *i;

    throw HandlerNotFound("No HandlerFactory registered for " + path.str());
    }

}
