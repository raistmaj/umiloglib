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
    localtime_r(&_actualTime.tv_sec,&timeinfo);
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
