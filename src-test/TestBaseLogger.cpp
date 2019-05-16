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

TEST(BaseLogger, variadic) {
    std::stringstream stream;
    BaseLogger logger("log-category");
    logger.setHandler(BaseLogger::LvlMsg, [&](const BaseLogger & l, const BaseLogger::LogMsg & logMsg) {
        BaseLogger::defaultHandler(l, logMsg, std::vector<std::ostream*>{&stream}, "%LC,%MC,%MS,%FN,%FI,%LI", nullptr);
    });
    //---------------
    LogMessage(logger).setCategory("msg-cat").write("message1", "message2", "message3").push();
    auto result = stream.str();
    EXPECT_STREQ("log-category,msg-cat,message1message2message3,,,0\n", result.c_str());
    //---------------
    stream.str(std::string());
    LogMessage(logger).setCategory("msg-cat").writeSp("message1", "message2", "message3").push();
    result = stream.str();
    EXPECT_STREQ("log-category,msg-cat,message1 message2 message3,,,0\n", result.c_str());
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(BaseLogger, formatting) {
    std::stringstream stream;
    BaseLogger logger("log-category");
    logger.setHandler(BaseLogger::LvlMsg, [&](const BaseLogger & l, const BaseLogger::LogMsg & logMsg) {
        BaseLogger::defaultHandler(l, logMsg, std::vector<std::ostream*>{&stream}, "%LC,%MC,%MS,%FN,%FI,%LI", nullptr);
    });
    //---------------
    LogMessage(logger).setCategory("msg-cat").message() << "message" << LPush;
    auto result = stream.str();
    EXPECT_STREQ("log-category,msg-cat,message,,,0\n", result.c_str());
    //---------------
    stream.str(std::string());
    LogMessage(logger, "function", "file", 5).message() << "message" << LPush;
    result = stream.str();
    EXPECT_STREQ("log-category,,message,function,file,5\n", result.c_str());
}

TEST(BaseLogger, formatting_custom_level) {
    std::stringstream stream;
    BaseLogger logger;
    logger.setHandler(BaseLogger::eLevel(5), [&](const BaseLogger & l, const BaseLogger::LogMsg & logMsg) {
        BaseLogger::defaultHandler(l, logMsg, std::vector<std::ostream*>{&stream}, "5,%LC,%MC,%MS,%FN,%FI,%LI", nullptr);
    });
    //---------------
    LogMessage(logger).level(5) << "message" << LPush;
    const auto result = stream.str();
    EXPECT_STREQ("5,,,message,,,0\n", result.c_str());
    //---------------
}

TEST(BaseLogger, formatting_unknown_level) {
    const std::stringstream stream;
    const BaseLogger logger;
    auto * coutBuff = std::cout.rdbuf();
    std::cout.rdbuf(stream.rdbuf());
    try {
        LogMessage(logger).level(25) << "message" << LPush;
        std::cout.rdbuf(coutBuff);
    }
    catch (...) {
        std::cout.rdbuf(coutBuff);
        throw;
    }
    const auto result = stream.str();
    ASSERT_STREQ("LVL(25):   message \n\t[ -> (0)]\n", result.c_str());
}

TEST(BaseLogger, formatting_level) {
    std::stringstream stream;
    BaseLogger logger;
    logger.setHandler(BaseLogger::LvlInfo, [&](const BaseLogger & l, const BaseLogger::LogMsg & logMsg) {
        BaseLogger::defaultHandler(l, logMsg, std::vector<std::ostream*>{&stream}, "INF: %LC %MC %MS", colorize::cyan);
    });
    logger.setHandler(BaseLogger::LvlWarning, [&](const BaseLogger & l, const BaseLogger::LogMsg & logMsg) {
        BaseLogger::defaultHandler(l, logMsg, std::vector<std::ostream*>{&stream}, "WRN: %LC %MC %MS", colorize::yellow);
    });
    logger.setPrintLevel(BaseLogger::LvlWarning);
    //---------------
    LogMessage(logger).info() << "message" << LPush;
    auto result = stream.str();
    ASSERT_STREQ("", result.c_str());
    //---------------
    stream.str(std::string());
    LogMessage(logger).warning() << "message" << LPush;
    result = stream.str();
    ASSERT_STREQ("WRN:   message\n", result.c_str());
}

#ifdef NDEBUG // 'Only file name' is enabled in release mode
TEST(BaseLogger, formatting_sources_only_filename_case1) {
    std::stringstream stream;
    BaseLogger logger;
    logger.setHandler(BaseLogger::LvlMsg, [&](const BaseLogger & l, const BaseLogger::LogMsg & logMsg) {
        BaseLogger::defaultHandler(l, logMsg, std::vector<std::ostream*>{&stream}, "--  %LC %MC %MS", nullptr);
    });
    //---------------
    LMessage(logger) << "message" << LPush;
    auto result = stream.str();
    const auto it = std::find_if(result.begin(), result.end(), [](const char v) {
        return v == '\\' || v == '/';
    });
    ASSERT_TRUE(it == result.end());
}

TEST(BaseLogger, formatting_sources_only_filename_case2) {
    std::stringstream stream;
    BaseLogger logger;
    logger.setHandler(BaseLogger::LvlMsg, [&](const BaseLogger & l, const BaseLogger::LogMsg & logMsg) {
        BaseLogger::defaultHandler(l, logMsg, std::vector<std::ostream*>{&stream}, "%FI", nullptr);
    });
    //---------------
    LMessage(logger) << "message" << LPush;
    const auto result = stream.str();
    ASSERT_STRCASEEQ("TestBaseLogger.cpp\n", result.c_str());
}
#endif

TEST(BaseLogger, colorize) {
    const BaseLogger logger;
    LVar(logMsg, logger).message() << "custom var message" << LPush;
    LDebug(logger) << "debug message" << LPush;
    LMessage(logger) << "just a message" << LPush;
    LInfo(logger) << "info message" << LPush;
    LSuccess(logger) << "info success message" << LPush;
    LWarning(logger) << "warning message" << LPush;
    LFail(logger) << "fail message" << LPush;
    LError(logger) << "error message" << LPush;
    LCritical(logger) << "critical message" << LPush;
}

TEST(BaseLogger, time_stamp) {
    std::stringstream stream;
    BaseLogger logger;
    logger.setHandler(BaseLogger::LvlMsg, [&](const BaseLogger & l, const BaseLogger::LogMsg & logMsg) {
        BaseLogger::defaultHandler(l, logMsg, std::vector<std::ostream*>{&stream}, "%TM(%Y)", nullptr);
    });
    //---------------
    LogMessage(logger).message() << "message" << LPush;
    const auto result = stream.str();
    EXPECT_STREQ(LTimeStamp("%Y\n").c_str(), result.c_str());
    //---------------
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

#ifndef NDEBUG
TEST(BaseLogger, invalid_callback_data_null_stream) {
    BaseLogger logger;
    logger.setHandler(BaseLogger::LvlMsg, [&](const BaseLogger & l, const BaseLogger::LogMsg & logMsg) {
        BaseLogger::defaultHandler(l, logMsg, std::vector<std::ostream*>{nullptr}, "%TM(%Y)", nullptr);
    });
    //---------------
    ASSERT_DEBUG_DEATH(LogMessage(logger).message() << "message" << LPush, "");
    //---------------
}
#endif

TEST(BaseLogger, invalid_callback_data_command_case1) {
    std::stringstream stream;
    BaseLogger logger;
    logger.setHandler(BaseLogger::LvlMsg, [&](const BaseLogger & l, const BaseLogger::LogMsg & logMsg) {
        BaseLogger::defaultHandler(l, logMsg, std::vector<std::ostream*>{&stream}, "%TG", nullptr);
    });

    auto * coutBuff = std::cout.rdbuf();
    std::cout.rdbuf(stream.rdbuf());
    try {
        LogMessage(logger).message() << "message" << LPush;
        std::cout.rdbuf(coutBuff);
    }
    catch (...) {
        std::cout.rdbuf(coutBuff);
        throw;
    }
    //---------------
    const auto result = stream.str();
    ASSERT_STREQ(" unknown formatting command: [TG]\n", result.c_str());
    //---------------
}

TEST(BaseLogger, invalid_callback_data_command_case2) {
    std::stringstream stream;
    BaseLogger logger;
    logger.setHandler(BaseLogger::LvlMsg, [&](const BaseLogger & l, const BaseLogger::LogMsg & logMsg) {
        BaseLogger::defaultHandler(l, logMsg, std::vector<std::ostream*>{&stream}, "%T", nullptr);
    });

    auto * coutBuff = std::cout.rdbuf();
    std::cout.rdbuf(stream.rdbuf());
    try {
        LogMessage(logger).message() << "message" << LPush;
        std::cout.rdbuf(coutBuff);
    }
    catch (...) {
        std::cout.rdbuf(coutBuff);
        throw;
    }
    //---------------
    const auto result = stream.str();
    ASSERT_STREQ(" unexpected end of formatting string after [T], expected the second command letter\n", result.c_str());
    //---------------
}

TEST(BaseLogger, invalid_callback_data_time_command_case1) {
    std::stringstream stream;
    BaseLogger logger;
    logger.setHandler(BaseLogger::LvlMsg, [&](const BaseLogger & l, const BaseLogger::LogMsg & logMsg) {
        BaseLogger::defaultHandler(l, logMsg, std::vector<std::ostream*>{&stream}, "%TM", nullptr);
    });

    auto * coutBuff = std::cout.rdbuf();
    std::cout.rdbuf(stream.rdbuf());
    try {
        LogMessage(logger).message() << "message" << LPush;
        std::cout.rdbuf(coutBuff);
    }
    catch (...) {
        std::cout.rdbuf(coutBuff);
        throw;
    }
    //---------------
    const auto result = stream.str();
    ASSERT_STREQ(" unexpected end of formatting string after the time command, expected '()'\n", result.c_str());
    //---------------
}

TEST(BaseLogger, invalid_callback_data_time_command_case2) {
    std::stringstream stream;
    BaseLogger logger;
    logger.setHandler(BaseLogger::LvlMsg, [&](const BaseLogger & l, const BaseLogger::LogMsg & logMsg) {
        BaseLogger::defaultHandler(l, logMsg, std::vector<std::ostream*>{&stream}, "%TMG", nullptr);
    });

    auto * coutBuff = std::cout.rdbuf();
    std::cout.rdbuf(stream.rdbuf());
    try {
        LogMessage(logger).message() << "message" << LPush;
        std::cout.rdbuf(coutBuff);
    }
    catch (...) {
        std::cout.rdbuf(coutBuff);
        throw;
    }
    //---------------
    const auto result = stream.str();
    ASSERT_STREQ(" unexpected symbol G after time command, expected '('\n", result.c_str());
    //---------------
}

TEST(BaseLogger, invalid_callback_data_time_command_case3) {
    std::stringstream stream;
    BaseLogger logger;
    logger.setHandler(BaseLogger::LvlMsg, [&](const BaseLogger & l, const BaseLogger::LogMsg & logMsg) {
        BaseLogger::defaultHandler(l, logMsg, std::vector<std::ostream*>{&stream}, "%TM(", nullptr);
    });

    auto * coutBuff = std::cout.rdbuf();
    std::cout.rdbuf(stream.rdbuf());
    try {
        LogMessage(logger).message() << "message" << LPush;
        std::cout.rdbuf(coutBuff);
    }
    catch (...) {
        std::cout.rdbuf(coutBuff);
        throw;
    }
    //---------------
    const auto result = stream.str();
    ASSERT_STREQ(" unexpected end of formatting string after the time command, missed ')'\n", result.c_str());
    //---------------
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(BaseLogger, threadsafe_simple_test) {
    std::stringstream stream;
    BaseLogger logger;
    logger.setHandler(BaseLogger::LvlDebug, [&](const BaseLogger & l, const BaseLogger::LogMsg & logMsg) {
        BaseLogger::defaultThreadSafeHandler(l, logMsg, std::vector<std::ostream*>{&stream}, "DBG: %LC %MC %MS", nullptr);
    });
    //---------------
    LogMessage(logger).debug() << "message" << LPush;
    const auto result = stream.str();
    ASSERT_STREQ("DBG:   message\n", result.c_str());
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(LogMessage, abort) {
    std::stringstream stream;
    BaseLogger logger;
    logger.setHandler(BaseLogger::LvlWarning, [&](const BaseLogger & l, const BaseLogger::LogMsg & logMsg) {
        BaseLogger::defaultThreadSafeHandler(l, logMsg, std::vector<std::ostream*>{&stream}, "WRN: %LC %MC %MS", colorize::yellow);
    });
    //---------------
    LogMessage msg(logger);
    msg.warning() << "message";
    msg.abort();
    msg.push();
    //---------------
    const auto result = stream.str();
    ASSERT_STREQ("", result.c_str());
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
