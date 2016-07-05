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
#ifndef UMILOCALDATA_HPP
#define UMILOCALDATA_HPP

#include <string>
#include <cstring>
#include "umiseverity.hpp"
#include "umifacility.hpp"

namespace umi {
    namespace log {
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
    }
}


#endif // UMILOCALDATA_HPP

