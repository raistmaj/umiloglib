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

namespace umi {
    namespace log {
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
            uint32_t get_port() const  {
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
    }
}

#endif // UMILOGCONNECTION_HPP

