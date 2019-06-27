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

#include "stsff/logging/utils/SourceName.h"

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

namespace stsff {
namespace logging {

    class CodeLocation final {
    public:

        typedef internal::CustStringView StringView;

        //---------------------------------------------------------------
        /// @{

        CodeLocation() = default;

        CodeLocation(const StringView function, const StringView file, const int line) noexcept
            : mFunction(function),
              mFile(file),
              mLine(line) {}

        CodeLocation(const CodeLocation &) = default;
        CodeLocation(CodeLocation &&) = default;

        ~CodeLocation() = default;

        CodeLocation & operator=(const CodeLocation &) = default;
        CodeLocation & operator=(CodeLocation &&) = default;

        /// @}
        //---------------------------------------------------------------
        /// @{

        StringView mFunction; //!< function name.
        StringView mFile;     //!< source file name.
        int mLine = 0;        //!< source line number.

        /// @}
        //---------------------------------------------------------------

    };

}
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

/*!
 * \details define this macro if you have macro conflicts
 *          and then make your one macros like this.
 */
#ifndef STSFF_LOGGER_DON_NOT_USE_MACROS

#define MakeCodeLocation() CodeLocation(__STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__)
#define MakeVarCodeLocation(VAR) CodeLocation VAR(__STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__)

#endif

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
