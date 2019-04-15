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

#include "ph/stdafx.h"

#include "gtest/gtest.h"
#include <stsff/logging/utils/Colorize.h>

using namespace stsff::logging;

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(Colorize, reset) {
    std::cout << colorize::cyan << "cyan -" << colorize::reset << "- default" << std::endl;
    std::cout << colorize::reset;
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(Colorize, foreground) {
    std::cout << colorize::grey << "grey text" << std::endl;

    std::cout << colorize::red << "red text" << std::endl;
    std::cout << colorize::green << "green text" << std::endl;
    std::cout << colorize::blue << "blue text" << std::endl;

    std::cout << colorize::yellow << "yellow text" << std::endl;
    std::cout << colorize::magenta << "magenta text" << std::endl;
    std::cout << colorize::cyan << "cyan text" << std::endl;

    std::cout << colorize::white << "white text" << std::endl;

    std::cout << colorize::reset;
}

TEST(Colorize, foreground_dark) {
    std::cout << colorize::darkGrey << colorize::onWhite << "grey dark text" << colorize::reset << std::endl;

    std::cout << colorize::darkRed << "red dark text" << std::endl;
    std::cout << colorize::darkGreen << "green dark text" << std::endl;
    std::cout << colorize::darkBlue << "blue dark text" << std::endl;

    std::cout << colorize::darkYellow << "yellow dark text" << std::endl;
    std::cout << colorize::darkMagenta << "magenta dark text" << std::endl;
    std::cout << colorize::darkCyan << "cyan dark text" << std::endl;

    std::cout << colorize::reset;
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(Colorize, background) {
    std::cout << colorize::onGrey << colorize::darkGrey << "grey background" << colorize::reset << std::endl;

    std::cout << colorize::onRed << colorize::darkGrey << "red background" << colorize::reset << std::endl;
    std::cout << colorize::onGreen << colorize::darkGrey << "green background" << colorize::reset << std::endl;
    std::cout << colorize::onBlue << colorize::white << "blue background" << colorize::reset << std::endl;

    std::cout << colorize::onYellow << colorize::darkGrey << "yellow background" << colorize::reset << std::endl;
    std::cout << colorize::onMagenta << colorize::darkGrey << "magenta background" << colorize::reset << std::endl;
    std::cout << colorize::onCyan << colorize::darkGrey << "cyan background" << colorize::reset << std::endl;

    std::cout << colorize::onWhite << colorize::darkGrey << "white background" << colorize::reset << std::endl;
}

TEST(Colorize, background_dark) {
    std::cout << colorize::onDarkGrey << "grey dark b background" << colorize::reset << std::endl;

    std::cout << colorize::onDarkRed << "red dark background" << colorize::reset << std::endl;
    std::cout << colorize::onDarkGreen << "green dark background" << colorize::reset << std::endl;
    std::cout << colorize::onDarkBlue << colorize::white << "blue dark background" << colorize::reset << std::endl;

    std::cout << colorize::onDarkYellow << "yellow dark background" << colorize::reset << std::endl;
    std::cout << colorize::onDarkMagenta << "magenta dark background" << colorize::reset << std::endl;
    std::cout << colorize::onDarkCyan << "cyan dark background" << colorize::reset << std::endl;

    std::cout << colorize::onDarkWhite << colorize::darkGrey << "white background" << colorize::reset << std::endl;
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
