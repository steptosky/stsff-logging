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

#include "stdafx.h"

#include "stsff/logging/utils/ByteFormat.h"

namespace stsff {
namespace logging {

    /**************************************************************************************************/
    //////////////////////////////////////////* Functions */////////////////////////////////////////////
    /**************************************************************************************************/

    inline std::string byteFormat(const char * formatting, const float value, const char * unit) noexcept {
        char buff[10] = {0};
#ifdef _MSC_VER
        return (sprintf_s(buff, 10, formatting, value, unit) < 0) ? std::string() : std::string(buff);
#else
        return (sprintf(buff, formatting, value, unit) < 0) ? std::string() : std::string(buff);
#endif
    }

    /**************************************************************************************************/
    //////////////////////////////////////////* Functions */////////////////////////////////////////////
    /**************************************************************************************************/

    std::string ByteFormat::bin(const Size size) noexcept {
        const char * unit = nullptr;
        auto value = float(size);

        if (size >= EXABYTE) {
            unit = "EiB";
            value /= EXABYTE;
        }
        else if (size >= PETABYTE) {
            unit = "PiB";
            value /= PETABYTE;
        }
        else if (size >= TERABYTE) {
            unit = "TiB";
            value /= TERABYTE;
        }
        else if (size >= GIGABYTE) {
            unit = "GiB";
            value /= GIGABYTE;
        }
        else if (size >= MEGABYTE) {
            unit = "MiB";
            value /= MEGABYTE;
        }
        else if (size >= KILOBYTE) {
            unit = "KiB";
            value /= KILOBYTE;
        }
        else if (size >= BYTE) {
            unit = "B";
        }
        else if (size == 0) {
            return "0 B";
        }

        return size < KILOBYTE ? byteFormat("%.0f %s", value, unit) : byteFormat("%.1f %s", value, unit);
    }

    std::string ByteFormat::si(const Size size) noexcept {
        const char * unit = nullptr;
        auto value = float(size);

        if (size >= SI_EXABYTE) {
            unit = "EB";
            value /= SI_EXABYTE;
        }
        else if (size >= SI_PETABYTE) {
            unit = "PB";
            value /= SI_PETABYTE;
        }
        else if (size >= SI_TERABYTE) {
            unit = "TB";
            value /= SI_TERABYTE;
        }
        else if (size >= SI_GIGABYTE) {
            unit = "GB";
            value /= SI_GIGABYTE;
        }
        else if (size >= SI_MEGABYTE) {
            unit = "MB";
            value /= SI_MEGABYTE;
        }
        else if (size >= SI_KILOBYTE) {
            unit = "kB";
            value /= SI_KILOBYTE;
        }
        else if (size >= SI_BYTE) {
            unit = "B";
        }
        else if (size == 0) {
            return "0 B";
        }

        return size < SI_KILOBYTE ? byteFormat("%.0f %s", value, unit) : byteFormat("%.1f %s", value, unit);
    }

    /**************************************************************************************************/
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**************************************************************************************************/

}
}
