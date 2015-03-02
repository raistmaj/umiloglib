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
#include "umilocaldata.hpp"

umi::LoggerLocalData::LoggerLocalData(const std::string &hostname,
                                      uint32_t version, bool print,
                                      umi::Facility maxFacility, umi::Severity maxSeverity)
  : m_hostname(hostname),
    m_version(version),
    m_print(print),
    m_precision(6),
    m_maxFacility(maxFacility),
    m_maxSeverity(maxSeverity)
{

}

umi::LoggerLocalData::LoggerLocalData(const LoggerLocalData &localData)
  : m_hostname(localData.m_hostname),
    m_version(localData.m_version),
    m_print(localData.m_print),
    m_precision(localData.m_precision),
    m_maxFacility(localData.m_maxFacility),
    m_maxSeverity(localData.m_maxSeverity)
{
}

umi::LoggerLocalData::LoggerLocalData(LoggerLocalData &&localData)
  : m_hostname(std::move(localData.m_hostname)),
    m_version(std::move(localData.m_version)),
    m_print(std::move(localData.m_print)),
    m_precision(std::move(localData.m_precision)),
    m_maxFacility(std::move(localData.m_maxFacility)),
    m_maxSeverity(std::move(localData.m_maxSeverity))
{
}

umi::LoggerLocalData::~LoggerLocalData()
{

}

umi::LoggerLocalData& umi::LoggerLocalData::operator=(const umi::LoggerLocalData& val)
{
  if(this != &val){
    m_hostname = val.m_hostname;
    m_version = val.m_version;
    m_print = val.m_print;
    m_precision = val.m_precision;
    m_maxFacility = val.m_maxFacility;
    m_maxSeverity = val.m_maxSeverity;
  }
  return *this;
}

umi::LoggerLocalData& umi::LoggerLocalData::operator=(umi::LoggerLocalData&& val)
{
  m_hostname = std::move(val.m_hostname);
  m_version = std::move(val.m_version);
  m_print = std::move(val.m_print);
  m_precision = std::move(val.m_precision);
  m_maxFacility = std::move(val.m_maxFacility);
  m_maxSeverity = std::move(val.m_maxSeverity);
  return *this;
}

const std::string& umi::LoggerLocalData::getHostname() const
{
  return m_hostname;
}

void umi::LoggerLocalData::setHostname(const std::string& val)
{
  m_hostname = val;
}

std::string& umi::LoggerLocalData::mutableHostname()
{
  return m_hostname;
}

uint32_t umi::LoggerLocalData::getVersion() const
{
  return m_version;
}

void umi::LoggerLocalData::setVersion(uint32_t version)
{
  m_version = version;
}

uint32_t& umi::LoggerLocalData::mutableVersion()
{
  return m_version;
}

bool umi::LoggerLocalData::getPrint() const
{
  return m_print;
}

void umi::LoggerLocalData::setPrint(bool value)
{
  m_print = value;
}

bool& umi::LoggerLocalData::mutablePrint()
{
  return m_print;
}

uint32_t umi::LoggerLocalData::getPrecision() const
{
  return m_precision;
}

void umi::LoggerLocalData::setPrecision(uint32_t val)
{
  m_precision = val;
}

uint32_t& umi::LoggerLocalData::mutablePrecision()
{
  return m_precision;
}

umi::Facility umi::LoggerLocalData::getMaxFacility() const
{
  return m_maxFacility;
}

umi::Severity umi::LoggerLocalData::getMaxSeverity() const
{
  return m_maxSeverity;
}
