#include "connection.h"

#include <algorithm>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "server.h"
#include "httpheader.h"
#include "getrequest.h"
#include "postrequest.h"
#include "putrequest.h"
#include "deleterequest.h"

#include <iostream>

namespace asio_httpd {

using namespace boost::asio;

// Make buffers slightly larger than a typical MTU
const std::size_t standard_buffer_size = 1500;

Connection::Connection(io_context & io, Server & server)
    : m_socket(io)
    , m_server(server)
    {}

Connection::~Connection() {}

void Connection::start()
    {
    // TODO install timer for timeouts
    readrequestline();
    }

void Connection::readrequestline()
    {
    async_read_until(m_socket, m_readbuf, '\n', boost::bind(&Connection::handle_readrequestline,
                                                            shared_from_this(),
                                                            boost::asio::placeholders::error));
    }

void Connection::handle_readrequestline(const boost::system::error_code& err)
    {
    if(!err)
        {
        try {
        std::istream input(&m_readbuf);
        std::string line;
        std::getline(input, line);

        if(!line.empty() && line.at(line.length() - 1) == '\r') // If last char is carriage return
            line.erase(line.length() - 1); // Remove it

        std::string::size_type pos = line.find(' ');
        if(pos == std::string::npos)
            throw BadRequest("No resource given");
        std::string rqtype = line.substr(0, pos);

        // Extract resource
        pos = line.find_first_not_of(" \t", pos);
        if(pos == std::string::npos)
            throw BadRequest("No resource given");
        std::string::size_type pos2 = line.find(' ', pos);
        if(pos2 == std::string::npos)
            throw BadRequest("No protocol version given");
        std::string resource = line.substr(pos, pos2 - pos);

        // Extract protocol and version string
        std::string protocolversion = line.substr(pos2 + 1);

        // Make request object
        if(rqtype == "GET")
            m_currentrequest = std::make_shared<GetRequest>(m_server);
        else if(rqtype == "POST")
            m_currentrequest = std::make_shared<PostRequest>(m_server);
        /*else if(rqtype == "PUT")
            m_currentrequest = boost::shared_ptr<Request>(new PutRequest);
        else if(rqtype == "DELETE")
            m_currentrequest = boost::shared_ptr<Request>(new DeleteRequest);*/
        else
            throw BadRequest("Unknown request method");

        m_currentrequest->set_resource(resource);
        m_currentrequest->set_protocol(protocolversion);

        readheaderfield();
        }
        catch(BadRequest& e) {
            // TODO Proper logging
            std::cerr << "In Connection::handle_readrequestline, caught BadRequest: " << e.what() << std::endl;
            // TODO Send BadRequest response
            }
        catch(Endpoint::MethodNotSupported& e) {
            // TODO Proper logging
            std::cerr << "In Connection::handle_readrequestline, caught Endpoint::MethodNotSupported: " << e.what() << std::endl;
            // TODO Send MethodNotAllowed response
            }
        catch(std::runtime_error& e) {
            // TODO Proper logging
            std::cerr << "In Connection::handle_readrequestline, caught runtime_error: " << e.what() << std::endl;
            // TODO Send InternalServerError response
            }
        }
    else {
        // TODO Proper logging
        std::cerr << "Error on reading request line: " << err << std::endl;
        }
    }

void Connection::readheaderfield()
    {
    async_read_until(m_socket,
                     m_readbuf,
                     '\n',
                     boost::bind(&Connection::handle_readheaderfield,
                                 shared_from_this(),
                                 boost::asio::placeholders::error));
    }

void Connection::handle_readheaderfield(const boost::system::error_code& err)
    {
    if(!err) {
        try {
            std::istream input(&m_readbuf);
            std::string line;
            std::getline(input, line);

            if(!line.empty() && line.at(line.length() - 1) == '\r') // If last char is carriage return
                line.erase(line.length() - 1); // Remove it

            if(line.empty()) {
                if(!m_curkv.first.empty()) {
                    m_currentrequest->add_http_header(HttpHeader(m_curkv.first, m_curkv.second));
                    m_curkv.first.clear();
                    m_curkv.second.clear();
                    }

                if(m_currentrequest->expected_payload_bytes() != 0) {
                    readpayload();
                    }
                else {
                    boost::function<void (boost::system::error_code)> callback = boost::bind(
                        &Connection::handle_get_response_block, shared_from_this(), _1);
                    m_currentrequest->get_output_block_async(m_writebuffers, callback);
                    }
                }
            else {
                if(line.at(0) == ' ' || line.at(0) == '\t') {
                    if(m_curkv.first.empty())
                        throw BadRequest("Line continuation without a preceding \"Key: Value\" line");

                    std::string::size_type pos = line.find_first_not_of(" \t");
                    if(pos == std::string::npos)
                        throw BadRequest("Empty continuation line");
                    m_curkv.second += line.substr(pos);
                    }
                else {
                    if(!m_curkv.first.empty()) {
                        m_currentrequest->add_http_header(HttpHeader(m_curkv.first, m_curkv.second));
                        m_curkv.first.clear();
                        m_curkv.second.clear();
                        }

                    m_curkv = parsekv(line);
                    readheaderfield();
                    }
                }
            }
        catch(BadRequest& e) {
            // TODO
            }
        catch(Server::HandlerNotFound& e) {
            // TODO
            }
        catch(std::runtime_error& e) {
            // TODO
            }
        }
    }

void Connection::readpayload()
    {
    // Switching buffers at this point - make sure old readbuffer is empty
    uint32_t bytes = 0;
    auto data = m_readbuf.data();
    for(const auto& buffer : data) {
        const uint8_t* block = buffer_cast<const uint8_t *>(const_buffer(buffer));
        std::size_t payload_size = std::min(buffer_size(const_buffer(buffer)), m_currentrequest->expected_payload_bytes());
        if(payload_size > 0) {
            auto payload_buffer = std::make_shared<std::vector<uint8_t>>(block, block + payload_size);
            m_currentrequest->add_payloadblock(payload_buffer);
            }
        bytes += payload_size;
        }
    m_readbuf.consume(bytes);

    // At this point, m_readbuf is either empty or m_currentrequest->expected_payload_bytes() is 0
    // (or both). If anything is left in m_readbuf at this point, it is part of the next request

    if(m_currentrequest->expected_payload_bytes() > 0) {
        auto payload_buffer = std::make_shared<std::vector<uint8_t>>(
            std::min(m_currentrequest->expected_payload_bytes() + 1, standard_buffer_size));
        m_socket.async_read_some(
            buffer(*payload_buffer),
            boost::bind(&Connection::handle_readpayloadblock,
                        shared_from_this(),
                        payload_buffer,
                        placeholders::error,
                        placeholders::bytes_transferred));
        }
    else {
        boost::function<void (boost::system::error_code)> callback = boost::bind(
            &Connection::handle_get_response_block, shared_from_this(), _1);
        m_currentrequest->get_output_block_async(m_writebuffers, callback);
        }
    }

void Connection::handle_readpayloadblock(std::shared_ptr<std::vector<uint8_t>> block, const boost::system::error_code& err, std::size_t bytes_transferred)
    {
    if(!err) {
        try {
            if(block->size() > bytes_transferred)
                block->erase(block->begin() + bytes_transferred);

            m_currentrequest->add_payloadblock(block);

            if(m_currentrequest->expected_payload_bytes() > 0) {
                auto payload_buffer = std::make_shared<std::vector<uint8_t>>(
                    std::min(m_currentrequest->expected_payload_bytes(), standard_buffer_size));
                m_socket.async_read_some(
                    buffer(*payload_buffer),
                    boost::bind(&Connection::handle_readpayloadblock,
                                shared_from_this(),
                                payload_buffer,
                                placeholders::error,
                                placeholders::bytes_transferred));
                }
            else {
                boost::function<void (boost::system::error_code)> callback = boost::bind(
                    &Connection::handle_get_response_block, shared_from_this(), _1);
                m_currentrequest->get_output_block_async(m_writebuffers, callback);
                }
            }
        catch(std::runtime_error& e) {
std::cerr << "While reading payload block, caught runtime error: " << e.what() << std::endl;
            /*TODO*/
            }
        }
    else {
std::cerr << "Error on reading payload: " << err.message() << std::endl;
        }
    }

void Connection::handle_get_response_block(const boost::system::error_code& err)
    {
    if(!err) {
        async_write(m_socket, m_writebuffers.at(0),
                boost::bind(&Connection::handle_write_block_prefix,
                            shared_from_this(),
                            placeholders::error,
                            placeholders::bytes_transferred));
        }
    }

void Connection::handle_write_block_prefix(const boost::system::error_code& err, std::size_t bytes_transferred)
    {
    if(!err) {
        m_writebuffers.at(0).consume(bytes_transferred);
        async_write(m_socket, m_writebuffers.at(1),
                boost::bind(&Connection::handle_write_block,
                            shared_from_this(),
                            placeholders::error,
                            placeholders::bytes_transferred));
        }
    }

void Connection::handle_write_block(const boost::system::error_code& err, std::size_t bytes_transferred)
    {
    if(!err) {
        m_writebuffers.at(1).consume(bytes_transferred);
        if(m_currentrequest->output_left()) {
            boost::function<void (boost::system::error_code)> callback = boost::bind(
                &Connection::handle_get_response_block, shared_from_this(), _1);
            m_currentrequest->get_output_block_async(m_writebuffers, callback);
            }
        else {
            bool keepalive = m_currentrequest->keepalive_requested();
            // Current request is finished at this point
            m_currentrequest.reset();
            m_curkv.first.clear();
            m_curkv.second.clear();

            if(keepalive)
                start();
            }
        }
    }

std::pair<std::string, std::string> Connection::parsekv(const std::string& in)
    {
    std::pair<std::string, std::string> out;

    std::string::size_type pos = in.find(':');
    if(pos == std::string::npos)
        throw BadRequest("Invalid Key-Value line: No colon: \"" + in + "\"");
    out.first = in.substr(0, pos);
    // Field names are case-insensitive
    std::transform(out.first.begin(), out.first.end(), out.first.begin(), ::tolower);

    if(in.length() == pos)
        return out;
    // Find position of first char of value
    pos = in.find_first_not_of(" \t", pos + 1);
    if(pos == std::string::npos)
        return out; // Is that even valid HTTP?
    out.second = in.substr(pos);

    return out;
    }

}
