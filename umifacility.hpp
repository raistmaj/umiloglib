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
#ifndef UMIFACILITY_HPP
#define UMIFACILITY_HPP

namespace umi {
    namespace log {
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
    }
}

#endif // UMIFACILITY_HPP

