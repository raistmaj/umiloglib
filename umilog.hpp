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

#include <memory>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <condition_variable>
#include <cstdint>
#include <string>
#include <thread>
#include <queue>
#include <atomic>


// clock_gettime is missing on windows
#ifdef _WIN32
#include <windows.h>
#include <mutex>
#endif

#ifdef _WIN32
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 2
#endif

namespace umi {
    namespace log {
        static std::once_flag flag_frequency;
        static std::once_flag flag_uepoch;
    }
}


static int clock_gettime(int id, struct timespec *ts)
{
  if (id == CLOCK_MONOTONIC) {
    static LARGE_INTEGER frequency;
    LARGE_INTEGER count;
    int64_t numberOfSeconds;

    // Initialize the frequency in a safe mode
    // I don't use thread local for the frequency as apple clang doesn't support it
    try {
      std::call_once(umi::log::flag_frequency,
        []() {
          if (QueryPerformanceCounter(&frequency) == FALSE) {
            throw std::exception();
          };
      });
    }
    catch (...) {
      return -1;
    }

    // Ticks
    if (QueryPerformanceCounter(&count) == FALSE) {
      return -1;
    }

    // Gets the nano seconds
    numberOfSeconds = static_cast<int64_t>(
      static_cast<double>(count.QuadPart) / frequency.QuadPart * 1000000000);
    ts->tv_sec = count.QuadPart / frequency.QuadPart;
    ts->tv_nsec = numberOfSeconds % 1000000000;
  }
  else if (id == CLOCK_REALTIME) {
    static ULARGE_INTEGER unixEpoch;
    try {
      std::call_once(umi::log::flag_uepoch,
        []() {
          SYSTEMTIME unixEpochSt = { 1970, 1, 0, 1, 0, 0, 0, 0 };
          FILETIME unixEpochFt;
          if (SystemTimeToFileTime(&unixEpochSt, &unixEpochFt) == FALSE) {
            throw std::exception();
          }
          unixEpoch.LowPart = unixEpochFt.dwLowDateTime;
          unixEpoch.QuadPart = unixEpochFt.dwHighDateTime;
      });
    }
    catch (...) {
      return -1;
    }
    ULARGE_INTEGER currentTime;
    FILETIME currentTimeFile;

    // Returns the current UTC in 100ns intervals since Jan 1 1601
    GetSystemTimePreciseAsFileTime(&currentTimeFile);
    currentTime.LowPart = currentTimeFile.dwLowDateTime;
    currentTime.HighPart = currentTimeFile.dwHighDateTime;

    // Time from epoch
    ts->tv_sec = (currentTime.QuadPart - unixEpoch.QuadPart) / 10000000;
    ts->tv_nsec = ((currentTime.QuadPart - unixEpoch.QuadPart) % 10000000) * 100;
  }
  else {
    return -1;
  }
  return 0;
}

#endif

namespace umi {
    namespace log {
        /**
         * Forward declaration of elements in the library
         * */
        /**
         * Socket class abstracts the communication handler
         * */
        class socket;

        /**
         * Factory used to create the different socket types
         * */
        class socket_factory;

        /**
         * Local data configuration of the logger
         * */
        class logger_local_data;

        /**
         * Logger clas
         * */
        class logger;

        /**
          \brief Facility

          Facility values MUST be in the range of 0 to 23 inclusive.
        */
        enum class facility : int {
            Kernel_Messages = 0,
            UserLevel_Messages = 1,
            Mail_System = 2,
            System_Daemons = 3,
            Security_Authorization_Messages = 4,
            Messages_Generated_Internally_By_Syslogd = 5,
            Line_Printer_Subsystem = 6,
            Network_News_Subsystem = 7,
            UUCP_Subsystem = 8,
            Clock_Daemon = 9,
            Security_Authorization_Messages_2 = 10,
            FTP_Daemon = 11,
            NTP_Subsystem = 12,
            Log_Audit = 13,
            Log_Alert = 14,
            Clock_Daemon_2 = 15,
            Local_Use_0 = 16,
            Local_Use_1 = 17,
            Local_Use_2 = 18,
            Local_Use_3 = 19,
            Local_Use_4 = 20,
            Local_Use_5 = 21,
            Local_Use_6 = 22,
            Local_Use_7 = 23
        };

        /**
          \brief Severity of the message

          Facility and Severity values are not normative but often used.  They
          are described in the following tables for purely informational
          purposes.

          Each message Priority also has a decimal Severity level indicator.
          These are described in the following table along with their numerical
          values.  Severity values MUST be in the range of 0 to 7 inclusive.
        */
        enum class severity : int {
            Emergency = 0,
            Alert = 1,
            Critical = 2,
            Error = 3,
            Warning = 4,
            Notice = 5,
            Informational = 6,
            Debug = 7
        };

        /**
          \brief Helper method to transform from the input string to the
          output severity.
          It doesn't matter if the string is in upper/lower case, internally
          it will be transformed to the right case and checked against the strings.
          If not found Debug level is returned
        */
        static umi::log::severity string_to_severity(const std::string &value) {
            static const std::unordered_map<std::string, umi::log::severity> unordered_map_string_to_severity =
                    {
                            {"emergency",     umi::log::severity::Emergency},
                            {"alert",         umi::log::severity::Alert},
                            {"critical",      umi::log::severity::Critical},
                            {"error",         umi::log::severity::Error},
                            {"warning",       umi::log::severity::Warning},
                            {"notice",        umi::log::severity::Notice},
                            {"informational", umi::log::severity::Informational},
                            {"debug",         umi::log::severity::Debug}
                    };
            std::string lcValue = boost::to_lower_copy(value);
            const auto element = unordered_map_string_to_severity.find(lcValue);
            if (element == unordered_map_string_to_severity.end()) {
                return umi::log::severity::Debug;
            }
            return element->second;
        }

        /**
          \brief Represents the local parameter data of the Logger
        */
        class logger_local_data {
        public:
            /**
              \brief Default constructor of the logger local data

              \param hostname with the local hostname of the program
              \param version with the actual version to be used in the stream
              \param print flag that will print in console the data to be sent
              \param maxFacility Maximum facility up we will report
              \param maxSeverity maximum severity up we will report
            */
            logger_local_data(const std::string &hostname,
                              uint32_t version,
                              bool print,
                              umi::log::facility maxFacility,
                              umi::log::severity maxSeverity)
                    : m_hostname(hostname),
                      m_version(version),
                      m_print(print),
                      m_maxFacility(maxFacility),
                      m_maxSeverity(maxSeverity) { }


            /**
              \brief Gets the actual hostname
            */
            const std::string &get_hostname() const {
                return m_hostname;
            }

            /**
              \bries Sets the actual hostname
            */
            void set_hostname(const std::string &val) {
                m_hostname = val;
            }

            /**
              \brief Mutable hostname
            */
            std::string &mutable_hostname() {
                return m_hostname;
            }

            /**
              \brief Gets the actual version
            */
            uint32_t get_version() const {
                return m_version;
            }

            /**
              \brief Set Version
            */
            void set_version(uint32_t version) {
                m_version = version;
            }

            /**
              \brief mutable version
            */
            uint32_t &mutable_version() {
                return m_version;
            }

            /**
              \brief Gets the print flag
            */
            bool get_print() const {
                return m_print;
            }

            /**
              \brief Set the print flag
            */
            void set_print(bool value) {
                m_print = value;
            }

            /**
              \brief Mutable set flag
            */
            bool &mutable_print() {
                return m_print;
            }

            /**
              \brief get the precision
            */
            uint32_t get_precision() const {
                return m_precision;
            }

            /**
              \brief Set the precision
            */
            void set_precision(uint32_t val) {
                m_precision = val;
            }

            /**
              \brief Mutable version of the precision
            */
            uint32_t &mutable_precision() {
                return m_precision;
            }

            /**
              \brief Gets the maximum facility to use in the reporting
            */
            umi::log::facility get_max_facility() const {
                return m_maxFacility;
            }

            /**
              \brief Gets the maximum severity to use in the reporting
            */
            umi::log::severity get_max_severity() const {
                return m_maxSeverity;
            }

        protected:
            std::string m_hostname; //!< Hostname of the actual logger
            uint32_t m_version;  //!< Version we are using in this
            bool m_print;  //!< Flag to print on std out before send
            uint32_t m_precision; //!< Precision we want in the timestamp
            umi::log::facility m_maxFacility; //!< The facility up we have to report
            umi::log::severity m_maxSeverity; //!< Max severity up we have to report

        };


        /**
          \brief Timestamp represents the an specific
          time structure in derived from RFC3339

          Whereas [RFC3339] makes allowances for multiple syntaxes, this
          document imposes further restrictions.  The TIMESTAMP value MUST
          follow these restrictions:

          -The "T" and "Z" characters in this syntax MUST be upper case.
          -Usage of the "T" character is REQUIRED.
          -Leap seconds MUST NOT be used.

          Valid outputs:

          1985-04-12T23:20:50.52Z
          1985-04-12T19:20:50.52-04:00
          2003-10-11T22:14:15.003Z
          2003-08-24T05:14:15.000003-07:00

          The maximum number of digits is 6
        */
        class Timestamp {
        public:
            /**
              \brief Generates a timestamp using RFC3339 with the previous
              notations

              \param precision from 0 to 6
            */
            static std::string get_timestamp(uint32_t precision) {
                // We don't allow more precision
                if (precision > 6) {
                    precision = 0;
                }
                struct timespec _actualTime;
                if (clock_gettime(CLOCK_REALTIME, &_actualTime) == 0) {
                    std::string _precisionSeconds;
                    std::string _date;
                    std::string timezone;
                    std::array<char, 256> buffer;
                    struct std::tm timeinfo;
#ifndef _WIN32
                    localtime_r(&_actualTime.tv_sec, &timeinfo);
#else
                    localtime_s(&timeinfo, &_actualTime.tv_sec);
#endif
                    strftime(buffer.data(), buffer.size(), "%Y-%m-%dT%T", &timeinfo);
                    _date = buffer.data();
                    // 1999-12-01T12:14:45
                    if (precision > 0) {
                        uint32_t _microseconds = static_cast<uint32_t>(_actualTime.tv_nsec / 1000);
                        snprintf(buffer.data(), buffer.size(), "%06u", _microseconds);
                        _precisionSeconds = ".";
                        _precisionSeconds += buffer.data();
                        _precisionSeconds = _precisionSeconds.substr(0, 1 + precision);
                    }
                    strftime(buffer.data(), buffer.size(), "%z", &timeinfo);
                    if (strnlen(buffer.data(), buffer.size()) >= 5) {
                        timezone.insert(timezone.end(), buffer.data(), buffer.data() + 3);
                        timezone += ":";
                        timezone.insert(timezone.end(), buffer.data() + 3, buffer.data() + 5);
                    } else {
                        timezone += 'Z';
                    }
                    return _date + _precisionSeconds + timezone;
                } else {
                    // Throw exception
                }
                return std::string();
            }
        };

        /**
          \brief Class to configure the connection against one system

          The connection can be UDP/IP or TCP(SSL only)/IP, we can specify ports or keep it empty
          and the class will choose the default ones

          Default ports

          TLS->6514
          UDP->514
          TCP->514
        */
        class connection {
        public:
            enum class connection_type : int {
                UDP,
                TCP,
                TLS
            };
        public:
            /**
              \brief Default connection configuration
            */
            connection(connection_type type, const std::string &host, int port, const std::string &caFile)
                    : m_connectionType(type),
                      m_host(host),
                      m_ca(caFile) {
                if (port <= 0) {
                    if (m_connectionType == connection_type::TLS) {
                        m_port = 6514;
                    } else {
                        m_port = 514;
                    }
                } else {
                    m_port = static_cast<uint32_t>(port);
                }
            }

            /**
              \brief Copy constructor
            */
            connection(const connection &val)
                    : m_connectionType(val.m_connectionType),
                      m_host(val.m_host),
                      m_port(val.m_port),
                      m_ca(val.m_ca) { }

            /**
              \brief rvalue constructor
            */
            connection(connection &&val)
                    : m_connectionType(std::move(val.m_connectionType)),
                      m_host(std::move(val.m_host)),
                      m_port(std::move(val.m_port)),
                      m_ca(std::move(val.m_ca)) { }

            /**
              \brief Clean the resources used by this connection data
            */
            virtual ~connection() { }

            /**
              \brief Assign operator
            */
            connection &operator=(const connection &val) {
                if (this != &val) {
                    m_connectionType = val.m_connectionType;
                    m_host = val.m_host;
                    m_port = val.m_port;
                    m_ca = val.m_ca;
                }
                return *this;
            }

            /**
              \brief Move operator
            */
            connection &operator=(connection &&val) {
                if (this != &val) {
                    m_connectionType = std::move(val.m_connectionType);
                    m_host = std::move(val.m_host);
                    m_port = std::move(val.m_port);
                    m_ca = std::move(val.m_ca);
                }
                return *this;
            }

            /**
              \brief Sets the ca file in case of SSL connection
            */
            void set_TLS_CA_file(const std::string &file) {
                m_ca = file;
            }

            /**
              \brief Gets the ca file in case of SSL connection
            */
            const std::string &get_TLS_CA_file() const {
                return m_ca;
            }

            /**
              \brief Gets the mutable version of the TLS CA file for SSL connections
            */
            std::string &mutable_TLS_CA_File() {
                return m_ca;
            }

            /**
              \brief Gets the connection type
            */
            umi::log::connection::connection_type get_connection_type() const {
                return m_connectionType;
            }

            /**
              \brief Sets the connection type
            */
            void set_connection_type(umi::log::connection::connection_type val) {
                m_connectionType = val;
            }

            /**
              \brief mutable connection type
            */
            umi::log::connection::connection_type &mutable_connection_type() {
                return m_connectionType;
            }

            /**
              \brief Gets the port
            */
            uint32_t get_port() const {
                return m_port;
            }

            /**
              \brief Sets the port
            */
            void set_port(uint32_t port) {
                m_port = port;
            }

            /**
              \brief Mutable version of the port
            */
            uint32_t &mutable_port() {
                return m_port;
            }

            /**
              \brief Gets the host we will send the data
            */
            const std::string &get_host() const {
                return m_host;
            }

            /**
              \brief Sets the host we will send the data
            */
            void set_host(const std::string &val) {
                m_host = val;
            }

            /**
              \brief Mutable version of the host
            */
            std::string &mutable_host() {
                return m_host;
            }

        protected:
            connection_type m_connectionType;  //!< Connection we are using(the type)
            std::string m_host;  //!< host we will send the data
            uint32_t m_port;  //!< Port we are using in the communication
            std::string m_ca; //!< Certificate authority
        };

        /**
           \brief Class to use structured data on the logs

           STRUCTURED-DATA provides a mechanism to express information in a well
           defined, easily parseable and interpretable data format.  There are
           multiple usage scenarios.  For example, it may express meta-
           information about the syslog message or application-specific
           information such as traffic counters or IP addresses.

           STRUCTURED-DATA can contain zero, one, or multiple structured data
           elements, which are referred to as "SD-ELEMENT" in this document.
         */
        class structured_data {
        public:
            struct sd_element {
                // Takes input string and returns an scaped string
                std::string escape(const std::string &val) const {

                }
                void add_param(const std::string &param_name, const std::string &param_value) {
                    std::string local_value(sd_element::escape(param_value));
                }
                std::string m_id;
                std::vector<std::pair<std::string, std::string>> m_params;
            };
        protected:

        };

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
            inline static constexpr int get_priority(umi::log::facility facility, umi::log::severity severity) {
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

        /**
          \brief Class to represent a socket connection
          against the rsyslog server.

          As we know udp connections differs from tcp and
          we can't use ssl with them
        */
        class socket {
            friend class socket_factory;

        public:
            /**
              \brief Clean the connection
            */
            virtual ~socket() { }

            /**
              \brief Sends the data
            */
            virtual void send(std::shared_ptr<std::string> message) = 0;

        protected:
            /**
              \brief Gets the internal boost asio
            */
            boost::asio::io_service &get_internal_service() {
                return m_logger.m_ioservice;
            }

            /**
              \brief Constructor with the default connection
              information data
            */
            socket(umi::log::logger &logger, const umi::log::connection &loggerInfo)
                    : m_logger(logger),
                      m_loggerInfo(loggerInfo) {
            }

            /**
             * Logger to use for the dispatcher information
             * */
            umi::log::logger &m_logger; //!< Logger to get the dispatcher
            /**
             * Information to use in the connection
             * */
            const umi::log::connection &m_loggerInfo;
        };

        class socket_udp : public socket {
        public:
            socket_udp(umi::log::logger &logger,
                       const umi::log::connection &loggerInfo)
                    : socket(logger, loggerInfo),
                      m_socket(std::make_unique<boost::asio::ip::udp::socket>(get_internal_service())) {
                m_socket->open(boost::asio::ip::udp::v4());
                boost::asio::ip::udp::resolver _resolver(get_internal_service());
                boost::asio::ip::udp::resolver::query _query(
                        boost::asio::ip::udp::v4(),
                        m_loggerInfo.get_host().c_str(),
                        boost::lexical_cast<std::string>(m_loggerInfo.get_port()));
                m_endpoint = std::make_unique<boost::asio::ip::udp::endpoint>(*_resolver.resolve(_query));
                m_isOpen = true;
            }

            virtual ~socket_udp() {
                if (m_socket) {
                    m_socket->close();
                }
            }

            void send(std::shared_ptr<std::string> message) {
                if (m_isOpen && m_socket) {
                    m_socket->async_send_to(
                            boost::asio::buffer(*message),
                            *m_endpoint,
                            std::bind(&umi::log::socket_udp::handler_send, this,
                                      message,
                                      0,
                                      std::placeholders::_1,
                                      std::placeholders::_2));
                }
            }

            void handler_send(std::shared_ptr<std::string> message,
                              std::size_t actual_position,
                              const boost::system::error_code &error,
                              std::size_t dataSent) {
                if (!error) {
                    if (m_isOpen && m_socket && (actual_position + dataSent) < message->size()) {
                        m_socket->async_send_to(
                                boost::asio::buffer((*message).substr(actual_position + dataSent)),
                                *m_endpoint,
                                std::bind(&umi::log::socket_udp::handler_send, this,
                                          message, //!< Thanks to this and the shared_ptr the message will not die
                                          (actual_position + dataSent),
                                          std::placeholders::_1,
                                          std::placeholders::_2));
                    }
                }
            }

        protected:
            /**
             * Flag to describe if the socket is open or not
             * */
            bool m_isOpen = false;
            /**
             * Socket used
             * */
            std::unique_ptr<boost::asio::ip::udp::socket> m_socket;
            /**
             * Endpoint where we connect
             * */
            std::unique_ptr<boost::asio::ip::udp::endpoint> m_endpoint;
        };

        class socket_tcp : public socket {
        public:
            socket_tcp(umi::log::logger &logger,
                       const umi::log::connection &loggerInfo)
                    : socket(logger, loggerInfo),
                      m_socket(std::make_unique<boost::asio::ip::tcp::socket>(get_internal_service())) {
                if (m_socket) {
                    boost::asio::socket_base::keep_alive _keepAlive(true);
                    m_socket->set_option(_keepAlive);
                    boost::asio::ip::tcp::resolver _resolver(get_internal_service());
                    boost::asio::ip::tcp::resolver::query _query(
                            m_loggerInfo.get_host().c_str(),
                            boost::lexical_cast<std::string>(
                                    m_loggerInfo.get_port()));
                    boost::asio::ip::tcp::resolver::iterator _endpoint(_resolver.resolve(_query));
                    boost::asio::ip::tcp::endpoint endPoint = *_endpoint;
                    m_socket->async_connect(
                            endPoint,
                            std::bind(&umi::log::socket_tcp::handle_on_connect, this,
                                      std::placeholders::_1,
                                      ++_endpoint));
                }
            }

            virtual ~socket_tcp() {
                if (m_socket) {
                    m_socket->close(); // this is not async
                }
            }

            void send(std::shared_ptr<std::string> message) {
                if (m_isOpen && m_socket) {
                    m_socket->async_send(
                            boost::asio::buffer(*message),
                            std::bind(&umi::log::socket_tcp::handler_send, this,
                                      message, //!< Thanks to this and the shared_ptr the message will not die
                                      0,
                                      std::placeholders::_1,
                                      std::placeholders::_2));
                }
            }

            void handle_on_connect(const boost::system::error_code &errorCode,
                                   boost::asio::ip::tcp::resolver::iterator endpointIT) {
                if (m_socket) {
                    if (!errorCode) {
                        m_isOpen = true;
                    } else if (endpointIT != boost::asio::ip::tcp::resolver::iterator()) {
                        // try next
                        m_socket->close();
                        boost::asio::ip::tcp::endpoint endPoint = *endpointIT;
                        m_socket->async_connect(
                                endPoint,
                                std::bind(&umi::log::socket_tcp::handle_on_connect, this,
                                          std::placeholders::_1,
                                          ++endpointIT));
                    }
                }
            }

            void handler_send(std::shared_ptr<std::string> message,
                              std::size_t last_position,
                              const boost::system::error_code &errorCode,
                              std::size_t dataSent) {
                if (!errorCode) {
                    if (m_socket && m_isOpen && (last_position + dataSent) < message->size()) {
                        m_socket->async_send(
                                boost::asio::buffer((*message).substr(last_position + dataSent)),
                                std::bind(&umi::log::socket_tcp::handler_send, this,
                                          message,
                                          last_position + dataSent,
                                          std::placeholders::_1,
                                          std::placeholders::_2));
                    }
                }
            }

        protected:
            /**
             * Flag to mark when the socket is opened
             * */
            bool m_isOpen = false;
            /**
             * The tcp socket
             * */
            std::unique_ptr<boost::asio::ip::tcp::socket> m_socket;
        };

        class socket_tls : public socket {
        public:
            socket_tls(umi::log::logger &logger,
                       const umi::log::connection &loggerInfo)
                    : socket(logger, loggerInfo),
                      m_sslContext(std::make_unique<boost::asio::ssl::context>(
                              get_internal_service(),
                              boost::asio::ssl::context::sslv23)),
                      m_socket() {
                if (m_sslContext) {
                    m_sslContext->set_verify_mode(boost::asio::ssl::context::verify_peer);
                    m_sslContext->set_options(boost::asio::ssl::context::default_workarounds |
                                              boost::asio::ssl::context::no_sslv2 |
                                              boost::asio::ssl::context::no_sslv3 |
                                              boost::asio::ssl::context::single_dh_use);
                    if (!loggerInfo.get_TLS_CA_file().empty()) {
                        m_sslContext->load_verify_file(loggerInfo.get_TLS_CA_file());
                    }
                    m_socket = std::make_unique<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>
                            (get_internal_service(),
                             *m_sslContext);
                    if (m_socket) {
                        boost::asio::socket_base::keep_alive _keepAlive(true);
                        m_socket->lowest_layer().set_option(_keepAlive);
                        boost::asio::ip::tcp::resolver _resolver(get_internal_service());
                        boost::asio::ip::tcp::resolver::query _query(m_loggerInfo.get_host().c_str(),
                                                                     boost::lexical_cast<std::string>(
                                                                             m_loggerInfo.get_port()));
                        boost::asio::ip::tcp::resolver::iterator _endpoint(_resolver.resolve(_query));
                        boost::asio::ip::tcp::endpoint endPoint = *_endpoint;
                        m_socket->lowest_layer().async_connect(endPoint,
                                                               std::bind(&umi::log::socket_tls::handle_on_connect,
                                                                         this,
                                                                         std::placeholders::_1,
                                                                         ++_endpoint));
                    }
                }
            }

            virtual ~socket_tls() {
                if (m_socket) {
                    m_socket->lowest_layer().close(); // this is not async
                }
            }

            void send(std::shared_ptr<std::string> message) {
                if (m_isOpen && m_socket) {
                    boost::asio::async_write(
                            *m_socket,
                            boost::asio::buffer(*message),
                            std::bind(&umi::log::socket_tls::handler_send, this,
                                      message, //!< Thanks to this and the shared_ptr the message will not die
                                      0,
                                      std::placeholders::_1,
                                      std::placeholders::_2));
                }
            }

            void handle_on_connect(const boost::system::error_code &errorCode,
                                   boost::asio::ip::tcp::resolver::iterator endpointIT) {
                if (m_socket) {
                    if (!errorCode) {
                        m_socket->async_handshake(boost::asio::ssl::stream_base::client,
                                                  std::bind(&umi::log::socket_tls::handle_on_handshake,
                                                            this,
                                                            std::placeholders::_1));
                    } else if (endpointIT != boost::asio::ip::tcp::resolver::iterator()) {
                        // try next
                        m_socket->lowest_layer().close();
                        boost::asio::ip::tcp::endpoint endPoint = *endpointIT;
                        m_socket->lowest_layer().async_connect(
                                endPoint,
                                std::bind(&umi::log::socket_tls::handle_on_connect,
                                          this,
                                          std::placeholders::_1,
                                          ++endpointIT));
                    }
                }
            }

            void handle_on_handshake(const boost::system::error_code &error) {
                if (!error) {
                    m_isOpen = true;
                }
            }

            void handler_send(std::shared_ptr<std::string> message,
                              std::size_t last_position,
                              const boost::system::error_code &errorCode,
                              std::size_t dataSent) {
                if (!errorCode) {
                    if (m_socket && m_isOpen && (last_position + dataSent) < message->size()) {
                        boost::asio::async_write(
                                *m_socket,
                                boost::asio::buffer((*message).substr(last_position + dataSent)),
                                std::bind(&umi::log::socket_tls::handler_send, this,
                                          message, //!< Thanks to this and the shared_ptr the message will not die
                                          last_position + dataSent,
                                          std::placeholders::_1,
                                          std::placeholders::_2));
                    }
                }
            }

        protected:
            /**
             * Flag to mark the socket is open
             * */
            bool m_isOpen = false;
            /**
             * SSL context used in the connection
             * */
            std::unique_ptr<boost::asio::ssl::context> m_sslContext;
            /**
             * Socket used in the connection
             * */
            std::unique_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> m_socket;
        };

        /**
          \brief Factory to create sockets depending on the logger
          configuration
        */
        class socket_factory {
        public:
            /**
              \brief Creates a socket with the desired configuration
            */
            static socket *create_socket(umi::log::logger &logger,
                                         const umi::log::connection &loggerInfo) {
                if (loggerInfo.get_connection_type() == umi::log::connection::connection_type::UDP) {
                    return new umi::log::socket_udp(logger, loggerInfo);
                } else if (loggerInfo.get_connection_type() == umi::log::connection::connection_type::TCP) {
                    return new umi::log::socket_tcp(logger, loggerInfo);
                } else if (loggerInfo.get_connection_type() == umi::log::connection::connection_type::TLS) {
                    return new umi::log::socket_tls(logger, loggerInfo);
                }
                return 0;
            }

        };
    }
}

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

/**
 * \brief Process the messages
 * */
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
