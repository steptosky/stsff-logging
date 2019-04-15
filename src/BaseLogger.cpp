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

#include <iostream>
#include <mutex>
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
                                 {LvlDebug, LevelConfig("DBG: ", &std::cout, colorize::magenta, false, false)},
                                 {LvlMsg, LevelConfig("-- ", &std::cout, nullptr, false, false)},
                                 {LvlInfo, LevelConfig("INF: ", &std::cout, colorize::cyan, false, false)},
                                 {LvlSuccess, LevelConfig("INF: ", &std::cout, colorize::green, false, false)},
                                 {LvlWarning, LevelConfig("WRN: ", &std::cout, colorize::yellow, false, false)},
                                 {LvlError, LevelConfig("ERR: ", &std::cerr, colorize::red, true, true)},
                                 {LvlCritical, LevelConfig("CRL: ", &std::cerr, colorize::red, true, true)},
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

    void BaseLogger::defaultThreadSafeCallBack(const BaseLogger * logger, const LogMsg & logMsg) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        defaultCallBack(logger, logMsg);
    }

    void BaseLogger::defaultCallBack(const BaseLogger * logger, const LogMsg & logMsg) {
        const auto printMessage = [&](std::ostream & outStream) {
            if (logger->mPrintLogCategory && logMsg.mIsPrintCategory && !logger->mCategory.empty()) {
                outStream.write(logger->mCategory.data(), logger->mCategory.size()) << " ";
            }
            if (logMsg.mIsPrintCategory && !logMsg.mCategory.empty()) {
                outStream.write(logMsg.mCategory.data(), logMsg.mCategory.size()) << " ";
            }
            if (!logMsg.mMsg.empty()) {
                outStream.write(logMsg.mMsg.data(), logMsg.mMsg.size());
            }
        };

        const auto * levelConf = logger->levelConfig(logMsg.mLevel);
        if (!levelConf) {
            printMessage(std::cout);
            if (logMsg.mIsPrintEol) {
                std::cout << std::endl;
            }
            return;
        }

        std::ostream * out = &std::cout;
        if (levelConf->mStream) {
            out = levelConf->mStream;
        }

        if (logger->mTextColorizing && levelConf->mColor) {
            levelConf->mColor(*out);
        }

        if (logMsg.mIsPrintLabel) {
            *out << levelConf->mLabel;
        }
        printMessage(*out);
        if (logger->mTextColorizing) {
            *out << colorize::reset;
        }

        const bool availableToPrinting = (!logMsg.mFunction.empty() || !logMsg.mFile.empty()) &&
                                         (levelConf->mIsPrintFunction || levelConf->mIsPrintSource);

        if (availableToPrinting) {
            *out << " \n\t[";
            if (levelConf->mIsPrintFunction && !logMsg.mFunction.empty()) {
                out->write(logMsg.mFunction.data(), logMsg.mFunction.size());
            }
            if (levelConf->mIsPrintSource && !logMsg.mFile.empty()) {
                (*out << " -> ").write(logMsg.mFile.data(), logMsg.mFile.size());
                if (logMsg.mLine > 0) {
                    *out << "(" << logMsg.mLine << ")";
                }
            }
            *out << "]";
        }

        if (logMsg.mIsPrintEol) {
            *out << std::endl;
        }
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
