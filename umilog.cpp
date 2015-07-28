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
#include "umilog.hpp"
#include "umilogtimestamp.hpp"
#include "umisocket.hpp"
#include <array>
#include <cstdarg>
#include <sstream>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>


int umi::Logger::getPriority(umi::Facility facility, umi::Severity severity)
{
  return static_cast<int>(facility) * 8 + static_cast<int>(severity);
}

umi::Logger::Logger(const LoggerLocalData& loggerData,
  const std::vector<umi::Connection>& loggerConnection)
  : m_loggerLocalData(loggerData),
    m_loggerConnection(loggerConnection),
    m_run(true),
    m_ioservice(),
    m_woker(m_ioservice),
    m_loggerThread(boost::bind(&boost::asio::io_service::run,&m_ioservice))
{
  // Create connections depending on the connection data this constructor
  // implies only one connection
  for(auto& i: m_loggerConnection){
    m_connections.push_back(std::unique_ptr<umi::Socket>(umi::SocketFactory::CreateSocket(*this,i)));
  }
}

umi::Logger::~Logger()
{
  m_run = false;
  std::unique_lock<std::mutex> _lock(m_queueMutex);
  m_ioservice.stop();// stop the io service
  m_connections.clear(); // stop the connections
  m_loggerThread.join(); // join the thread to release the memory
}

void umi::Logger::processMessages()
{
  std::queue<std::shared_ptr<std::string>> _localQueue;
  {
    std::unique_lock<std::mutex> _lock(m_queueMutex);
    std::swap(m_messageQueue,_localQueue);
  }
  while(!_localQueue.empty() && m_run){
    std::shared_ptr<std::string> _elementToSend = _localQueue.front();
    _localQueue.pop();
    // Process element
    for(auto& singleSocket : m_connections ){
      singleSocket->send(_elementToSend);
    }
  }
}

void umi::Logger::Log(umi::Facility facility,
  umi::Severity severity,
  const std::string& app,
  const std::string& msgid,
  const char* message, ...)
{
  if(getPriority(facility,severity) <=
     getPriority(m_loggerLocalData.getMaxFacility(),
                 m_loggerLocalData.getMaxSeverity())){
    std::stringstream _messageToSend;
    // The maximum buffer we can send is 64k
    std::array<char,1024*64> _maxBuffer;

    va_list _variableList;
    va_start(_variableList,message);
    int _result = vsnprintf(_maxBuffer.data(),_maxBuffer.size(),message,_variableList);
    if(_result >= 0) {
      _messageToSend << "<" << getPriority(facility,severity) << ">"
        << m_loggerLocalData.getVersion()  << " "
        << umi::Timestamp::getTimestamp(m_loggerLocalData.getPrecision()) << " "
        << m_loggerLocalData.getHostname() << " "
        << app << " "
        << getpid() << " "
        << msgid << " - "
        << _maxBuffer.data();
      if(m_loggerLocalData.getPrint()) {
        std::cout << _messageToSend.str() << '\n';
      }
      {
        std::unique_lock<std::mutex> _lock(m_queueMutex);
        // The elements are store as shared pointer to avoid problems with the async logging
        m_messageQueue.push(std::make_shared<std::string>(_messageToSend.str()));
      }
      m_ioservice.post(boost::bind(&umi::Logger::processMessages,this));
    }
    va_end(_variableList);
  }
}

const std::vector<umi::Connection>& umi::Logger::getConnectionData() const
{
  return m_loggerConnection;
}
