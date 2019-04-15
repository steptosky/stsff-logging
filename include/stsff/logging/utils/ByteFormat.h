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

#include <cstdint>
#include <string>
#include "stsff/logging/Export.h"

namespace stsff {
namespace logging {

    /**************************************************************************************************/
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**************************************************************************************************/

    class ByteFormat {
        ByteFormat() = default;
    public:

        typedef std::uint64_t Size;

        enum eBinValue : Size {
            BYTE = 1,
            KILOBYTE = 1024 * BYTE,
            MEGABYTE = 1024 * KILOBYTE,
            GIGABYTE = 1024 * MEGABYTE,
            TERABYTE = 1024 * GIGABYTE,
            PETABYTE = 1024 * TERABYTE,
            EXABYTE = 1024 * PETABYTE,
        };

        enum eSiValue : Size {
            SI_BYTE = 1,
            SI_KILOBYTE = 1000 * SI_BYTE,
            SI_MEGABYTE = 1000 * SI_KILOBYTE,
            SI_GIGABYTE = 1000 * SI_MEGABYTE,
            SI_TERABYTE = 1000 * SI_GIGABYTE,
            SI_PETABYTE = 1000 * SI_TERABYTE,
            SI_EXABYTE = 1000 * SI_PETABYTE,
        };

        LoggingExp static std::string bin(Size size);
        LoggingExp static std::string si(Size size);

    };

    /**************************************************************************************************/
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**************************************************************************************************/

}
}
