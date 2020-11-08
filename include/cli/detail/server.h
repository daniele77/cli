/*******************************************************************************
 * CLI - A simple command line interface.
 * Copyright (C) 2016 Daniele Pallastrelli
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************/

#ifndef CLI_DETAIL_SERVER_H_
#define CLI_DETAIL_SERVER_H_

#include <memory>
#include <queue>
#include "asio.hpp"

namespace cli
{
namespace detail
{

class Session : public std::enable_shared_from_this<Session>, public std::streambuf
{
public:
    virtual ~Session() = default;
    virtual void Start()
    {
        OnConnect();
        Read();
    }

protected:

    Session(asio::ip::tcp::socket _socket) : socket(std::move(_socket)), outStream( this ) {}

    virtual void Disconnect()
    {
        socket.shutdown( asio::ip::tcp::socket::shutdown_both );
        socket.close();
    }

    virtual void Read()
    {
      auto self( shared_from_this() );
      socket.async_read_some( asio::buffer( data, max_length ),
          [ this, self ]( std::error_code ec, std::size_t length )
          {
              if ( !socket.is_open() || ( ec == asio::error::eof ) || ( ec == asio::error::connection_reset ) )
                  OnDisconnect();
              else if ( ec )
                  OnError();
              else
              {
                  OnDataReceived( std::string( data, length ));
                  Read();
              }
          });
    }

    virtual void Send(const std::string& msg)
    {
        std::error_code ec;
        asio::write(socket, asio::buffer(msg), ec);
        if ((ec == asio::error::eof) || (ec == asio::error::connection_reset))
            OnDisconnect();
        else if (ec)
            OnError();
    }

    virtual std::ostream& OutStream() { return outStream; }

    virtual void OnConnect() = 0;
    virtual void OnDisconnect() = 0;
    virtual void OnError() = 0;
    virtual void OnDataReceived(const std::string& _data) = 0;

    virtual std::string Encode(const std::string& _data) const { return _data; }

private:

    // std::streambuf
    std::streamsize xsputn( const char* s, std::streamsize n ) override
    {
        Send(Encode(std::string(s, s+n)));
        return n;
    }
    int overflow( int c ) override
    {
        Send(Encode(std::string(1, static_cast< char >(c))));
        return c;
    }

    asio::ip::tcp::socket socket;
    enum { max_length = 1024 };
    char data[ max_length ];
    std::ostream outStream;
};


class Server
{
public:
    // disable value semantics
    Server( const Server& ) = delete;
    Server& operator = ( const Server& ) = delete;

    Server(notboost::BoostExecutor::ContextType& ios, unsigned short port) :
        acceptor( ios, asio::ip::tcp::endpoint( asio::ip::tcp::v4(), port )),
        socket( ios )
    {
        Accept();
    }
    Server(notboost::BoostExecutor::ContextType& ios, std::string address, unsigned short port) :
        acceptor( ios, asio::ip::tcp::endpoint(asio::ip::address::from_string(address), port)),
        socket( ios )
    {
        Accept();
    }
    virtual ~Server() = default;
    // returns shared_ptr instead of unique_ptr because Session needs to use enable_shared_from_this
    virtual std::shared_ptr< Session > CreateSession( asio::ip::tcp::socket socket ) = 0;
private:
    void Accept()
    {
        acceptor.async_accept( socket, [this](std::error_code ec)
            {
                if ( !ec ) CreateSession( std::move( socket ) ) -> Start();
                Accept();
            });
    }
    asio::ip::tcp::acceptor acceptor;
    asio::ip::tcp::socket socket;
};

} // namespace detail
} // namespace cli

#endif // CLI_DETAIL_SERVER_H_

