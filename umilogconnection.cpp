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
#include "umilogconnection.hpp"

umi::Connection::Connection(ConnectionType type, const std::string& host, int port, const std::string& caFile)
  : m_connectionType(type),
    m_host(host),
    m_ca(caFile)
{
  if(port <= 0) {
    if(m_connectionType == ConnectionType::TLS){
      m_port = 6514;
    }else {
      m_port = 514;
    }
  }else {
    m_port = static_cast<uint32_t>(port);
  }
}

umi::Connection::Connection(const Connection& val)
  : m_connectionType(val.m_connectionType),
    m_host(val.m_host),
    m_port(val.m_port),
    m_ca(val.m_ca)
{

}

umi::Connection::Connection(Connection&& val)
  : m_connectionType(std::move(val.m_connectionType)),
    m_host(std::move(val.m_host)),
    m_port(std::move(val.m_port)),
    m_ca(std::move(val.m_ca))
{

}

umi::Connection::~Connection()
{
}


umi::Connection& umi::Connection::operator=(const Connection& val)
{
  if(this != &val){
    m_connectionType = val.m_connectionType;
    m_host = val.m_host;
    m_port = val.m_port;
    m_ca = val.m_ca;
  }
  return *this;
}

umi::Connection& umi::Connection::operator=(Connection&& val)
{
  m_connectionType = std::move(val.m_connectionType);
  m_host = std::move(val.m_host);
  m_port = std::move(val.m_port);
  m_ca = std::move(val.m_ca);
  return *this;
}

void umi::Connection::setTLSCAFile(const std::string& file)
{
  m_ca = file;
}

const std::string& umi::Connection::getTLSCAFile() const
{
  return m_ca;
}

std::string& umi::Connection::mutableTLSCAFile()
{
  return m_ca;
}

umi::Connection::ConnectionType umi::Connection::getConnectionType() const
{
  return m_connectionType;
}

void umi::Connection::setConnectionType(umi::Connection::ConnectionType val)
{
  m_connectionType = val;
}

umi::Connection::ConnectionType& umi::Connection::mutableConnectionType()
{
  return m_connectionType;
}

uint32_t umi::Connection::getPort() const
{
  return m_port;
}

void umi::Connection::setPort(uint32_t port)
{
  m_port = port;
}

uint32_t& umi::Connection::mutablePort()
{
  return m_port;
}

const std::string& umi::Connection::getHost() const
{
  return m_host;
}

void umi::Connection::setHost(const std::string& val)
{
  m_host = val;
}

std::string& umi::Connection::mutableHost()
{
  return m_host;
}

