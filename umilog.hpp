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
#include "umilogtimestamp.hpp"
#include <memory>
#include <iostream>
#include <boost/asio.hpp>
#include <condition_variable>
#include <cstdint>
#include <string>
#include <thread>
#include <queue>
#include <atomic>

namespace umi {
    namespace log {
        class socket;

        class socket_factory;

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
        class logger {
            friend class socket;

        public:
            /**
              \brief Translate the actual Facility and Severity to a valid number

              The actual formula is facility * 8 + severity
            */
            inline static int get_priority(umi::log::facility facility, umi::log::severity severity) {
                return static_cast<int>(facility) * 8 + static_cast<int>(severity);
            }

        public:
            /**
              \brief Creates a logger instance
            */
            logger(const logger_local_data &loggerData, const std::vector<umi::log::connection> &loggerConnection);

            /**
              \brief Release the resources used by the logger
            */
            virtual ~logger() {
                m_run = false;
                std::unique_lock<std::mutex> _lock(m_queueMutex);
                m_ioservice.stop();// stop the io service
                m_connections.clear(); // stop the connections
                m_loggerThread.join(); // join the thread to release the memory
            }

            /**
              \brief Gets the connection data
            */
            const std::vector<umi::log::connection> &get_connection_data() const {
                return m_loggerConnection;
            }

            /**
             \brief Log a message into the system.
            */
            template<typename... Args>
            void log(umi::log::facility facility,
                     umi::log::severity severity,
                     const std::string &app,
                     const std::string &msgid,
                     const char *message, Args &&... args) {
                if (get_priority(facility, severity) <=
                    get_priority(m_loggerLocalData.get_max_facility(),
                                 m_loggerLocalData.get_max_severity())) {
                    std::stringstream _messageToSend;
                    // The maximum buffer we can send is 64k
                    std::array<char, 1024 * 64> _maxBuffer;

                    int _result = snprintf(_maxBuffer.data(), _maxBuffer.size(), message, std::forward<Args>(args)...);
                    if (_result >= 0) {
                        _messageToSend << "<" << get_priority(facility, severity) << ">"
                        << m_loggerLocalData.get_version() << " "
                        << umi::log::Timestamp::get_timestamp(m_loggerLocalData.get_precision()) << " "
                        << m_loggerLocalData.get_hostname() << " "
                        << app << " "
                        << getpid() << " "
                        << msgid << " - "
                        << _maxBuffer.data();
                        if (m_loggerLocalData.get_print()) {
                            std::cout << _messageToSend.str() << '\n';
                        }
                        {
                            std::unique_lock<std::mutex> _lock(m_queueMutex);
                            // The elements are store as shared pointer to avoid problems with the async logging
                            m_messageQueue.push(std::make_shared<std::string>(_messageToSend.str()));
                        }
                        m_ioservice.post([this]() { this->process_messages(); });
                    }
                }
            }

        protected:
            /**
              \brief Internal function to process the queue
            */
            void process_messages();

        protected:
            /**
             * Local logger data
             * */
            umi::log::logger_local_data m_loggerLocalData;
            /**
             * Local connection information
             * */
            std::vector<umi::log::connection> m_loggerConnection;
            /**
             * Atomic to control de status
             * */
            std::atomic_bool m_run;
            /**
             * The boost io service
             * */
            boost::asio::io_service m_ioservice;
            /**
             * The worker to keep the io service pending of new messages
             * */
            boost::asio::io_service::work m_worker;
            /**
             * Connections we will use to send data
             * */
            std::vector<std::unique_ptr<umi::log::socket>> m_connections;
            /**
             * Thread used to run the io service
             * */
            std::thread m_loggerThread;
            /**
             * Mutex used to protect the message queue
             * */
            std::mutex m_queueMutex;
            /**
             * Internal message queue
             * */
            std::queue<std::shared_ptr<std::string>> m_messageQueue;
        };
    }
}

#include "umisocket_priv.hpp"

/**
  \brief Creates a logger instance
*/
umi::log::logger::logger(const umi::log::logger_local_data &loggerData,
                         const std::vector<umi::log::connection> &loggerConnection)
        : m_loggerLocalData(loggerData),
          m_loggerConnection(loggerConnection),
          m_run(true),
          m_ioservice(),
          m_worker(m_ioservice),
          m_loggerThread([&]() { m_ioservice.run(); }) {
    // Create connections depending on the connection data this constructor
    // implies only one connection
    for (auto &i: m_loggerConnection) {
        m_connections.push_back(std::unique_ptr<umi::log::socket>(umi::log::socket_factory::create_socket(*this, i)));
    }
}


void umi::log::logger::process_messages() {
    std::queue<std::shared_ptr<std::string>> _localQueue;
    {
        std::unique_lock<std::mutex> _lock(m_queueMutex);
        std::swap(m_messageQueue, _localQueue);
    }
    while (!_localQueue.empty() && m_run) {
        std::shared_ptr<std::string> _elementToSend = _localQueue.front();
        _localQueue.pop();
        // Process element
        for (auto &singleSocket : m_connections) {
            singleSocket->send(_elementToSend);
        }
    }
}

#endif
