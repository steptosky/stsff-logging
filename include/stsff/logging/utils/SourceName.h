#pragma once

/*
**  Copyright(C) 2019, StepToSky and FlightFactor
**  All rights reserved
**
**  Redistribution and use in source and binary forms, with or without
**  modification, are permitted provided that the following conditions are met:
**
**  1.Redistributions of source code must retain the above copyright notice, this
**    list of conditions and the following disclaimer.
**  2.Redistributions in binary form must reproduce the above copyright notice,
**    this list of conditions and the following disclaimer in the documentation
**    and / or other materials provided with the distribution.
**  3.The name of StepToSky or the name of FlightFactor or the names of its
**    contributors may NOT be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**  DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
**  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**  Contacts: www.steptosky.com or www.flightfactor.aero
*/

#include "stsff/logging/internal/InternalUtils.h"

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

namespace stsff {
namespace logging {

#ifdef STSFF_LOGGER_USE_FULL_SOURCES_PATH
    constexpr const char * sourceName(const char * str) {
        return str;
    }
#else
    constexpr const char * __strEnd(const char * str) {
        return *str ? __strEnd(str + 1) : str;
    }

    constexpr bool __strSlant(const char * str, const char sep) {
        return *str == sep ? true : (*str ? __strSlant(str + 1, sep) : false);
    }

    constexpr const char * __rSlant(const char * str, const char sep) {
        return *str == sep ? (str + 1) : __rSlant(str - 1, sep);
    }

    constexpr const char * sourceName(const char * str) {
        // c++ 11 doesn't allow declaration variables in constexpr
        // it is available since 14.
        // so the code might be:
        // const char * res1 = strSlant(str, '\\') ? rSlant(strEnd(str), '\\') : str;
        // const char * res2 = strSlant(res1, '/') ? rSlant(strEnd(res1), '/') : res1;
        // return res2;
#   if defined(STSFF_LOGGER_OS_MACOS) || defined(STSFF_LOGGER_OS_LINUX)
        return __strSlant(str, '/') ? __rSlant(__strEnd(str), '/') : str;
#   elif defined(STSFF_LOGGER_OS_WINDOWS)
        return __strSlant(str, '\\') ? __rSlant(__strEnd(str), '\\') : str;
#   endif
    }
#endif

}
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
