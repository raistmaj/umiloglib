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
#ifndef UMITLOGTIMESTAMP_HPP
#define UMITLOGTIMESTAMP_HPP

#include <string>
#include <cstdint>

namespace umi{
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
  class Timestamp{
  public:
    /**
      \brief Generates a timestamp using RFC3339 with the previous
      notations

      \param precision from 0 to 6
    */
    static std::string getTimestamp(uint32_t precision);
  };
}

#endif // UMITLOGTIMESTAMP_HPP

