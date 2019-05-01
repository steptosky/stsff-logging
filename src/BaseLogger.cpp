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

#include <mutex>
#include <ctime>
#include <algorithm>
#include "stsff/logging/BaseLogger.h"
#include "stsff/logging/utils/Colorize.h"

namespace stsff {
namespace logging {

    /**************************************************************************************************/
    ////////////////////////////////////* Constructors/Destructor */////////////////////////////////////
    /**************************************************************************************************/

    BaseLogger::BaseLogger(const StringView category, const CallBack & callBack)
        : BaseLogger(category,
                         {
                                 {
                                     LvlDebug,
                                     LevelConfig({&std::cout}, "DBG: %LC %MC %MS", colorize::magenta)
                                 },
                                 {
                                     LvlMsg,
                                     LevelConfig({&std::cout}, "--  %LC %MC %MS", nullptr)
                                 },
                                 {
                                     LvlInfo,
                                     LevelConfig({&std::cout}, "INF: %LC %MC %MS", colorize::cyan)
                                 },
                                 {
                                     LvlSuccess,
                                     LevelConfig({&std::cout}, "INF: %LC %MC %MS | OK", colorize::green)
                                 },
                                 {
                                     LvlWarning,
                                     LevelConfig({&std::cout}, "WRN: %LC %MC %MS", colorize::yellow)
                                 },
                                 {
                                     LvlFail,
                                     LevelConfig({&std::cerr}, "ERR: %LC %MC [%TM(%Y-%m-%d %T)] %MS | FAIL\n\t[%FN -> %FI(%LI)]", colorize::red)
                                 },
                                 {
                                     LvlError,
                                     LevelConfig({&std::cerr}, "ERR: %LC %MC [%TM(%Y-%m-%d %T)] %MS \n\t[%FN -> %FI(%LI)]", colorize::red)
                                 },
                                 {
                                     LvlCritical,
                                     LevelConfig({&std::cerr}, "CRL: %LC %MC [%TM(%Y-%m-%d %T)] %MS \n\t[%FN -> %FI(%LI)]", colorize::red)
                                 },
                         },
                     callBack) { }

    /**************************************************************************************************/
    //////////////////////////////////////////* Functions */////////////////////////////////////////////
    /**************************************************************************************************/

    void BaseLogger::setLevelConfig(const std::size_t level, const LevelConfig & conf) {
        const auto it = mLevels.find(level);
        if (it != mLevels.end()) {
            it->second = conf;
        }
        else {
            mLevels.emplace(level, conf);
        }
    }

    const BaseLogger::LevelConfig * BaseLogger::levelConfig(const std::size_t level) const {
        const auto it = mLevels.find(level);
        return it != mLevels.end() ? &it->second : nullptr;
    }

    BaseLogger::LevelConfig * BaseLogger::levelConfig(const std::size_t level) {
        const auto it = mLevels.find(level);
        return it != mLevels.end() ? &it->second : nullptr;
    }

    /**************************************************************************************************/
    //////////////////////////////////////////* Functions */////////////////////////////////////////////
    /**************************************************************************************************/

    std::string BaseLogger::timeStamp(const std::string & format) {
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

    void BaseLogger::defaultThreadSafeCallBack(const BaseLogger * logger, const LogMsg & logMsg) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        defaultCallBack(logger, logMsg);
    }

    void BaseLogger::defaultCallBack(const BaseLogger * logger, const LogMsg & logMsg) {
        static const LevelConfig defaultLevel({&std::cout}, "UNSPECIFIED LVL CONF: %LC %MC %MS \n\t[%FN -> %FI(%LI)]", colorize::yellow);

        const std::uint32_t time = ('T' << 8) | 'M';
        const std::uint32_t logCategory = ('L' << 8) | 'C';
        const std::uint32_t messageCategory = ('M' << 8) | 'C';
        const std::uint32_t message = ('M' << 8) | 'S';
        const std::uint32_t functionName = ('F' << 8) | 'N';
        const std::uint32_t fileName = ('F' << 8) | 'I';
        const std::uint32_t fileLineNum = ('L' << 8) | 'I';

        auto * levelConf = logger->levelConfig(logMsg.mLevel);
        if (!levelConf) {
            levelConf = &defaultLevel;
            for (auto & s : levelConf->mStreams) {
                *s << " level configuration isn't specified for the level: [" << logMsg.mLevel << "]" << std::endl;
            }
        }

#ifndef NDEBUG
        for (auto & s : levelConf->mStreams) {
            if (!s) {
                std::cerr << colorize::red
                        << " nullptr stream in the level configuration: [" << logMsg.mLevel << "]"
                        << colorize::reset << std::endl;
                assert(s);
            }
        }
#endif

        auto * streams = &levelConf->mStreams;
        if (logger->mTextColorizing && levelConf->mColor) {
            for (auto & s : *streams) { levelConf->mColor(*s); }
        }

        bool process = false;
        auto ch = levelConf->mFormatting.begin();

        while (ch != levelConf->mFormatting.end()) {
            if (*ch == '%') {
                process = true;
                ++ch;
                continue;
            }
            if (!process) {
                for (auto & s : *streams) { *s << *ch; }
                ++ch;
                continue;
            }
            process = false;
            const auto second = ch + 1;
            if (second == levelConf->mFormatting.end()) {
                for (auto & s : *streams) {
                    *s << colorize::red
                            << " unexpected end of formatting string after ["
                            << *ch
                            << "], expected the second command letter"
                            << colorize::reset;
                }
                break;
            }

            const std::uint32_t command = (*ch << 8) | *second;
            ch = second + 1;

            switch (command) {
                case logCategory: {
                    if (!logger->mCategory.empty()) {
                        for (auto & s : *streams) { s->write(logger->mCategory.data(), logger->mCategory.size()); }
                    }
                    break;
                }
                case messageCategory: {
                    if (!logMsg.mCategory.empty()) {
                        for (auto & s : *streams) { s->write(logMsg.mCategory.data(), logMsg.mCategory.size()); }
                    }
                    break;
                }
                case message: {
                    if (!logMsg.mMsg.empty()) {
                        for (auto & s : *streams) { s->write(logMsg.mMsg.data(), logMsg.mMsg.size()); }
                    }
                    break;
                }
                case functionName: {
                    for (auto & s : *streams) { s->write(logMsg.mFunction.data(), logMsg.mFunction.size()); }
                    break;
                }
                case fileName: {
                    for (auto & s : *streams) { s->write(logMsg.mFile.data(), logMsg.mFile.size()); }
                    break;
                }
                case fileLineNum: {
                    for (auto & s : *streams) { *s << logMsg.mLine; }
                    break;
                }
                case time: {
                    if (ch == levelConf->mFormatting.end()) {
                        for (auto & s : *streams) {
                            *s << colorize::red
                                    << " unexpected end of formatting string after the time command, expected '()'"
                                    << colorize::reset;
                        }
                        break;
                    }
                    if (*ch != '(') {
                        for (auto & s : *streams) {
                            *s << colorize::red
                                    << " unexpected symbol " << *ch << " after time command, expected '('"
                                    << colorize::reset;
                        }
                        ++ch;
                        break;
                    }
                    ++ch; // skip '('
                    std::size_t length = 0;
                    auto endOfTimeFormat = std::find_if(ch, levelConf->mFormatting.end(), [&](const char c) {
                        ++length;
                        return c == ')';
                    });
                    if (endOfTimeFormat == levelConf->mFormatting.end()) {
                        for (auto & s : *streams) {
                            *s << colorize::red
                                    << " unexpected end of formatting string after the time command, missed ')'"
                                    << colorize::reset;
                        }
                        break;
                    }
                    const std::string formattingTimeString(&*ch, length == 0 ? 0 : length - 1); // remove last ')' char
                    ch = endOfTimeFormat;
                    const auto t = timeStamp(formattingTimeString);
                    for (auto & s : *streams) { *s << t; }
                    ++ch;
                    break;
                }
                default: {
                    for (auto & s : *streams) {
                        *s << colorize::red
                                << " unknown formatting command: [" << char(command >> 8) << char(command) << "]"
                                << colorize::reset;
                    }
                }
            }
        }
        if (logger->mTextColorizing) {
            for (auto & s : *streams) { *s << colorize::reset; }
        }
        for (auto & s : *streams) { *s << std::endl; }
    }

    /**************************************************************************************************/
    //////////////////////////////////////////* Functions */////////////////////////////////////////////
    /**************************************************************************************************/

    void LogMessage::push() {
        if (!mPushed) {
            const auto str = mStream.str();
            if (!str.empty()) {
                mLogMsg.mMsg = BaseLogger::StringView(str.data(), str.length());
                mLog->log(mLogMsg);
            }
            mPushed = true;
        }
    }

    /**************************************************************************************************/
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**************************************************************************************************/

}
}
