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

namespace umi{

  /**
    \brief Represents the local parameter data of the Logger
  */
  class LoggerLocalData{
  public:
    /**
      \brief Default constuctor of the logger local data

      \param hostname with the local hostname of the program
      \param version with the actual version to be used in the stream
      \param print flag that will print in console the data to be sent
      \param maxFacility Maximum facility up we will report
      \param maxSeverity maximum severity up we will report
    */
    LoggerLocalData(const std::string& hostname,
                    uint32_t version, bool print,
                    umi::Facility maxFacility, umi::Severity maxSeverity);
    /**
      \brief Copy constructor of the LoggerLocalData

      \param localData element we want to copy
    */
    LoggerLocalData(const LoggerLocalData& localData);
    /**
      \brief Rvalue copy constructor

      \param localData element we want to copy, localData is an rvalue
    */
    LoggerLocalData(LoggerLocalData&& localData);
    /**
      \brief Release the resources used by the logger local data
    */
    virtual ~LoggerLocalData();
    /**
      \brief Assign operator of the element
    */
    LoggerLocalData& operator=(const LoggerLocalData& val);
    /**
      \brief Move assign operator of the element
    */
    LoggerLocalData& operator=(LoggerLocalData&& val);
    /**
      \brief Gets the actual hostname
    */
    const std::string& getHostname() const;
    /**
      \bries Sets the actual hostname
    */
    void setHostname(const std::string& val);
    /**
      \brief Mutable hostname
    */
    std::string& mutableHostname();
    /**
      \brief Gets the actual version
    */
    uint32_t getVersion() const;
    /**
      \brief Set Version
    */
    void setVersion(uint32_t version);
    /**
      \brief mutable version
    */
    uint32_t& mutableVersion();
    /**
      \brief Gets the print flag
    */
    bool getPrint() const;
    /**
      \brief Set the print flag
    */
    void setPrint(bool value);
    /**
      \brief Mutable set flag
    */
    bool& mutablePrint();
    /**
      \brief get the precision
    */
    uint32_t getPrecision() const;
    /**
      \brief Set the precision
    */
    void setPrecision(uint32_t val);
    /**
      \brief Mutable version of the precision
    */
    uint32_t& mutablePrecision();
    /**
      \brief Gets the maximum facility to use in the reporting
    */
    umi::Facility getMaxFacility() const;
    /**
      \brief Gets the maximum severity to use in the reporting
    */
    umi::Severity getMaxSeverity() const;
  protected:
    std::string m_hostname; //!< Hostname of the actual logger
    uint32_t m_version;  //!< Version we are using in this
    bool m_print;  //!< Flag to print on stdout before send
    uint32_t m_precision; //!< Precision we want in the timestamp
    umi::Facility m_maxFacility; //!< The facility up we have to report
    umi::Severity m_maxSeverity; //!< Max severity up we have to report
  };

}


#endif // UMILOCALDATA_HPP

