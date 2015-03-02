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
#ifndef UMILOGCONNECTION_HPP
#define UMILOGCONNECTION_HPP

#include <cstdint>
#include <string>

namespace umi{
  /**
    \brief Class to configure the connection against one system

    The connection can be UDP/IP or TCP(SSL only)/IP, we can specify ports or keep it empty
    and the class will choose the default ones

    Default ports

    TLS->6514
    UDP->514
    TCP->514
  */
  class Connection{
  public:
    enum class ConnectionType : int{
      UDP,
      TCP,
      TLS
    };
  public:
    /**
      \brief Default connection configuration
    */
    Connection(ConnectionType type, const std::string& host, int port, const std::string& caFile);
    /**
      \brief Copy constructor
    */
    Connection(const Connection& val);
    /**
      \brief rvalue constructor
    */
    Connection(Connection&& val);
    /**
      \brief Clean the resources used by this connection data
    */
    virtual ~Connection();
    /**
      \brief Assign operator
    */
    Connection& operator=(const Connection& val);
    /**
      \brief Move operator
    */
    Connection& operator=(Connection&& val);
    /**
      \brief Sets the ca file in case of SSL connection
    */
    void setTLSCAFile(const std::string& file);
    /**
      \brief Gets the ca file in case of SSL connection
    */
    const std::string& getTLSCAFile() const;
    /**
      \brief Gets the mutable version of the TLS CA file for SSL connections
    */
    std::string& mutableTLSCAFile();
    /**
      \brief Gets the connection type
    */
    umi::Connection::ConnectionType getConnectionType() const;
    /**
      \brief Sets the connection type
    */
    void setConnectionType(umi::Connection::ConnectionType val);
    /**
      \brief mutable connection type
    */
    umi::Connection::ConnectionType& mutableConnectionType();
    /**
      \brief Gets the port
    */
    uint32_t getPort() const;
    /**
      \brief Sets the port
    */
    void setPort(uint32_t port);
    /**
      \brief Mutable version of the port
    */
    uint32_t& mutablePort();
    /**
      \brief Gets the host we will send the data
    */
    const std::string& getHost() const;
    /**
      \brief Sets the host we will send the data
    */
    void setHost(const std::string& val);
    /**
      \brief Mutable version of the host
    */
    std::string& mutableHost();
  protected:
    ConnectionType m_connectionType;  //!< Connection we are using(the type)
    std::string m_host;  //!< host we will send the data
    uint32_t m_port;  //!< Port we are using in the communication
    std::string m_ca; //!< Certificate authority
  };
}

#endif // UMILOGCONNECTION_HPP

