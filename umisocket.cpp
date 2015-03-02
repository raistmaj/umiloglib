/*Copyright (c) 2015, José Gerardo Palma Durán, raistmaj@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by the University of
   California, Berkeley and its contributors.
4. Neither the name of the University nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*/
#include "umisocket.hpp"
#include <memory>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>

namespace umi {
  class SocketUDP : public Socket {
  public:
    SocketUDP(umi::Logger& logger,
              const umi::Connection& loggerInfo)
      : Socket(logger,loggerInfo),
        m_socket(new boost::asio::ip::udp::socket(getInternalService()))
    {
      m_socket->open(boost::asio::ip::udp::v4());
      boost::asio::ip::udp::resolver _resolver(getInternalService());
      boost::asio::ip::udp::resolver::query _query(boost::asio::ip::udp::v4(),
        m_loggerInfo.getHost().c_str(),
        boost::lexical_cast<std::string>(m_loggerInfo.getPort()));
      m_endpoint = std::unique_ptr<boost::asio::ip::udp::endpoint>(
            new boost::asio::ip::udp::endpoint(*_resolver.resolve(_query)));
      m_isOpen = true;
    }
    virtual ~SocketUDP()
    {
      if(m_socket){
        m_socket->close(); // Not async close
      }
    }
    void send(std::shared_ptr<std::string> message)
    {
      if(m_isOpen && m_socket) {
        m_socket->async_send_to(boost::asio::buffer(*message),
          *m_endpoint,
          boost::bind(&umi::SocketUDP::handlerSend,this,
          message, //!< Thanks to this and the shared_ptr the message will not die
          0,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
      }
    }
    void handlerSend(std::shared_ptr<std::string> message,
      std::size_t actualPosition,
      const boost::system::error_code& error,
      std::size_t dataSent)
    {
      if(!error){
        if(m_isOpen && m_socket && (actualPosition + dataSent) <  message->size()) {
          m_socket->async_send_to(boost::asio::buffer((*message).substr(actualPosition + dataSent)),
            *m_endpoint,
            boost::bind(&umi::SocketUDP::handlerSend,this,
            message, //!< Thanks to this and the shared_ptr the message will not die
            (actualPosition + dataSent),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
        }
      }
    }

  protected:
    bool m_isOpen = false;
    std::unique_ptr<boost::asio::ip::udp::socket> m_socket;
    std::unique_ptr<boost::asio::ip::udp::endpoint> m_endpoint;
  };

  class SocketTCP : public Socket {
  public:
    SocketTCP(umi::Logger& logger,
              const umi::Connection& loggerInfo)
      : Socket(logger,loggerInfo),
        m_socket(new boost::asio::ip::tcp::socket(getInternalService()))
    {
      if(m_socket){
        boost::asio::socket_base::keep_alive _keepAlive(true);
        m_socket->set_option(_keepAlive);
        boost::asio::ip::tcp::resolver _resolver(getInternalService());
        boost::asio::ip::tcp::resolver::query _query(m_loggerInfo.getHost().c_str(),
          boost::lexical_cast<std::string>(m_loggerInfo.getPort()));
        boost::asio::ip::tcp::resolver::iterator _endpoint(_resolver.resolve(_query));
        boost::asio::ip::tcp::endpoint endPoint = *_endpoint;
        m_socket->async_connect(endPoint,
          boost::bind(&umi::SocketTCP::handleOnConnect,this,
            boost::asio::placeholders::error,
            ++_endpoint));
      }
    }
    virtual ~SocketTCP()
    {
      if(m_socket) {
        m_socket->close(); // this is not async
      }
    }
    void send(std::shared_ptr<std::string> message)
    {
      if(m_isOpen && m_socket) {
        m_socket->async_send(boost::asio::buffer(*message),
          boost::bind(&umi::SocketTCP::handlerSend,this,
          message, //!< Thanks to this and the shared_ptr the message will not die
          0,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
      }
    }
    void handleOnConnect(const boost::system::error_code& errorCode,
                         boost::asio::ip::tcp::resolver::iterator endpointIT)
    {
      if(m_socket){
        if(errorCode == 0){
          m_isOpen = true;
        }else if(endpointIT != boost::asio::ip::tcp::resolver::iterator()){
          // try next
          m_socket->close();
          boost::asio::ip::tcp::endpoint endPoint = *endpointIT;
          m_socket->async_connect(endPoint,
            boost::bind(&umi::SocketTCP::handleOnConnect,this,
              boost::asio::placeholders::error,
              ++endpointIT));
        }
      }
    }

    void handlerSend(std::shared_ptr<std::string> message,
      std::size_t lastPosition,
      const boost::system::error_code& errorCode,
      std::size_t dataSent)
    {
      if(!errorCode){
        if(m_socket && m_isOpen && (lastPosition + dataSent) < message->size()){
          m_socket->async_send(boost::asio::buffer((*message).substr(lastPosition + dataSent)),
            boost::bind(&umi::SocketTCP::handlerSend,this,
            message,
            lastPosition + dataSent,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
        }
      }
    }
  protected:
    bool m_isOpen = false;
    std::unique_ptr<boost::asio::ip::tcp::socket> m_socket;
  };

  class SocketTLS : public Socket {
  public:
    SocketTLS(umi::Logger& logger,
              const umi::Connection& loggerInfo)
      : Socket(logger,loggerInfo),
        m_sslContext(new boost::asio::ssl::context(getInternalService(),boost::asio::ssl::context::sslv23)),
        m_socket()
    {
      if(m_sslContext){
        m_sslContext->set_verify_mode(boost::asio::ssl::context::verify_peer);
        m_sslContext->set_options(boost::asio::ssl::context::default_workarounds |
                                  boost::asio::ssl::context::no_sslv2 |
                                  boost::asio::ssl::context::no_sslv3 |
                                  boost::asio::ssl::context::single_dh_use);
        if(!loggerInfo.getTLSCAFile().empty()){
          m_sslContext->load_verify_file(loggerInfo.getTLSCAFile());
        }
        m_socket.reset(new boost::asio::ssl::stream<boost::asio::ip::tcp::socket>(getInternalService(),*m_sslContext));
        if(m_socket){
          boost::asio::socket_base::keep_alive _keepAlive(true);
          m_socket->lowest_layer().set_option(_keepAlive);
          boost::asio::ip::tcp::resolver _resolver(getInternalService());
          boost::asio::ip::tcp::resolver::query _query(m_loggerInfo.getHost().c_str(),
            boost::lexical_cast<std::string>(m_loggerInfo.getPort()));
          boost::asio::ip::tcp::resolver::iterator _endpoint(_resolver.resolve(_query));
          boost::asio::ip::tcp::endpoint endPoint = *_endpoint;
          m_socket->lowest_layer().async_connect(endPoint,
            boost::bind(&umi::SocketTLS::handleOnConnect,this,
              boost::asio::placeholders::error,
              ++_endpoint));
        }
      }
    }
    virtual ~SocketTLS()
    {
      if(m_socket) {
        m_socket->lowest_layer().close(); // this is not async
      }
    }
    void send(std::shared_ptr<std::string> message)
    {
      if(m_isOpen && m_socket) {
        boost::asio::async_write(*m_socket,
          boost::asio::buffer(*message),
          boost::bind(&umi::SocketTLS::handlerSend,this,
            message, //!< Thanks to this and the shared_ptr the message will not die
            0,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
      }
    }
    void handleOnConnect(const boost::system::error_code& errorCode,
                         boost::asio::ip::tcp::resolver::iterator endpointIT)
    {
      if(m_socket){
        if(errorCode == 0){
          m_socket->async_handshake(boost::asio::ssl::stream_base::client,
                                    boost::bind(&umi::SocketTLS::handleOnHandshake,this,
                                                boost::asio::placeholders::error));
        }else if(endpointIT != boost::asio::ip::tcp::resolver::iterator()){
          // try next
          m_socket->lowest_layer().close();
          boost::asio::ip::tcp::endpoint endPoint = *endpointIT;
          m_socket->lowest_layer().async_connect(endPoint,
            boost::bind(&umi::SocketTLS::handleOnConnect,this,
              boost::asio::placeholders::error,
              ++endpointIT));
        }
      }
    }
    void handleOnHandshake(const boost::system::error_code& error)
    {
      if(!error){
        m_isOpen = true;
      }
    }
    void handlerSend(std::shared_ptr<std::string> message,
      std::size_t lastPosition,
      const boost::system::error_code& errorCode,
      std::size_t dataSent)
    {
      if(!errorCode){
        if(m_socket && m_isOpen && (lastPosition + dataSent) < message->size()){
          boost::asio::async_write(*m_socket,
            boost::asio::buffer((*message).substr(lastPosition+dataSent)),
            boost::bind(&umi::SocketTLS::handlerSend,this,
              message, //!< Thanks to this and the shared_ptr the message will not die
              lastPosition + dataSent,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
        }
      }
    }
  protected:
    bool m_isOpen = false;
    std::unique_ptr<boost::asio::ssl::context> m_sslContext;
    std::unique_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> m_socket;
  };

}

umi::Socket::Socket(umi::Logger& logger,
  const umi::Connection& loggerInfo)
  : m_logger(logger),
    m_loggerInfo(loggerInfo)
{
}

umi::Socket::~Socket()
{
}

boost::asio::io_service& umi::Socket::getInternalService()
{
  return m_logger.m_ioservice;
}

umi::Socket* umi::SocketFactory::CreateSocket(umi::Logger& logger,
                                              const umi::Connection& loggerInfo)
{
  if(loggerInfo.getConnectionType() == umi::Connection::ConnectionType::UDP){
    return new umi::SocketUDP(logger,loggerInfo);
  }else if(loggerInfo.getConnectionType() == umi::Connection::ConnectionType::TCP) {
    return new umi::SocketTCP(logger,loggerInfo);
  }else if(loggerInfo.getConnectionType() == umi::Connection::ConnectionType::TLS) {
    return new umi::SocketTLS(logger,loggerInfo);
  }
  return 0;
}
