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

#include <iosfwd>
#include "stsff/logging/Export.h"

namespace stsff {
namespace logging {
    namespace colorize {

        /**************************************************************************************************/
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /**************************************************************************************************/

        // Based on this code https://github.com/ikalnytskyi/termcolor

        /**************************************************************************************************/
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /**************************************************************************************************/
        // common

        LoggingExp std::ostream & grey(std::ostream & stream);

        LoggingExp std::ostream & red(std::ostream & stream);
        LoggingExp std::ostream & green(std::ostream & stream);
        LoggingExp std::ostream & blue(std::ostream & stream);

        LoggingExp std::ostream & yellow(std::ostream & stream);
        LoggingExp std::ostream & magenta(std::ostream & stream);
        LoggingExp std::ostream & cyan(std::ostream & stream);

        LoggingExp std::ostream & white(std::ostream & stream);

        //-------------------------------------------------------------------------

        LoggingExp std::ostream & onGrey(std::ostream & stream);

        LoggingExp std::ostream & onRed(std::ostream & stream);
        LoggingExp std::ostream & onGreen(std::ostream & stream);
        LoggingExp std::ostream & onBlue(std::ostream & stream);

        LoggingExp std::ostream & onYellow(std::ostream & stream);
        LoggingExp std::ostream & onMagenta(std::ostream & stream);
        LoggingExp std::ostream & onCyan(std::ostream & stream);

        LoggingExp std::ostream & onWhite(std::ostream & stream);

        /**************************************************************************************************/
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /**************************************************************************************************/
        // windows only in Unix it will be standard as above.

        LoggingExp std::ostream & darkGrey(std::ostream & stream);

        LoggingExp std::ostream & darkRed(std::ostream & stream);
        LoggingExp std::ostream & darkGreen(std::ostream & stream);
        LoggingExp std::ostream & darkBlue(std::ostream & stream);

        LoggingExp std::ostream & darkYellow(std::ostream & stream);
        LoggingExp std::ostream & darkMagenta(std::ostream & stream);
        LoggingExp std::ostream & darkCyan(std::ostream & stream);

        LoggingExp std::ostream & darkWhite(std::ostream & stream);

        //-------------------------------------------------------------------------

        LoggingExp std::ostream & onDarkGrey(std::ostream & stream);

        LoggingExp std::ostream & onDarkRed(std::ostream & stream);
        LoggingExp std::ostream & onDarkGreen(std::ostream & stream);
        LoggingExp std::ostream & onDarkBlue(std::ostream & stream);

        LoggingExp std::ostream & onDarkYellow(std::ostream & stream);
        LoggingExp std::ostream & onDarkMagenta(std::ostream & stream);
        LoggingExp std::ostream & onDarkCyan(std::ostream & stream);

        LoggingExp std::ostream & onDarkWhite(std::ostream & stream);

        /**************************************************************************************************/
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /**************************************************************************************************/

        LoggingExp std::ostream & reset(std::ostream & stream);

        /**************************************************************************************************/
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /**************************************************************************************************/

    }
}
}
