#ifndef _ASIOHTTPD_CONNECTION
#define _ASIOHTTPD_CONNECTION

#include <array>
#include <memory>

#include <boost/asio.hpp>

#include "request.h"

namespace asio_httpd {

class Server;

using namespace boost::asio;

class Connection : public std::enable_shared_from_this<Connection>
    {
    public:
    Connection(io_context& io, Server& server);
    ~Connection();

    void start();

    ip::tcp::socket& socket() {return m_socket;}

    void readrequestline();
    void handle_readrequestline(const boost::system::error_code& err);
    void readheaderfield();
    void handle_readheaderfield(const boost::system::error_code& err);

    void readpayload();
    void handle_readpayloadblock(std::shared_ptr<std::vector<uint8_t>> block, const boost::system::error_code& err, std::size_t);

    void handle_get_response_block(const boost::system::error_code& err);
    void handle_write_block(const boost::system::error_code& err, std::size_t bytes_transferred);
    void handle_write_block_prefix(const boost::system::error_code& err, std::size_t bytes_transferred);

    class BadRequest : public std::runtime_error
        {public: BadRequest(const std::string & msg): runtime_error(msg) {}};

    private:
    std::pair<std::string, std::string> parsekv(const std::string & in);

    ip::tcp::socket m_socket;
    Server& m_server;
    std::shared_ptr<Request> m_currentrequest;

    streambuf m_readbuf;
    // Two writebuffers for output header and output body
    std::array<streambuf, 2> m_writebuffers;
    std::pair<std::string, std::string> m_curkv;
    };

}

#endif
