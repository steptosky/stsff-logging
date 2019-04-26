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

#include <cassert>
#include <iostream>
#include <sstream>
#include <functional>
#include <unordered_map>
#include <limits>
#include <vector>
#include "internal/InternalUtils.h"

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

namespace stsff {
namespace logging {

    /*!
     * \brief This is a base logger interface.
     * \details By default it prints all messages to std::cout and std::cerr.
     * \details Default log level is \"LvlDebug\".
     * \details Default callback is \link BaseLogger::defaultCallBack \endlink
     * \details The logger supports categories for logger.
     * \code \link logger->setLogCategoryName("my logger category") \endlink; \endcode
     * \note You can define log printing for your own types. \see \link LogMessage \endlink
     * \note You can define your own levels as this is just std::size_t.
     */
    class BaseLogger {
    public:

        typedef internal::CustStringView StringView;

        //---------------------------------------------------------------
        /// @{

        /*!
         * \brief Predefined log levels
         * \details This has interval 100 so
         *          you can easily specify your own levels
         *          between this standard ones.
         */
        enum eLevel : std::size_t {
            LvlCritical = 100,
            LvlError = 200,
            LvlFail = 300,
            LvlWarning = 400,
            LvlSuccess = 500,
            LvlInfo = 600,
            LvlMsg = 700,
            LvlDebug = 800,
        };

        /// @}
        //---------------------------------------------------------------
        /// @{

        /*! 
         * \brief Represents a log message.
         */
        struct LogMsg {
            LogMsg(const std::size_t lvl, const StringView msg,
                   const StringView fn, const StringView fl, const int ln)
                : mMsg(msg),
                  mFunction(fn),
                  mFile(fl),
                  mLevel(lvl),
                  mLine(ln) {}

            StringView mMsg;      //!< message itself.
            StringView mFunction; //!< function name.
            StringView mFile;     //!< source file name.
            std::size_t mLevel;   //!< level of current message.
            int mLine;            //!< source line number.
        };

        //---------------------------------------------------------------

        /*!
         * \brief Represents log level configuration.
         * \details It allows you to adjust each level as you wish.
         * \details It supports multistream printing and formatting message.
         * \details Setup example:
         *          \code  LevelConfig({&std::cout}, "ERR: ", "ERR: %LC %MC [%TM(%Y-%m-%d %T)] %MS \n\t[%FN -> %FI(%LI)]", colorize::red) \endcode
         *          \li \%TM - time that takes format string for the std::strftime function inside brackets.
         *          \li \%LC - log category.
         *          \li \%MS - message.
         *          \li \%FN - function name.
         *          \li \%FI - file name.
         *          \li \%LI - file line.
         * 
         */
        struct LevelConfig {
            typedef std::ostream &(*ColorFn)(std::ostream &);

            explicit LevelConfig(std::vector<std::ostream *> streams, std::string formatting, const ColorFn color = nullptr)
                : mStreams(std::move(streams)),
                  mFormatting(std::move(formatting)),
                  mColor(color) {}

            LevelConfig(const LevelConfig &) = default;
            LevelConfig(LevelConfig &&) = default;

            ~LevelConfig() = default;

            LevelConfig & operator=(const LevelConfig &) = default;
            LevelConfig & operator=(LevelConfig &&) = default;

            std::vector<std::ostream *> mStreams; //!< stream where the level will be printed.
            std::string mFormatting;              //!< message formatting
            ColorFn mColor;                       //!< text color in terminal.
        };

        /// @}
        //---------------------------------------------------------------
        /// @{

        /*!
         * \see Maps fo the l;og levels configurations.
         */
        typedef std::unordered_map<std::size_t, LevelConfig> LevelConfigs;

        /*!
         * \see BaseLogger::defaultCallBack
         * \see BaseLogger::defaultThreadSafeCallBack
         */
        typedef std::function<void(const BaseLogger * logger, const LogMsg & logMsg)> CallBack;

        /// @}
        //---------------------------------------------------------------
        /// @{

        LoggingExp explicit BaseLogger(StringView category = StringView(), const CallBack & callBack = defaultCallBack);

        explicit BaseLogger(StringView category, LevelConfigs levelsConf, CallBack callBack = defaultCallBack)
            : mLevels(std::move(levelsConf)),
              mCategory(category.data(), category.size()),
              mCallBack(std::move(callBack)) {}

        explicit BaseLogger(LevelConfigs levelsConf, StringView category = StringView(), CallBack callBack = defaultCallBack)
            : mLevels(std::move(levelsConf)),
              mCategory(category.data(), category.size()),
              mCallBack(std::move(callBack)) {}

        virtual ~BaseLogger() = default;

        BaseLogger(const BaseLogger &) = default;
        BaseLogger(BaseLogger &&) = default;
        BaseLogger & operator=(const BaseLogger &) = default;
        BaseLogger & operator=(BaseLogger &&) = default;

        /// @}
        //---------------------------------------------------------------
        /// @{

        /*!
         * \details Logs specified message.
         * \details Usually developers should not call this method directly.
         * \param [in] logMsg
         */
        void log(const LogMsg & logMsg) const {
            if (logMsg.mLevel <= mLevel) {
                mCallBack(this, logMsg);
            }
        }

        /// @}
        //---------------------------------------------------------------
        /// @{

        /*!
         * \details Set level configuration by id.
         * \param [in] level
         * \param [in] conf
         */
        LoggingExp void setLevelConfig(std::size_t level, const LevelConfig & conf);

        /*!
         * \details Get the level configurations map.
         * \return level configurations map.
         */
        const LevelConfigs & levelConfigs() const { return mLevels; }

        /*!
         * \details Get the level configurations map.
         * \return level configurations map.
         */
        LevelConfigs & levelConfigs() { return mLevels; }

        /*!
         * \details Get the level configuration by its id.
         * \param [in] level 
         * \return nullptr if specified level isn't found otherwise the pointer to config.
         */
        LoggingExp const LevelConfig * levelConfig(std::size_t level) const;

        /*!
         * \details Get the level configuration by its id.
         * \param [in] level
         * \return nullptr if specified level isn't found otherwise the pointer to config.
         */
        LoggingExp LevelConfig * levelConfig(std::size_t level);

        /// @}
        //---------------------------------------------------------------
        /// @{

        /*!
         * \details Set current function for log printing.
         * \see BaseLogger::defaultCallBack
         * \see BaseLogger::defaultThreadSafeCallBack
         * \param [in] callBack
         */
        void setCallBack(const CallBack & callBack) { mCallBack = callBack; }

        /// @}
        //---------------------------------------------------------------
        /// @{

        /*!
         * \details A default callback for log printing.
         * \param [in] logger 
         * \param [in] logMsg 
         */
        LoggingExp static void defaultCallBack(const BaseLogger * logger, const LogMsg & logMsg);

        /*!
         * \copydoc BaseLogger::defaultCallBack
         * \details Thread safe version.
         */
        LoggingExp static void defaultThreadSafeCallBack(const BaseLogger * logger, const LogMsg & logMsg);

        /*!
         * \details Makes timestamp string.
         * \param [in] format see description of C++ std::strftime function.
         *                    Maximum string size is 99.
         */
        LoggingExp static std::string timeStamp(const std::string & format = "%Y-%m-%d %T");

        /// @}
        //---------------------------------------------------------------
        /// @{

        /*!
         * \details Constraint for level printing.
         *          Default is \link BaseLogger::LvlDebug \endlink
         * \param [in] level
         */
        void setPrintLevel(const std::size_t level) { mLevel = level; }

        /*!
         * \return current level printing.
         */
        std::size_t printLevel() const { return mLevel; }

        /*!
         * \details Enable/Disable text colorizing in consoles.
         * \param [in] state
         */
        void setTextColorizing(const bool state) { mTextColorizing = state; }

        /*!
         * \see BaseLogger::setTextColorizing
         */
        bool isTextColorizing() const { return mTextColorizing; }

        /*!
         * \details Set log category name.
         * \param [in] category
         */
        void setLogCategoryName(const std::string & category) { mCategory = category; }

        /*!
         * \details Get log category name.
         * \return log category name.
         */
        const std::string & logCategoryName() const { return mCategory; }

        /// @}
        //---------------------------------------------------------------

    private:

        LevelConfigs mLevels;
        std::string mCategory;
        CallBack mCallBack = defaultCallBack;
        std::size_t mLevel = LvlDebug;
        bool mTextColorizing = true;

    };

    /**************************************************************************************************/
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**************************************************************************************************/

    /*!
     * \brief Message for \link BaseLogger \endlink.
     * \details Represents one log message.
     * \note In normal way you should not use this class directly use the macros instead.
     * \code LWarning(logger) << "My warning"; \endcode
     * \note The message will be printed when destructor is called
     *       or if you manually forced printing with the method \link LogMessage::push() \endlink
     *       or operator << with \link LogMessage::CmdPush \endlink param.
     *       Also you can use the macro LPush.
     * \code LWarning(logger) << "My warning" << LPush; \endcode
     *       It can be needed when you process the exceptions or
     *       want printing the message before its destructor calls.
     * \note As the class has template operator << you can define log printing for your own types.
     * \code
     * // define somewhere
     * template<>
     * inline stsff::LogMessage & stsff::LogMessage::operator<<<MyType>(const MyType & msg) {
     *     this->operator<< msg; // change it for your type
     *     return *this;
     * }
     * \endcode
     */
    class LogMessage {
    public:

        typedef BaseLogger::StringView StringView;

        struct CmdPush {};

        /// @}
        //---------------------------------------------------------------
        /// @{

        explicit LogMessage(const BaseLogger & logger)
            : LogMessage(&logger, StringView(), StringView(), 0) {}

        explicit LogMessage(const BaseLogger * logger)
            : LogMessage(logger, StringView(), StringView(), 0) {}

        LogMessage(const BaseLogger & logger, const StringView function, const StringView file, const int line)
            : LogMessage(&logger, function, file, line) {}

        LogMessage(const BaseLogger * logger, const StringView function, const StringView file, const int line)
            : mLogMsg(BaseLogger::LvlMsg, StringView(), function, file, line),
              mLog(logger) {
            assert(mLog);
        }

        virtual ~LogMessage() {
            push();
        }

        /// @}
        //---------------------------------------------------------------
        /// @{

        template<class T>
        LogMessage & operator<<(const T & msg) {
            if (mLogMsg.mLevel <= mLog->printLevel()) {
                mStream << msg;
            }
            return *this;
        }

        LogMessage & operator<<(const CmdPush &) {
            push();
            return *this;
        }

        LogMessage & write(const char * data, const std::size_t size) {
            mStream.write(data, size);
            return *this;
        }

        /// @}
        //---------------------------------------------------------------
        /// @{

        /*!
         * \brief Sets custom level.
         * \details This sets your own custom level
         * \tparam T integer, std::size_t is proffered.
         * \param [in] level
         */
        template<typename T>
        LogMessage & level(const T level) {
            static_assert(std::numeric_limits<T>::is_integer,
                "This method can be used for the integers types only. See also BaseLogger::eLevel.");
            mLogMsg.mLevel = BaseLogger::eLevel(level);
            return *this;
        }

        LogMessage & critical() { return level(static_cast<std::size_t>(BaseLogger::LvlCritical)); }
        LogMessage & error() { return level(static_cast<std::size_t>(BaseLogger::LvlError)); }
        LogMessage & fail() { return level(static_cast<std::size_t>(BaseLogger::LvlFail)); }
        LogMessage & warning() { return level(static_cast<std::size_t>(BaseLogger::LvlWarning)); }
        LogMessage & success() { return level(static_cast<std::size_t>(BaseLogger::LvlSuccess)); }
        LogMessage & info() { return level(static_cast<std::size_t>(BaseLogger::LvlInfo)); }
        LogMessage & message() { return level(static_cast<std::size_t>(BaseLogger::LvlMsg)); }
        LogMessage & debug() { return level(static_cast<std::size_t>(BaseLogger::LvlDebug)); }

        /// @}
        //---------------------------------------------------------------
        /// @{

        LogMessage & setFunction(const StringView fn) {
            mLogMsg.mFunction = fn;
            return *this;
        }

        LogMessage & setFile(const StringView file) {
            mLogMsg.mFile = file;
            return *this;
        }

        LogMessage & setFileLine(const int line) {
            mLogMsg.mLine = line;
            return *this;
        }

        /// @}
        //---------------------------------------------------------------
        /// @{

        /*!
         * \brief Prints message now.
         * \details As the normal way to printing the message when destructor calls,
         *          you may forced printing when you want.
         * \details If message was aborted with \link LogMessage::abort \endlink
         *          this method will not have any effects.
         */
        LoggingExp void push();

        /*!
         * \brief Prevents printing the message.
         */
        void abort() {
            mPushed = true;
        }

        /*!
         * \return Log message string.
         */
        std::string string() const {
            return mStream.str();
        }

        /// @}
        //---------------------------------------------------------------

    private:

        std::stringstream mStream;
        BaseLogger::LogMsg mLogMsg;
        const BaseLogger * mLog = nullptr;
        bool mPushed = false;

    };

}
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

/*!
 * \details define this macro if you have macro conflicts 
 *          and then make your one macros like this.
 */
#ifndef STSFF_LOGGER_DONT_USE_MACROS

// Just creates LogMessage variable and you can use it then 
#   define LCustomVar(VAR,L)  stsff::logging::LogMessage VAR(L, __STS_FUNC_NAME__, __FILE__, __LINE__)

// Log messages                 
#   define LCritical(L)       stsff::logging::LogMessage(L, __STS_FUNC_NAME__,  stsff::logging::internal::fileName(__FILE__), __LINE__).critical()
#   define LError(L)          stsff::logging::LogMessage(L, __STS_FUNC_NAME__,  stsff::logging::internal::fileName(__FILE__), __LINE__).error()
#   define LFail(L)           stsff::logging::LogMessage(L, __STS_FUNC_NAME__,  stsff::logging::internal::fileName(__FILE__), __LINE__).fail()
#   define LWarning(L)        stsff::logging::LogMessage(L, __STS_FUNC_NAME__,  stsff::logging::internal::fileName(__FILE__), __LINE__).warning()
#   define LSuccess(L)        stsff::logging::LogMessage(L, __STS_FUNC_NAME__,  stsff::logging::internal::fileName(__FILE__), __LINE__).success()
#   define LInfo(L)           stsff::logging::LogMessage(L, __STS_FUNC_NAME__,  stsff::logging::internal::fileName(__FILE__), __LINE__).info()
#   define LMessage(L)        stsff::logging::LogMessage(L, __STS_FUNC_NAME__,  stsff::logging::internal::fileName(__FILE__), __LINE__).message()
#   define LDebug(L)          stsff::logging::LogMessage(L, __STS_FUNC_NAME__,  stsff::logging::internal::fileName(__FILE__), __LINE__).debug()
#   define LLevel(L,LVL)      stsff::logging::LogMessage(L, __STS_FUNC_NAME__,  stsff::logging::internal::fileName(__FILE__), __LINE__).level(LVL)

// Force push
#   define LPush stsff::logging::LogMessage::CmdPush()

#   define LEOL "\n"

#endif

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
