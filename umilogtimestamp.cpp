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
#include "umilogtimestamp.hpp"
#include <boost/lexical_cast.hpp>
#include <ctime>
#include <array>
#include <iostream>

// clock_gettime is missing on windows
#ifdef _WIN32
#include <windows.h>
#include <mutex>
typedef unsigned int clockid_t;
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 2
#endif

std::once_flag flag_frequency;
std::once_flag flag_uepoch;

static int clock_gettime(int id, struct timespec *ts)
{
  if (id == CLOCK_MONOTONIC) {
    static LARGE_INTEGER frequency;
    LARGE_INTEGER count;
    int64_t numberOfSeconds;

    // Initialize the frequency in a safe mode
    // I don't use thread local for the frequency as apple clang doesn't support it
    try {
      std::call_once(flag_frequency, 
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
      std::call_once(flag_uepoch,
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

std::string umi::Timestamp::getTimestamp(uint32_t precision){
  // We don't allow more precision
  if(precision > 6) {
    precision = 0;
  }
  struct timespec _actualTime;
  if(clock_gettime(CLOCK_REALTIME,&_actualTime) == 0) {
    std::string _precisionSeconds;
    std::string _date;
    std::string _timezone;
    std::array<char,256> buffer;
    struct std::tm timeinfo;
#ifndef _WIN32
    localtime_r(&_actualTime.tv_sec,&timeinfo);
#else
    localtime_s(&timeinfo, &_actualTime.tv_sec);
#endif
    strftime(buffer.data(),buffer.size(),"%Y-%m-%dT%T",&timeinfo);
    _date = buffer.data();
    // 1999-12-01T12:14:45
    if(precision > 0) {
      uint32_t _microseconds = static_cast<uint32_t>(_actualTime.tv_nsec / 1000);
      snprintf(buffer.data(),buffer.size(),"%06u",_microseconds);
      _precisionSeconds = ".";
      _precisionSeconds += buffer.data();
      _precisionSeconds = _precisionSeconds.substr(0,1+precision);
    }
    strftime(buffer.data(),buffer.size(),"%z",&timeinfo);
    if(strnlen(buffer.data(),buffer.size()) >= 5) {
      _timezone.insert(_timezone.end(),buffer.data(),buffer.data()+3);
      _timezone += ":";
      _timezone.insert(_timezone.end(),buffer.data()+3,buffer.data()+5);
    }else {
      _timezone += 'Z';
    }
    return _date + _precisionSeconds + _timezone;
  }else {
    // Throw exception
  }
  return std::string();
}
