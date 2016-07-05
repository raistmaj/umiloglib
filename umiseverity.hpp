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
#ifndef UMISEVERITY_HPP
#define UMISEVERITY_HPP

#include<string>
#include <boost/algorithm/string.hpp>
#include <unordered_map>

namespace umi {
    namespace log {
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
        umi::log::severity string_to_severity(const std::string &value) {
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
    }
}

#endif // UMISEVERITY_HPP

