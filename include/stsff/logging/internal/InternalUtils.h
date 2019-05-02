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

#include "stsff/logging/Export.h"
#include <cstddef>
#include <string>

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

#ifndef NDEBUG
#   define STSFF_LOGGER_USE_FULL_SOURCES_PATH
#endif

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

#ifndef __STS_FUNC_NAME__
#	ifdef _MSC_VER
#		define __STS_FUNC_NAME__ __FUNCTION__
#	else
#		define __STS_FUNC_NAME__ __PRETTY_FUNCTION__
#	endif
#endif

#if defined(_WIN32) || defined(_WIN64)
#   define STSFF_LOGGER_OS_WINDOWS
#elif defined(__APPLE__)
#   define STSFF_LOGGER_OS_MACOS
#elif defined(__unix__) || defined(__unix)
#   define STSFF_LOGGER_OS_LINUX
#else
#   error unsupported platform
#endif

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

namespace stsff {
namespace logging {
    namespace internal {

#ifdef STSFF_LOGGER_USE_FULL_SOURCES_PATH
        constexpr const char * fileName(const char * str) {
            return str;
        }
#else
        constexpr const char * strEnd(const char * str) {
            return *str ? strEnd(str + 1) : str;
        }

        constexpr bool strSlant(const char * str, const char sep) {
            return *str == sep ? true : (*str ? strSlant(str + 1, sep) : false);
        }

        constexpr const char * rSlant(const char * str, const char sep) {
            return *str == sep ? (str + 1) : rSlant(str - 1, sep);
        }

        constexpr const char * fileName(const char * str) {
            // c++ 11 doesn't allow declaration variables in constexpr
        // it is available since 14.
        // so the code might be:
        // const char * res1 = strSlant(str, '\\') ? rSlant(strEnd(str), '\\') : str;
        // const char * res2 = strSlant(res1, '/') ? rSlant(strEnd(res1), '/') : res1;
        // return res2;
#   if defined(STSFF_LOGGER_OS_MACOS) || defined(STSFF_LOGGER_OS_LINUX)
                return strSlant(str, '/') ? rSlant(strEnd(str), '/') : str;
#   elif defined(STSFF_LOGGER_OS_WINDOWS)
                return strSlant(str, '\\') ? rSlant(strEnd(str), '\\') : str;
#   endif
        }
#endif

        /*!
         * \brief Represents string message of the logger.
         * \details Actually it should be deleted when we can use std::string_view.
         *          This class is just alternative for 11,14 C++
         *          because string_view was introduced since C++ 17.
         */
        class CustStringView {
        public:

            //---------------------------------------------------------------
            /// @{
            CustStringView() = default;

            LoggingExp CustStringView(const char * data);

            explicit CustStringView(const std::string & str)
                : CustStringView(str.data(), str.size()) {}

            CustStringView(const char * data, const std::size_t size)
                : mData(data),
                  mSize(size) {}

            template<typename T>
            CustStringView(const T & str)
                : CustStringView(str.data(), str.size()) {}

            CustStringView & operator=(const CustStringView & other) = default;

            /// @}
            //---------------------------------------------------------------
            /// @{

            bool empty() const { return !mData || mSize == 0; }
            std::size_t size() const { return mSize; }
            const char * data() const { return mData; }

            /// @}
            //---------------------------------------------------------------

        private:

            const char * mData = nullptr;
            std::size_t mSize = 0;

        };

    }
}
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
