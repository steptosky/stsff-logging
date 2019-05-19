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

#include <ctime>
#include <algorithm>
#include "stsff/logging/BaseLogger.h"
#include "stsff/logging/utils/Colorize.h"

namespace stsff {
namespace logging {

    /**************************************************************************************************/
    ////////////////////////////////////* Constructors/Destructor */////////////////////////////////////
    /**************************************************************************************************/

    BaseLogger::BaseLogger(const StringView category)
        : BaseLogger(category, {
                {
                    LvlDebug, [](const BaseLogger & l, const LogMsg & m) {
                        defaultHandler(l, m, std::clog, "DBG: %LC %MC %MS", colorize::magenta);
                    }
                },
                {
                    LvlMsg, [](const BaseLogger & l, const LogMsg & m) {
                        defaultHandler(l, m, std::clog, "--  %LC %MC %MS", nullptr);
                    }

                },
                {
                    LvlInfo, [](const BaseLogger & l, const LogMsg & m) {
                        defaultHandler(l, m, std::clog, "INF: %LC %MC %MS", colorize::cyan);
                    }
                },
                {
                    LvlSuccess, [](const BaseLogger & l, const LogMsg & m) {
                        defaultHandler(l, m, std::clog, "INF: %LC %MC %MS | OK", colorize::green);
                    }
                },
                {
                    LvlWarning, [](const BaseLogger & l, const LogMsg & m) {
                        defaultHandler(l, m, std::clog, "WRN: %LC %MC %MS", colorize::yellow);
                    }
                },
                {
                    LvlFail, [](const BaseLogger & l, const LogMsg & m) {
                        defaultHandler(l, m, std::clog, "ERR: %LC %MC %MS | FAIL\n\t[%TM(%Y-%m-%d] [%T)] [%FN -> %FI(%LI)]", colorize::red);
                    }
                },
                {
                    LvlError, [](const BaseLogger & l, const LogMsg & m) {
                        defaultHandler(l, m, std::cerr, "ERR: %LC %MC %MS \n\t[%TM(%Y-%m-%d] [%T)] [%FN -> %FI(%LI)]", colorize::red);
                    }
                },
                {
                    LvlCritical, [](const BaseLogger & l, const LogMsg & m) {
                        defaultHandler(l, m, std::cerr, "ERR: %LC %MC %MS \n\t[%TM(%Y-%m-%d] [%T)] [%FN -> %FI(%LI)]", colorize::red);
                    }
                },
        }) { }

    BaseLogger::~BaseLogger() noexcept {
        try {
            std::clog.flush();
        }
        catch (const std::exception & e) {
            std::cerr << colorize::red << e.what() << " [" << __STS_FUNC_NAME__ << "]" << colorize::reset << std::endl;
        }
        catch (...) {
            std::cerr << colorize::red << "unknown exception [" << __STS_FUNC_NAME__ << "]" << colorize::reset << std::endl;
        }
    }

    /**************************************************************************************************/
    //////////////////////////////////////////* Functions */////////////////////////////////////////////
    /**************************************************************************************************/

    void BaseLogger::log(const LogMsg & logMsg) const {
        if (logMsg.mLevel <= mLevel) {
            const auto it = mLevels.find(logMsg.mLevel);
            if (it != mLevels.end()) {
                it->second(*this, logMsg);
            }
            else {
                const auto formatting = std::string("LVL(")
                                        .append(std::to_string(logMsg.mLevel))
                                        .append("): %LC %MC %MS \n\t[%FN -> %FI(%LI)]");
                defaultHandler(*this, logMsg, std::cout, formatting, colorize::yellow);
            }
        }
    }

    /**************************************************************************************************/
    //////////////////////////////////////////* Functions */////////////////////////////////////////////
    /**************************************************************************************************/

    void BaseLogger::setHandler(const std::size_t level, const LevelHandler & handler) noexcept {
        const auto it = mLevels.find(level);
        if (it != mLevels.end()) {
            it->second = handler;
        }
        else {
            mLevels.emplace(level, handler);
        }
    }

    /**************************************************************************************************/
    //////////////////////////////////////////* Functions */////////////////////////////////////////////
    /**************************************************************************************************/

    void BaseLogger::defaultHandler(const BaseLogger & logger, const LogMsg & logMsg, std::ostream & stream,
                                    const std::string & formatting, const ColorFn color) {

        const std::uint32_t time = ('T' << 8) | 'M';
        const std::uint32_t logCategory = ('L' << 8) | 'C';
        const std::uint32_t messageCategory = ('M' << 8) | 'C';
        const std::uint32_t message = ('M' << 8) | 'S';
        const std::uint32_t functionName = ('F' << 8) | 'N';
        const std::uint32_t fileName = ('F' << 8) | 'I';
        const std::uint32_t fileLineNum = ('L' << 8) | 'I';

        if (color) {
            color(stream);
        }

        bool process = false;
        auto ch = formatting.begin();

        while (ch != formatting.end()) {
            if (*ch == '%') {
                process = true;
                ++ch;
                continue;
            }
            if (!process) {
                stream << *ch;
                ++ch;
                continue;
            }
            process = false;
            const auto second = ch + 1;
            if (second == formatting.end()) {
                stream << colorize::red
                        << " unexpected end of formatting string after ["
                        << *ch
                        << "], expected the second command letter"
                        << colorize::reset;
                break;
            }

            const std::uint32_t command = (*ch << 8) | *second;
            ch = second + 1;

            switch (command) {
                case logCategory: {
                    if (!logger.mCategory.empty()) {
                        stream.write(logger.mCategory.data(), logger.mCategory.size());
                    }
                    break;
                }
                case messageCategory: {
                    if (!logMsg.mCategory.empty()) {
                        stream.write(logMsg.mCategory.data(), logMsg.mCategory.size());
                    }
                    break;
                }
                case message: {
                    if (!logMsg.mMsg.empty()) {
                        stream.write(logMsg.mMsg.data(), logMsg.mMsg.size());
                    }
                    break;
                }
                case functionName: {
                    stream.write(logMsg.mFunction.data(), logMsg.mFunction.size());
                    break;
                }
                case fileName: {
                    stream.write(logMsg.mFile.data(), logMsg.mFile.size());
                    break;
                }
                case fileLineNum: {
                    stream << logMsg.mLine;
                    break;
                }
                case time: {
                    if (ch == formatting.end()) {
                        stream << colorize::red
                                << " unexpected end of formatting string after the time command, expected '()'"
                                << colorize::reset;
                        break;
                    }
                    if (*ch != '(') {
                        stream << colorize::red
                                << " unexpected symbol " << *ch << " after time command, expected '('"
                                << colorize::reset;
                        ++ch;
                        break;
                    }
                    ++ch; // skip '('
                    std::size_t length = 0;
                    auto endOfTimeFormat = std::find_if(ch, formatting.end(), [&](const char c) {
                        ++length;
                        return c == ')';
                    });
                    if (endOfTimeFormat == formatting.end()) {
                        stream << colorize::red
                                << " unexpected end of formatting string after the time command, missed ')'"
                                << colorize::reset;
                        break;
                    }
                    const std::string formattingTimeString(&*ch, length == 0 ? 0 : length - 1); // remove last ')' char
                    ch = endOfTimeFormat;
                    const auto t = timeStamp(formattingTimeString);
                    stream << t;
                    ++ch;
                    break;
                }
                default: {
                    stream << colorize::red
                            << " unknown formatting command: [" << char(command >> 8) << char(command) << "]"
                            << colorize::reset;
                }
            }
        }
        if (color) {
            stream << colorize::reset;
        }
        stream << std::endl;
    }

    std::string BaseLogger::timeStamp(const std::string & format) noexcept {
        std::string out;
        if (format.empty()) {
            return out;
        }
        //-------------------
        const std::size_t timeBuffSize = 100;
        char buffer[100] = {0};
        assert(format.size() < timeBuffSize - 1);
        //-------------------
        time_t time = std::time(nullptr);
        tm timeInfo = {};
#ifdef _MSC_VER
        localtime_s(&timeInfo, &time);
#else
		timeInfo = *localtime(&time);
#endif
        const auto byteNum = std::strftime(buffer, sizeof buffer, format.c_str(), &timeInfo);
        if (byteNum > 0) {
            out.append(buffer, byteNum);
        }
        return out;
    }

    /**************************************************************************************************/
    //////////////////////////////////////////* Functions */////////////////////////////////////////////
    /**************************************************************************************************/

    void LogMessage::push() noexcept {
        if (mPushed || !mLog || mLogMsg.mLevel > mLog->level()) {
            return;
        }
        mPushed = true;
        try {
            const auto str = string();
            if (!str.empty()) {
                mLogMsg.mMsg = BaseLogger::StringView(str.data(), str.length());
                mLog->log(mLogMsg);
            }
        }
        catch (const std::exception & e) {
            std::cerr << colorize::red << e.what() << " [" << __STS_FUNC_NAME__ << "]" << colorize::reset << std::endl;
        }
        catch (...) {
            std::cerr << colorize::red << "unknown exception [" << __STS_FUNC_NAME__ << "]" << colorize::reset << std::endl;
        }
    }

    /**************************************************************************************************/
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**************************************************************************************************/

}
}
