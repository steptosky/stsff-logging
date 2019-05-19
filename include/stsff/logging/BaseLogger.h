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
#include "utils/SourceName.h"

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

namespace stsff {
namespace logging {

    /*!
     * \brief This is a base logger interface.
     * \details By default it prints all messages to std::cout and std::cerr.
     * \details Default log level is \"LvlDebug\".
     * \details Default levels handler is \link BaseLogger::defaultHandler \endlink
     * \details The logger supports categories for logger and messages.
     * \code 
     * BaseLogger::setName("my logger category");
     * LMessage(logger).setCategory("my message category") << "my message";
     * LcMessage(logger, "my message category") << "my message";
     * \endcode
     * \note You can define log printing for your own types. \see \link LogMessage \endlink
     * \note You can define your own levels and its handler as this is just std::size_t.
     * \note You can define your own handler for any level with std::bind or lambda.
     * \code 
	 * BaseLogger::setHandler(BaseLogger::LvlDebug, std::bind(defaultHandler, std::placeholders::_1, std::placeholders::_2,
     *                                                        std::vector<std::ostream*>{&std::cout}, 
     *                                                        "DBG: %LC %MC %MS", colorize::magenta)) 
     * \endcode
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
            LogMsg(const std::size_t lvl, const StringView category, const StringView msg,
                   const StringView fn, const StringView fl, const int ln)
                : mCategory(category),
                  mMsg(msg),
                  mFunction(fn),
                  mFile(fl),
                  mLevel(lvl),
                  mLine(ln) {}

            StringView mCategory; //!< message category.
            StringView mMsg;      //!< message itself.
            StringView mFunction; //!< function name.
            StringView mFile;     //!< source file name.
            std::size_t mLevel;   //!< level of current message.
            int mLine;            //!< source line number.
        };

        /// @}
        //---------------------------------------------------------------
        /// @{

        /*!
         * \details Handler for levels.
         */
        typedef std::function<void(const BaseLogger &, const LogMsg &)> LevelHandler;

        /*!
         * \details Map fo the log level handlers.
         */
        typedef std::unordered_map<std::size_t, LevelHandler> LevelHandlers;

        /*!
         * \details Color function for streams and for default handlers.
         */
        typedef std::ostream & (*ColorFn)(std::ostream &);

        /*!
         * \see \link BaseLogger::defaultHandler \endlink
         */
        typedef std::function<void(const BaseLogger * logger, const LogMsg & logMsg)> CallBack;

        /// @}
        //---------------------------------------------------------------
        /// @{

        LoggingExp explicit BaseLogger(StringView category = StringView());

        explicit BaseLogger(const StringView category, LevelHandlers levelsConf)
            : mLevels(std::move(levelsConf)),
              mCategory(category.data(), category.size()) {}

        LoggingExp virtual ~BaseLogger() noexcept;

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
        LoggingExp void log(const LogMsg & logMsg) const;

        /// @}
        //---------------------------------------------------------------
        /// @{

        /*!
         * \details Set level's handler.
         * \param [in] level
         * \param [in] handler
         */
        LoggingExp void setHandler(std::size_t level, const LevelHandler & handler) noexcept;

        /*!
         * \details Get the levels handler map.
         * \return levels handler map.
         */
        const LevelHandlers & handlers() const { return mLevels; }

        /*!
         * \details Get the levels handler map
         * \return levels handler map.
         */
        LevelHandlers & handlers() { return mLevels; }

        /// @}
        //---------------------------------------------------------------
        /// @{

        /*!
         * \details Constraint for level printing.
         *          Default is \link BaseLogger::LvlDebug \endlink
         * \param [in] level
         */
        void setLevel(const std::size_t level) { mLevel = level; }

        /*!
         * \return current level printing.
         */
        std::size_t level() const { return mLevel; }

        /*!
         * \details Set log category name.
         * \param [in] category
         */
        void setName(const std::string & category) { mCategory = category; }

        /*!
         * \details Get log category name.
         * \return log category name.
         */
        const std::string & name() const { return mCategory; }

        /// @}
        //---------------------------------------------------------------
        /// @{

        /*!
         * \details A default callback for log printing.
         * \details Setup example:
         *          \li \%TM - time that takes format string for the std::strftime function inside brackets.
         *          \li \%LN - log name.
         *          \li \%MC - message category.
         *          \li \%MS - message.
         *          \li \%FN - function name.
         *          \li \%FI - file name.
         *          \li \%LI - file line.
         * \param [in] logger
         * \param [in] logMsg
         * \param [in] stream
         * \param [in] formatting example \code "ERR: %LC %MC [%TM(%Y-%m-%d %T)] %MS \n\t[%FN -> %FI(%LI)]" \endcode
         * \param [in] color
         */
        LoggingExp static void defaultHandler(const BaseLogger & logger, const LogMsg & logMsg,
                                              std::ostream & stream,
                                              const std::string & formatting,
                                              ColorFn color);

        /*!
         * \details Makes timestamp string.
         * \param [in] format see description of C++ std::strftime function.
         * \warning Maximum string size is 99.
         */
        LoggingExp static std::string timeStamp(const std::string & format = "%Y-%m-%d %T") noexcept;

        /// @}
        //---------------------------------------------------------------

    private:

        LevelHandlers mLevels;
        std::string mCategory;
        std::size_t mLevel = LvlDebug;

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
     *     *this << msg; // change it for your type
     *     return *this;
     * }
     * \endcode
     */
    class LogMessage final {
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
            : mLogMsg(BaseLogger::LvlMsg, StringView(), StringView(), function, file, line),
              mLog(logger) {
            assert(mLog);
        }

        ~LogMessage() {
            push();
        }

        LogMessage(const LogMessage &) = delete;
        LogMessage(LogMessage &&) = default;
        LogMessage & operator=(const LogMessage &) = delete;
        LogMessage & operator=(LogMessage &&) = default;

        /// @}
        //---------------------------------------------------------------
        /// @{

        /*!
		 * \brief Alternative for the operator <<
		 * \details Example: writeSp(1,2,3) -> "123"
		 * \see \link LogMessage::writeSp \endlink
		 * \param [in] args
		 */
        template<typename... Args>
        LogMessage & write(const Args & ... args) {
            int unpack[]{0, ((*this << args), 0)...};
            return *this;
        }

        /*!
         * \brief Write message with the space between.
         * \details Example: writeSp(1,2,3) -> "1 2 3 "
         * \pre This function takes 2 or more arguments.
         * \see \link LogMessage::write \endlink
         * \param [in] first
         * \param [in] other
         */
        template<typename T, typename... Args>
        LogMessage & writeSp(const T & first, const Args & ... other) {
            static_assert(sizeof...(other), "This function takes 2 or more arguments!");
            *this << first;
            int unpack[]{0, ((*this << " " << other), 0)...};
            return *this;
        }

        /// @}
        //---------------------------------------------------------------
        /// @{

        template<class T>
        LogMessage & operator<<(const T & msg) {
            mStream << msg;
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

        LogMessage & critical() noexcept { return level(static_cast<std::size_t>(BaseLogger::LvlCritical)); }
        LogMessage & error() noexcept { return level(static_cast<std::size_t>(BaseLogger::LvlError)); }
        LogMessage & fail() noexcept { return level(static_cast<std::size_t>(BaseLogger::LvlFail)); }
        LogMessage & warning() noexcept { return level(static_cast<std::size_t>(BaseLogger::LvlWarning)); }
        LogMessage & success() noexcept { return level(static_cast<std::size_t>(BaseLogger::LvlSuccess)); }
        LogMessage & info() noexcept { return level(static_cast<std::size_t>(BaseLogger::LvlInfo)); }
        LogMessage & message() noexcept { return level(static_cast<std::size_t>(BaseLogger::LvlMsg)); }
        LogMessage & debug() noexcept { return level(static_cast<std::size_t>(BaseLogger::LvlDebug)); }

        /// @}
        //---------------------------------------------------------------
        /// @{

        LogMessage & setCategory(const StringView category) {
            mLogMsg.mCategory = category;
            return *this;
        }

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
        LoggingExp void push() noexcept;

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

// Log messages

// Just creates LogMessage variable and you can use it then 
#   define LVar(VAR,L)      stsff::logging::LogMessage VAR(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__);VAR
#   define LCritical(L)         stsff::logging::LogMessage(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__).critical()
#   define LError(L)            stsff::logging::LogMessage(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__).error()
#   define LFail(L)             stsff::logging::LogMessage(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__).fail()
#   define LWarning(L)          stsff::logging::LogMessage(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__).warning()
#   define LSuccess(L)          stsff::logging::LogMessage(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__).success()
#   define LInfo(L)             stsff::logging::LogMessage(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__).info()
#   define LMessage(L)          stsff::logging::LogMessage(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__).message()
#   define LDebug(L)            stsff::logging::LogMessage(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__).debug()
#   define LLevel(L,LVL)        stsff::logging::LogMessage(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__).level(LVL)
#   define LTimeStamp(F)        stsff::logging::BaseLogger::timeStamp(F)

// Category log messages

// Just creates LogMessage variable and you can use it then 
#   define LcVar(VAR,L,C)   stsff::logging::LogMessage VAR(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__);VAR.setCategory(C)
#   define LcCritical(L,C)      stsff::logging::LogMessage(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__).setCategory(C).critical()
#   define LcError(L,C)         stsff::logging::LogMessage(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__).setCategory(C).error()
#   define LcWarning(L,C)       stsff::logging::LogMessage(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__).setCategory(C).warning()
#   define LcSuccess(L,C)       stsff::logging::LogMessage(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__).setCategory(C).success()
#   define LcInfo(L,C)          stsff::logging::LogMessage(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__).setCategory(C).info()
#   define LcMessage(L,C)       stsff::logging::LogMessage(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__).setCategory(C).message()
#   define LcDebug(L,C)         stsff::logging::LogMessage(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__).setCategory(C).debug()
#   define LcLevel(L,C,LVL)     stsff::logging::LogMessage(L, __STS_FUNC_NAME__, stsff::logging::sourcePath(__FILE__), __LINE__).setCategory(C).level(LVL)

// Force push
#   define LPush stsff::logging::LogMessage::CmdPush()

#   define LEOL "\n"

#endif

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
