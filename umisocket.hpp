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
#ifndef UMISOCKET_HPP
#define UMISOCKET_HPP

#include "umilog.hpp"

namespace umi{
  /**
    \brief Creates a socket depending on the
    specify configuration

    Forward declaration
  */
  class SocketFactory;

  /**
    \brief Class to represent a socket connection
    against the rsyslog server.

    As we know udp connections differs from tcp and
    we can't use ssl with them
  */
  class Socket {
    friend class SocketFactory;
  public:
    /**
      \brief Clean the connection
    */
    virtual ~Socket();
    /**
      \brief Sends the data
    */
    virtual void send(std::shared_ptr<std::string> message) = 0;
  protected:
    /**
      \brief Gets the internal boost asio
    */
    boost::asio::io_service& getInternalService();
    /**
      \brief Constructor with the default connection
      information data
    */
    Socket(umi::Logger& logger,
           const umi::Connection& loggerInfo);
    umi::Logger& m_logger; //!< Logger to get the dispatcher
    const umi::Connection& m_loggerInfo; //!< Connection info
  };

  /**
    \brief Factory to create sockets depending on the logger
    configuration
  */
  class SocketFactory {
  public:
    /**
      \brief Creates a socket with the desired configuration
    */
    static Socket* CreateSocket(umi::Logger& logger,
                                const umi::Connection& loggerInfo);

  };
}

#endif // UMISOCKET_HPP

