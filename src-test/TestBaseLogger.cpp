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
#include <stsff/logging/BaseLogger.h>
#include <stsff/logging/utils/Colorize.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

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
            v.second.mStreams = {&stream};
        }
    }

    std::string result() const { return stream.str(); }
    void clear() { stream = std::stringstream(); }
};

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(BaseLogger, formatting) {
    BaseLoggerCallback callback;
    BaseLogger logger("log-category");
    callback.setupLevels(&logger);
    logger.levelConfig(BaseLogger::LvlMsg)->mFormatting = "%LC,%MS,%FN,%FI,%LI";
    //---------------
    callback.clear();
    LogMessage(logger).message() << "message" << LPush;
    EXPECT_STREQ("log-category,message,,,0\n", callback.result().c_str());
    //---------------
    callback.clear();
    LogMessage(logger, "function", "file", 5).message() << "message" << LPush;
    EXPECT_STREQ("log-category,message,function,file,5\n", callback.result().c_str());
}

TEST(BaseLogger, formatting_custom_level) {
    BaseLoggerCallback callback;
    BaseLogger logger;
    callback.setupLevels(&logger);
    logger.setLevelConfig(BaseLogger::eLevel(5), BaseLogger::LevelConfig({&callback.stream}, "5,%LC,%MS,%FN,%FI,%LI"));
    //---------------
    LogMessage(logger).level(5) << "message" << LPush;
    EXPECT_STREQ("5,,message,,,0\n", callback.result().c_str());
    //---------------
}

TEST(BaseLogger, formatting_unknown_level) {
    BaseLoggerCallback callback;
    BaseLogger logger;
    callback.setupLevels(&logger);
    auto * coutBuff = std::cout.rdbuf();
    std::cout.rdbuf(callback.stream.rdbuf());
    try {
        LogMessage(logger).level(25) << "message" << LPush;
        std::cout.rdbuf(coutBuff);
    }
    catch (...) {
        std::cout.rdbuf(coutBuff);
        throw;
    }
    ASSERT_STREQ(" level configuration isn't specified for the level: [25]\n\
UNSPECIFIED LVL CONF:  message \n\t[ -> (0)]\n", callback.result().c_str());
}

TEST(BaseLogger, formatting_level) {
    BaseLoggerCallback callback;
    BaseLogger logger;
    callback.setupLevels(&logger);
    logger.setPrintLevel(BaseLogger::LvlWarning);
    //---------------
    callback.clear();
    LogMessage(logger).info() << "message" << LPush;
    ASSERT_STREQ("", callback.result().c_str());
    //---------------
    callback.clear();
    LogMessage(logger).warning() << "message" << LPush;
    ASSERT_STREQ("WRN:  message\n", callback.result().c_str());
}

#ifdef NDEBUG // 'Only file name' is enabled in release mode
TEST(BaseLogger, formatting_sources_only_filename) {
    BaseLoggerCallback callback;
    BaseLogger logger;
    callback.setupLevels(&logger);
    //---------------
    LMessage(logger) << "message" << LPush;
    auto result = callback.result();
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

TEST(BaseLogger, time_stamp) {
    BaseLoggerCallback callback;
    BaseLogger logger;
    callback.setupLevels(&logger);
    logger.setLevelConfig(BaseLogger::LvlMsg, BaseLogger::LevelConfig({&callback.stream}, "%TM(%Y)"));
    //---------------
    LogMessage(logger).message() << "message" << LPush;
    EXPECT_STREQ(BaseLogger::timeStamp("%Y\n").c_str(), callback.result().c_str());
    //---------------
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

#ifndef NDEBUG
TEST(BaseLogger, invalid_callback_data_null_stream) {
    BaseLoggerCallback callback;
    BaseLogger logger;
    callback.setupLevels(&logger);
    logger.setLevelConfig(BaseLogger::LvlMsg, BaseLogger::LevelConfig({nullptr}, "%TM(%Y)"));
    //---------------
    ASSERT_DEBUG_DEATH(LogMessage(logger).message() << "message" << LPush, "");
    //---------------
}
#endif

TEST(BaseLogger, invalid_callback_data_command_case1) {
    BaseLoggerCallback callback;
    BaseLogger logger;
    callback.setupLevels(&logger);
    logger.setLevelConfig(BaseLogger::LvlMsg, BaseLogger::LevelConfig({&callback.stream}, "%TG"));

    auto * coutBuff = std::cout.rdbuf();
    std::cout.rdbuf(callback.stream.rdbuf());
    try {
        LogMessage(logger).message() << "message" << LPush;
        std::cout.rdbuf(coutBuff);
    }
    catch (...) {
        std::cout.rdbuf(coutBuff);
        throw;
    }
    //---------------
    ASSERT_STREQ(" unknown formatting command: [TG]\n", callback.result().c_str());
    //---------------
}

TEST(BaseLogger, invalid_callback_data_command_case2) {
    BaseLoggerCallback callback;
    BaseLogger logger;
    callback.setupLevels(&logger);
    logger.setLevelConfig(BaseLogger::LvlMsg, BaseLogger::LevelConfig({&callback.stream}, "%T"));

    auto * coutBuff = std::cout.rdbuf();
    std::cout.rdbuf(callback.stream.rdbuf());
    try {
        LogMessage(logger).message() << "message" << LPush;
        std::cout.rdbuf(coutBuff);
    }
    catch (...) {
        std::cout.rdbuf(coutBuff);
        throw;
    }
    //---------------
    ASSERT_STREQ(" unexpected end of formatting string after [T], expected the second command letter\n", callback.result().c_str());
    //---------------
}

TEST(BaseLogger, invalid_callback_data_time_command_case1) {
    BaseLoggerCallback callback;
    BaseLogger logger;
    callback.setupLevels(&logger);
    logger.setLevelConfig(BaseLogger::LvlMsg, BaseLogger::LevelConfig({&callback.stream}, "%TM"));

    auto * coutBuff = std::cout.rdbuf();
    std::cout.rdbuf(callback.stream.rdbuf());
    try {
        LogMessage(logger).message() << "message" << LPush;
        std::cout.rdbuf(coutBuff);
    }
    catch (...) {
        std::cout.rdbuf(coutBuff);
        throw;
    }
    //---------------
    ASSERT_STREQ(" unexpected end of formatting string after the time command, expected '()'\n", callback.result().c_str());
    //---------------
}

TEST(BaseLogger, invalid_callback_data_time_command_case2) {
    BaseLoggerCallback callback;
    BaseLogger logger;
    callback.setupLevels(&logger);
    logger.setLevelConfig(BaseLogger::LvlMsg, BaseLogger::LevelConfig({&callback.stream}, "%TMG"));

    auto * coutBuff = std::cout.rdbuf();
    std::cout.rdbuf(callback.stream.rdbuf());
    try {
        LogMessage(logger).message() << "message" << LPush;
        std::cout.rdbuf(coutBuff);
    }
    catch (...) {
        std::cout.rdbuf(coutBuff);
        throw;
    }
    //---------------
    ASSERT_STREQ(" unexpected symbol G after time command, expected '('\n", callback.result().c_str());
    //---------------
}

TEST(BaseLogger, invalid_callback_data_time_command_case3) {
    BaseLoggerCallback callback;
    BaseLogger logger;
    callback.setupLevels(&logger);
    logger.setLevelConfig(BaseLogger::LvlMsg, BaseLogger::LevelConfig({&callback.stream}, "%TM("));

    auto * coutBuff = std::cout.rdbuf();
    std::cout.rdbuf(callback.stream.rdbuf());
    try {
        LogMessage(logger).message() << "message" << LPush;
        std::cout.rdbuf(coutBuff);
    }
    catch (...) {
        std::cout.rdbuf(coutBuff);
        throw;
    }
    //---------------
    ASSERT_STREQ(" unexpected end of formatting string after the time command, missed ')'\n", callback.result().c_str());
    //---------------
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(BaseLogger, threadsafe_simple_test) {
    BaseLoggerCallback callback;
    BaseLogger logger;
    callback.setupLevels(&logger);
    logger.setCallBack(BaseLogger::defaultThreadSafeCallBack);
    //---------------
    LogMessage(logger).debug() << "message" << LPush;
    ASSERT_STREQ("DBG:  message\n", callback.result().c_str());
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(LogMessage, abort) {
    BaseLoggerCallback callback;
    BaseLogger logger;
    callback.setupLevels(&logger);
    //---------------
    callback.clear();
    LogMessage msg(logger);
    msg.warning() << "message";
    msg.abort();
    msg.push();
    ASSERT_STREQ("", callback.result().c_str());
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
