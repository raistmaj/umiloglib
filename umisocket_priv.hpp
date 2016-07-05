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

#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

namespace umi {
    namespace log {
        /**
          \brief Creates a socket depending on the
          specify configuration

          Forward declaration
        */
        class socket_factory;
        /**
         * \brief Creates an udp socket to use
         * for a connection against a collector
         *
         * Forward declaration
         * */
        class socket_udp;

        /**
         * \brief Creates a tcp socket to use
         * for a connection against a collector
         *
         * Forward declaration
         * */
        class socket_tcp;

        /**
         * \brief Creates a tcp using tls socket to use
         * for a connection against a collector
         *
         * Forward declaration
         * */
        class socket_tls;

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
                            boost::bind(&umi::log::socket_udp::handler_send, this,
                                        message, //!< Thanks to this and the shared_ptr the message will not die
                                        0,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
                }
            }

            void handler_send(std::shared_ptr<std::string> message,
                              std::size_t actualPosition,
                              const boost::system::error_code &error,
                              std::size_t dataSent) {
                if (!error) {
                    if (m_isOpen && m_socket && (actualPosition + dataSent) < message->size()) {
                        m_socket->async_send_to(
                                boost::asio::buffer((*message).substr(actualPosition + dataSent)),
                                *m_endpoint,
                                boost::bind(&umi::log::socket_udp::handler_send, this,
                                            message, //!< Thanks to this and the shared_ptr the message will not die
                                            (actualPosition + dataSent),
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
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
                            boost::bind(&umi::log::socket_tcp::handle_on_connect, this,
                                        boost::asio::placeholders::error,
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
                            boost::bind(&umi::log::socket_tcp::handler_send, this,
                                        message, //!< Thanks to this and the shared_ptr the message will not die
                                        0,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
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
                                boost::bind(&umi::log::socket_tcp::handle_on_connect, this,
                                            boost::asio::placeholders::error,
                                            ++endpointIT));
                    }
                }
            }

            void handler_send(std::shared_ptr<std::string> message,
                              std::size_t lastPosition,
                              const boost::system::error_code &errorCode,
                              std::size_t dataSent) {
                if (!errorCode) {
                    if (m_socket && m_isOpen && (lastPosition + dataSent) < message->size()) {
                        m_socket->async_send(
                                boost::asio::buffer((*message).substr(lastPosition + dataSent)),
                                boost::bind(&umi::log::socket_tcp::handler_send, this,
                                            message,
                                            lastPosition + dataSent,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
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
                                                               boost::bind(&umi::log::socket_tls::handle_on_connect,
                                                                           this,
                                                                           boost::asio::placeholders::error,
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
                    boost::asio::async_write(*m_socket,
                                             boost::asio::buffer(*message),
                                             boost::bind(&umi::log::socket_tls::handler_send, this,
                                                         message, //!< Thanks to this and the shared_ptr the message will not die
                                                         0,
                                                         boost::asio::placeholders::error,
                                                         boost::asio::placeholders::bytes_transferred));
                }
            }

            void handle_on_connect(const boost::system::error_code &errorCode,
                                   boost::asio::ip::tcp::resolver::iterator endpointIT) {
                if (m_socket) {
                    if (!errorCode) {
                        m_socket->async_handshake(boost::asio::ssl::stream_base::client,
                                                  boost::bind(&umi::log::socket_tls::handle_on_handshake, this,
                                                              boost::asio::placeholders::error));
                    } else if (endpointIT != boost::asio::ip::tcp::resolver::iterator()) {
                        // try next
                        m_socket->lowest_layer().close();
                        boost::asio::ip::tcp::endpoint endPoint = *endpointIT;
                        m_socket->lowest_layer().async_connect(
                                endPoint,
                                boost::bind(&umi::log::socket_tls::handle_on_connect,
                                            this,
                                            boost::asio::placeholders::error,
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
                              std::size_t lastPosition,
                              const boost::system::error_code &errorCode,
                              std::size_t dataSent) {
                if (!errorCode) {
                    if (m_socket && m_isOpen && (lastPosition + dataSent) < message->size()) {
                        boost::asio::async_write(
                                *m_socket,
                                boost::asio::buffer((*message).substr(lastPosition + dataSent)),
                                boost::bind(&umi::log::socket_tls::handler_send, this,
                                            message, //!< Thanks to this and the shared_ptr the message will not die
                                            lastPosition + dataSent,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
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

#endif // UMISOCKET_HPP

