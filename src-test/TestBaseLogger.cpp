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

#include <sstream>
#include <algorithm>
#include <stsff/logging/BaseLogger.h>
#include <stsff/logging/utils/Colorize.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace stsff::logging;

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

class BaseLoggerCallback {
public:

    std::stringstream stream;

    void setupLevels(BaseLogger * logger) {
        assert(logger);
        for (auto & v : logger->levelConfigs()) {
            v.second.mStream = &stream;
        }
    }

    std::string result() const { return stream.str(); }
    void clear() { stream = std::stringstream(); }
};

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(BaseLogger, forrmating) {
    BaseLoggerCallback clbk;
    BaseLogger logger("[logger]");
    clbk.setupLevels(&logger);
    //---------------
    clbk.clear();
    LogMessage(logger).error() << "message" << LPush;
    ASSERT_STREQ("ERR: [logger] message\n", clbk.result().c_str());
    //---------------
    clbk.clear();
    LogMessage(logger, "[category]").error() << "message" << LPush;
    ASSERT_STREQ("ERR: [logger] [category] message\n", clbk.result().c_str());
    //---------------
    clbk.clear();
    LogMessage(logger, "function", "file", 5).error() << "message" << LPush;
    ASSERT_STREQ("ERR: [logger] message \n\t[function -> file(5)]\n", clbk.result().c_str());
    //---------------
    clbk.clear();
    LogMessage(logger, "[category]", "function", "file", 5).error() << "message" << LPush;
    ASSERT_STREQ("ERR: [logger] [category] message \n\t[function -> file(5)]\n", clbk.result().c_str());
}

TEST(BaseLogger, forrmating_no_log_category) {
    BaseLoggerCallback clbk;
    BaseLogger logger("[logger]");
    clbk.setupLevels(&logger);
    //---------------
    clbk.clear();
    LogMessage(logger, "[category]", "function", "file", 5).error() << "message" << LPush;
    ASSERT_STREQ("ERR: [logger] [category] message \n\t[function -> file(5)]\n", clbk.result().c_str());
    //---------------
    clbk.clear();
    logger.setLogCategoryPrint(false);
    LogMessage(logger, "[category]", "function", "file", 5).error() << "message" << LPush;
    ASSERT_STREQ("ERR: [category] message \n\t[function -> file(5)]\n", clbk.result().c_str());
}

TEST(BaseLogger, forrmating_level_default_labels) {
    BaseLoggerCallback clbk;
    BaseLogger logger;
    clbk.setupLevels(&logger);
    //---------------
    clbk.clear();
    LogMessage(logger).debug() << "message" << LPush;
    ASSERT_STREQ("DBG: message\n", clbk.result().c_str());
    //---------------
    clbk.clear();
    LogMessage(logger).message() << "message" << LPush;
    ASSERT_STREQ("-- message\n", clbk.result().c_str());
    //---------------
    clbk.clear();
    LogMessage(logger).info() << "message" << LPush;
    ASSERT_STREQ("INF: message\n", clbk.result().c_str());
    //---------------
    clbk.clear();
    LogMessage(logger).warning() << "message" << LPush;
    ASSERT_STREQ("WRN: message\n", clbk.result().c_str());
    //---------------
    clbk.clear();
    LogMessage(logger).error() << "message" << LPush;
    ASSERT_STREQ("ERR: message\n", clbk.result().c_str());
    //---------------
    clbk.clear();
    LogMessage(logger).critical() << "message" << LPush;
    ASSERT_STREQ("CRL: message\n", clbk.result().c_str());
    //---------------
}

TEST(BaseLogger, forrmating_custom_level) {
    BaseLoggerCallback clbk;
    BaseLogger logger;
    clbk.setupLevels(&logger);
    logger.setLevelConfig(BaseLogger::eLevel(5), BaseLogger::LevelConfig("5: ", &clbk.stream));
    //---------------
    LogMessage(logger).level(5) << "message" << LPush;
    ASSERT_STREQ("5: message\n", clbk.result().c_str());
    //---------------
}

TEST(BaseLogger, forrmating_unknown_level) {
    BaseLoggerCallback clbk;
    BaseLogger logger;
    clbk.setupLevels(&logger);
    auto * coutBuff = std::cout.rdbuf();
    std::cout.rdbuf(clbk.stream.rdbuf());
    try {
        LLevel(logger, 25) << "message" << LPush;
        std::cout.rdbuf(coutBuff);
    }
    catch (...) {
        std::cout.rdbuf(coutBuff);
        throw;
    }
    ASSERT_STREQ("message\n", clbk.result().c_str());
}

TEST(BaseLogger, forrmating_level) {
    BaseLoggerCallback clbk;
    BaseLogger logger;
    clbk.setupLevels(&logger);
    logger.setPrintLevel(BaseLogger::LvlWarning);
    //---------------
    clbk.clear();
    LogMessage(logger).info() << "message" << LPush;
    ASSERT_STREQ("", clbk.result().c_str());
    //---------------
    clbk.clear();
    LogMessage(logger).warning() << "message" << LPush;
    ASSERT_STREQ("WRN: message\n", clbk.result().c_str());
}

TEST(BaseLogger, forrmating_no_source) {
    BaseLoggerCallback clbk;
    BaseLogger logger({{BaseLogger::LvlMsg, BaseLogger::LevelConfig("-- ", nullptr, nullptr, true, false)}});
    clbk.setupLevels(&logger);
    //---------------
    LogMessage(logger, "[category]", "function", "file", 5).message() << "message" << LPush;
    ASSERT_STREQ("-- [category] message \n\t[function]\n", clbk.result().c_str());
}

TEST(BaseLogger, forrmating_no_function) {
    BaseLoggerCallback clbk;
    BaseLogger logger({{BaseLogger::LvlMsg, BaseLogger::LevelConfig("-- ", nullptr, nullptr, false, true)}});
    clbk.setupLevels(&logger);
    //---------------
    LogMessage(logger, "[category]", "function", "file", 5).message() << "message" << LPush;
    ASSERT_STREQ("-- [category] message \n\t[ -> file(5)]\n", clbk.result().c_str());
}

#ifdef NDEBUG // 'Only file name' is enabled in release mode
TEST(BaseLogger, forrmating_sources_only_filename) {
    BaseLoggerCallback clbk;
    BaseLogger logger;
    clbk.setupLevels(&logger);
    //---------------
    LMessage(logger) << "message" << LPush;
    auto result = clbk.result();
    const auto it = std::find_if(result.begin(), result.end(), [](const char v) {
        return v == '\\' || v == '/';
    });
    ASSERT_TRUE(it == result.end());
}
#endif

TEST(BaseLogger, colorize) {
    const BaseLogger logger;
    LDebug(logger) << "debug message" << LPush;
    LMessage(logger) << "just a message" << LPush;
    LInfo(logger) << "info message" << LPush;
    LSuccess(logger) << "info success message" << LPush;
    LWarning(logger) << "warning message" << LPush;
    LError(logger) << "error message" << LPush;
    LCritical(logger) << "critical message" << LPush;
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(BaseLogger, threadsafe_simple_test) {
    BaseLoggerCallback clbk;
    BaseLogger logger;
    clbk.setupLevels(&logger);
    logger.setCallBack(BaseLogger::defaultThreadSafeCallBack);
    //---------------
    LogMessage(logger).debug() << "message" << LPush;
    ASSERT_STREQ("DBG: message\n", clbk.result().c_str());
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(BaseLogger, copy_constructor_call) {
    BaseLoggerCallback clbk;
    BaseLogger logger("[logger]");
    clbk.setupLevels(&logger);
    //---------------
    clbk.clear();
    LogMessage(logger, "[category]", "function", "file", 5).error() << "message" << LPush;
    ASSERT_STREQ("ERR: [logger] [category] message \n\t[function -> file(5)]\n", clbk.result().c_str());
    //---------------
    clbk.clear();
    BaseLogger logger2(logger);
    logger2.setLogCategoryName("[logger2]");
    LogMessage(logger2, "[category]", "function", "file", 5).error() << "message" << LPush;
    ASSERT_STREQ("ERR: [logger2] [category] message \n\t[function -> file(5)]\n", clbk.result().c_str());
}

TEST(BaseLogger, copy_constructor_operator) {
    BaseLoggerCallback clbk;
    BaseLogger logger("[logger]");
    clbk.setupLevels(&logger);
    //---------------
    clbk.clear();
    LogMessage(logger, "[category]", "function", "file", 5).error() << "message" << LPush;
    ASSERT_STREQ("ERR: [logger] [category] message \n\t[function -> file(5)]\n", clbk.result().c_str());
    //---------------
    clbk.clear();
    BaseLogger logger2("[logger2]");
    logger2.setTextColorizing(false);
    logger2 = logger;
    logger2.setLogCategoryName("[logger2]");
    LogMessage(logger2, "[category]", "function", "file", 5).error() << "message" << LPush;
    ASSERT_STREQ("ERR: [logger2] [category] message \n\t[function -> file(5)]\n", clbk.result().c_str());
}

TEST(BaseLogger, move_constructor_call) {
    BaseLoggerCallback clbk;
    BaseLogger logger("[logger]");
    clbk.setupLevels(&logger);
    //---------------
    clbk.clear();
    LogMessage(logger, "[category]", "function", "file", 5).error() << "message" << LPush;
    ASSERT_STREQ("ERR: [logger] [category] message \n\t[function -> file(5)]\n", clbk.result().c_str());
    //---------------
    clbk.clear();
    BaseLogger logger2(std::move(logger));
    logger2.setLogCategoryName("[logger2]");
    LogMessage(logger2, "[category]", "function", "file", 5).error() << "message" << LPush;
    ASSERT_STREQ("ERR: [logger2] [category] message \n\t[function -> file(5)]\n", clbk.result().c_str());
}

TEST(BaseLogger, move_constructor_operator) {
    BaseLoggerCallback clbk;
    BaseLogger logger("[logger]");
    clbk.setupLevels(&logger);
    //---------------
    clbk.clear();
    LogMessage(logger, "[category]", "function", "file", 5).error() << "message" << LPush;
    ASSERT_STREQ("ERR: [logger] [category] message \n\t[function -> file(5)]\n", clbk.result().c_str());
    //---------------
    clbk.clear();
    BaseLogger logger2("[logger2]");
    logger2.setTextColorizing(false);
    logger2 = std::move(logger);
    logger2.setLogCategoryName("[logger2]");
    LogMessage(logger2, "[category]", "function", "file", 5).error() << "message" << LPush;
    ASSERT_STREQ("ERR: [logger2] [category] message \n\t[function -> file(5)]\n", clbk.result().c_str());
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(LogMessage, abort) {
    BaseLoggerCallback clbk;
    BaseLogger logger;
    clbk.setupLevels(&logger);
    //---------------
    clbk.clear();
    LogMessage msg(logger);
    msg.warning() << "message";
    msg.abort();
    msg.push();
    ASSERT_STREQ("", clbk.result().c_str());
}

TEST(LogMessage, forrmating_no_eol) {
    BaseLoggerCallback clbk;
    BaseLogger logger;
    clbk.setupLevels(&logger);
    //---------------
    LMessage(logger) << "message" << LPush;
    ASSERT_STREQ("-- message\n", clbk.result().c_str());
    //---------------
    clbk.clear();
    LMessage(logger).noEol() << "message";
    ASSERT_STREQ("-- message", clbk.result().c_str());
}

TEST(LogMessage, forrmating_no_category) {
    BaseLoggerCallback clbk;
    BaseLogger logger("[logger]");
    clbk.setupLevels(&logger);
    //---------------
    LCatMessage(logger,"[category]") << "message" << LPush;
    ASSERT_STREQ("-- [logger] [category] message\n", clbk.result().c_str());
    //---------------
    clbk.clear();
    LCatMessage(logger, "[category]").noCategory() << "message";
    ASSERT_STREQ("-- message\n", clbk.result().c_str());
}

TEST(LogMessage, forrmating_no_label) {
    BaseLoggerCallback clbk;
    BaseLogger logger;
    clbk.setupLevels(&logger);
    //---------------
    LCatMessage(logger, "[category]") << "message" << LPush;
    ASSERT_STREQ("-- [category] message\n", clbk.result().c_str());
    //---------------
    clbk.clear();
    LCatMessage(logger, "[category]").noLabel() << "message";
    ASSERT_STREQ("[category] message\n", clbk.result().c_str());
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
