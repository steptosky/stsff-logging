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

#include <ostream>
#include "stsff/logging/utils/Colorize.h"

#if defined(_WIN32) || defined(_WIN64)
#   define COLORIZE_OS_WINDOWS
#elif defined(__APPLE__)
#   define COLORIZE_OS_MACOS
#elif defined(__unix__) || defined(__unix)
#   define COLORIZE_OS_LINUX
#else
#   error unsupported platform
#endif

#ifdef COLORIZE_OS_WINDOWS
#   define COLORIZE_ON_WINDOWS(X) X
#else
#   define COLORIZE_ON_WINDOWS(X)
#endif

#ifdef COLORIZE_OS_MACOS
#   define COLORIZE_ON_MACOS(X) X
#else
#   define COLORIZE_ON_MACOS(X)
#endif

#ifdef COLORIZE_OS_LINUX
#   define COLORIZE_ON_LINUX(X) X
#else
#   define COLORIZE_ON_LINUX(X)
#endif

#if defined(COLORIZE_OS_MACOS) || defined(COLORIZE_OS_LINUX)
#   include <unistd.h>
#elif defined(COLORIZE_OS_WINDOWS)
#   include <io.h>
#   include <windows.h>
#endif

#include <iostream>
#include <cstdio>

namespace stsff {
namespace logging {
    namespace colorize {

        /**************************************************************************************************/
        /////////////////////////////////////////* Static area *////////////////////////////////////////////
        /**************************************************************************************************/

        // An index to be used to access a private storage of I/O streams. See
        // colorize / disabled I/O manipulators for details.
        static const auto gColorizeIndex = std::ios_base::xalloc();

        inline FILE * getStdStream(const std::ostream & stream) {
            if (&stream == &std::cout) {
                return stdout;
            }
            if (&stream == &std::cerr) {
                return stderr;
            }
            if (&stream == &std::clog) {
                return stdout;
            }
            return nullptr;
        }

        //! Test whether a given `std::ostream` object refers to
        //! a terminal.
        inline bool isAtty(const std::ostream & stream) {
            auto * stdStream = getStdStream(stream);

            // Unfortunately, fileno() ends with segmentation fault
            // if invalid file descriptor is passed. So we need to
            // handle this case gracefully and assume it's not a tty
            // if standard stream is not detected, and 0 is returned.
            if (!stdStream) {
                return false;
            }
            COLORIZE_ON_MACOS(return static_cast<bool>(::isatty(fileno(stdStream))));
            COLORIZE_ON_LINUX(return static_cast<bool>(::isatty(fileno(stdStream))));
            COLORIZE_ON_WINDOWS(return ::_isatty(_fileno(stdStream)) != 0;);
        }

        // Say whether a given stream should be colorized or not. It's always
        // true for ATTY streams and may be true for streams marked with
        // colorize flag.
        inline bool isColorized(std::ostream & stream) {
            return isAtty(stream) || stream.iword(gColorizeIndex) != 0;
        }

        /**************************************************************************************************/
        //////////////////////////////////////////* Functions */////////////////////////////////////////////
        /**************************************************************************************************/

#if defined(COLORIZE_OS_WINDOWS)
        //! Change Windows Terminal colors attribute. If some
        //! parameter is `-1` then attribute won't changed.
        inline void winChangeAttributes(std::ostream & stream, const int foreground, const int background = -1) {
            // yeah, i know.. it's ugly, it's windows.
            static WORD defaultAttributes = 0;

            // Windows doesn't have ANSI escape sequences and so we use special
            // API to change Terminal output color. That means we can't
            // manipulate colors by means of "std::stringstream" and hence
            // should do nothing in this case.
            if (!isAtty(stream)) {
                return;
            }
            // get terminal handle
            HANDLE hTerminal = INVALID_HANDLE_VALUE;
            if (&stream == &std::cout) {
                hTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
            }
            else if (&stream == &std::cerr) {
                hTerminal = GetStdHandle(STD_ERROR_HANDLE);
            }
            else if (&stream == &std::clog) {
                hTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
            }

            // save default terminal attributes if it unsaved
            if (!defaultAttributes) {
                CONSOLE_SCREEN_BUFFER_INFO info;
                if (!GetConsoleScreenBufferInfo(hTerminal, &info)) {
                    return;
                }
                defaultAttributes = info.wAttributes;
            }

            // restore all default settings
            if (foreground == -1 && background == -1) {
                SetConsoleTextAttribute(hTerminal, defaultAttributes);
                return;
            }

            // get current settings
            CONSOLE_SCREEN_BUFFER_INFO info;
            if (!GetConsoleScreenBufferInfo(hTerminal, &info)) {
                return;
            }

            if (foreground != -1) {
                info.wAttributes &= ~(info.wAttributes & 0x0F);
                info.wAttributes |= static_cast<WORD>(foreground);
            }

            if (background != -1) {
                info.wAttributes &= ~(info.wAttributes & 0xF0);
                info.wAttributes |= static_cast<WORD>(background);
            }

            SetConsoleTextAttribute(hTerminal, info.wAttributes);
        }
#endif

        /**************************************************************************************************/
        //////////////////////////////////////////* Functions */////////////////////////////////////////////
        /**************************************************************************************************/

        std::ostream & grey(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[30m");
                COLORIZE_ON_LINUX(stream << "\033[30m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, 0 | FOREGROUND_INTENSITY));
            }
            return stream;
        }

        std::ostream & darkGrey(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[30m");
                COLORIZE_ON_LINUX(stream << "\033[30m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, 0));
            }
            return stream;
        }

        //-------------------------------------------------------------------------

        std::ostream & red(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[31m");
                COLORIZE_ON_LINUX(stream << "\033[31m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, FOREGROUND_RED | FOREGROUND_INTENSITY));
            }
            return stream;
        }

        std::ostream & darkRed(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[31m");
                COLORIZE_ON_LINUX(stream << "\033[31m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, FOREGROUND_RED));
            }
            return stream;
        }

        std::ostream & green(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream <<"\033[32m");
                COLORIZE_ON_LINUX(stream <<"\033[32m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, FOREGROUND_GREEN | FOREGROUND_INTENSITY));
            }
            return stream;
        }

        std::ostream & darkGreen(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[32m");
                COLORIZE_ON_LINUX(stream << "\033[32m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, FOREGROUND_GREEN));
            }
            return stream;
        }

        std::ostream & blue(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[34m");
                COLORIZE_ON_LINUX(stream << "\033[34m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, FOREGROUND_BLUE | FOREGROUND_INTENSITY));
            }
            return stream;
        }

        std::ostream & darkBlue(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[34m");
                COLORIZE_ON_LINUX(stream << "\033[34m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, FOREGROUND_BLUE));
            }
            return stream;
        }

        //-------------------------------------------------------------------------

        std::ostream & yellow(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream <<"\033[33m");
                COLORIZE_ON_LINUX(stream <<"\033[33m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY));
            }
            return stream;
        }

        std::ostream & darkYellow(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[33m");
                COLORIZE_ON_LINUX(stream << "\033[33m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, FOREGROUND_RED | FOREGROUND_GREEN));
            }
            return stream;
        }

        std::ostream & magenta(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[35m");
                COLORIZE_ON_LINUX(stream << "\033[35m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY));
            }
            return stream;
        }

        std::ostream & darkMagenta(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[35m");
                COLORIZE_ON_LINUX(stream << "\033[35m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, FOREGROUND_RED | FOREGROUND_BLUE));
            }
            return stream;
        }

        std::ostream & cyan(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[36m");
                COLORIZE_ON_LINUX(stream << "\033[36m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY));
            }
            return stream;
        }

        std::ostream & darkCyan(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[36m");
                COLORIZE_ON_LINUX(stream << "\033[36m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, FOREGROUND_GREEN | FOREGROUND_BLUE));
            }
            return stream;
        }

        //-------------------------------------------------------------------------

        std::ostream & white(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[37m");
                COLORIZE_ON_LINUX(stream << "\033[37m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE));
            }
            return stream;
        }

        /**************************************************************************************************/
        //////////////////////////////////////////* Functions */////////////////////////////////////////////
        /**************************************************************************************************/

        std::ostream & onGrey(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[40m");
                COLORIZE_ON_LINUX(stream << "\033[40m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, -1, 0 | BACKGROUND_INTENSITY));
            }
            return stream;
        }

        std::ostream & onDarkGrey(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[40m");
                COLORIZE_ON_LINUX(stream << "\033[40m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, -1, 0));
            }
            return stream;
        }

        //-------------------------------------------------------------------------

        std::ostream & onRed(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[41m");
                COLORIZE_ON_LINUX(stream << "\033[41m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, -1, BACKGROUND_RED | BACKGROUND_INTENSITY));
            }
            return stream;
        }

        std::ostream & onDarkRed(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[41m");
                COLORIZE_ON_LINUX(stream << "\033[41m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, -1, BACKGROUND_RED));
            }
            return stream;
        }

        std::ostream & onGreen(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[42m");
                COLORIZE_ON_LINUX(stream << "\033[42m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, -1, BACKGROUND_GREEN | BACKGROUND_INTENSITY));
            }
            return stream;
        }

        std::ostream & onDarkGreen(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[42m");
                COLORIZE_ON_LINUX(stream << "\033[42m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, -1, BACKGROUND_GREEN));
            }
            return stream;
        }

        std::ostream & onBlue(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[44m");
                COLORIZE_ON_LINUX(stream << "\033[44m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, -1, BACKGROUND_BLUE | BACKGROUND_INTENSITY));
            }
            return stream;
        }

        std::ostream & onDarkBlue(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[44m");
                COLORIZE_ON_LINUX(stream << "\033[44m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, -1, BACKGROUND_BLUE));
            }
            return stream;
        }

        //-------------------------------------------------------------------------

        std::ostream & onYellow(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[43m");
                COLORIZE_ON_LINUX(stream << "\033[43m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, -1, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY));
            }
            return stream;
        }

        std::ostream & onDarkYellow(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[43m");
                COLORIZE_ON_LINUX(stream << "\033[43m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, -1, BACKGROUND_RED | BACKGROUND_GREEN));
            }
            return stream;
        }

        std::ostream & onMagenta(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[45m");
                COLORIZE_ON_LINUX(stream << "\033[45m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, -1, BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY));
            }
            return stream;
        }

        std::ostream & onDarkMagenta(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[45m");
                COLORIZE_ON_LINUX(stream << "\033[45m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, -1, BACKGROUND_BLUE | BACKGROUND_RED));
            }
            return stream;
        }

        std::ostream & onCyan(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[46m");
                COLORIZE_ON_LINUX(stream << "\033[46m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, -1, BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY));
            }
            return stream;
        }

        std::ostream & onDarkCyan(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[46m");
                COLORIZE_ON_LINUX(stream << "\033[46m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, -1, BACKGROUND_GREEN | BACKGROUND_BLUE));
            }
            return stream;
        }

        //-------------------------------------------------------------------------

        std::ostream & onWhite(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[47m");
                COLORIZE_ON_LINUX(stream << "\033[47m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream,-1, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY));
            }
            return stream;
        }

        std::ostream & onDarkWhite(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[47m");
                COLORIZE_ON_LINUX(stream << "\033[47m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, -1, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE));
            }
            return stream;
        }

        /**************************************************************************************************/
        //////////////////////////////////////////* Functions */////////////////////////////////////////////
        /**************************************************************************************************/

        std::ostream & reset(std::ostream & stream) {
            if (isColorized(stream)) {
                COLORIZE_ON_MACOS(stream << "\033[00m");
                COLORIZE_ON_LINUX(stream << "\033[00m");
                COLORIZE_ON_WINDOWS(winChangeAttributes(stream, -1, -1));
            }
            return stream;
        }

        /**************************************************************************************************/
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /**************************************************************************************************/

    }
}
}
