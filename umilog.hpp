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
#ifndef UMILOG_HPP
#define UMILOG_HPP

#include "umilocaldata.hpp"
#include "umilogconnection.hpp"
#include "umiseverity.hpp"
#include "umifacility.hpp"
#include <memory>
#include <condition_variable>
#include <cstdint>
#include <string>
#include <boost/asio.hpp>
#include <thread>
#include <queue>
#include <atomic>

namespace umi{
  class Socket;

  /**
    \brief Class to represent the actual log of data

    A instance of Logger will open a connection against the specified
    place to submit data with the specified parameters.

    We use two configuration parameters, the one related to the actual
    host, (hostname, version), and the ones related to connection against
    the relay/collector


    This class doesn't support structure data

    See RFC 5424

    https://tools.ietf.org/html/rfc5424
    https://tools.ietf.org/html/rfc5425
    https://tools.ietf.org/html/rfc5426
    https://tools.ietf.org/html/rfc5427
  */
  class Logger {
    friend class Socket;
  public:
    /**
      \brief Translate the actual Facility and Severity to a valid number

      The actual formula is facility * 8 + severity
    */
    static int getPriority(umi::Facility facility, umi::Severity severity);
  public:
    /**
      \brief Creates a logger instance
    */
    Logger(const LoggerLocalData& loggerData, const std::vector<umi::Connection>& loggerConnection);
    /**
      \brief Release the resources used by the logger
    */
    virtual ~Logger();
    /**
      \brief Log a message into the system.
    */
    void Log(Facility facility,
     Severity severity,
     const std::string& app,
     const std::string& msgid,
     const char* message, ...);
    /**
      \brief Gets the logger local data
    */
    const umi::LoggerLocalData& getLocalData() const;
    /**
      \brief Gets the connection data
    */
    const std::vector<umi::Connection>& getConnectionData() const;
  protected:
    /**
      \brief Internal fucntion to process the queue
    */
    void processMessages();
  protected:
    umi::LoggerLocalData m_loggerLocalData; //!< Local logger data
    std::vector<umi::Connection> m_loggerConnection; //!< Local connection information
    std::atomic_bool m_run; //!< Atomic to control the status
    boost::asio::io_service m_ioservice; //!< The io service
    boost::asio::io_service::work m_woker; //!< the asio internal worker to give things to asio
    std::vector<std::unique_ptr<umi::Socket>> m_connections; //!< Connections we will use to send data to the loggers
    std::thread m_loggerThread; //!< Internal thread
    std::mutex m_queueMutex; //!< Mutex used in the queue to dispatch messages
    std::queue<std::shared_ptr<std::string>> m_messageQueue; //!< Internal messagequeue
  };
}

#endif
