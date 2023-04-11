#pragma once

#include "exlib/win32/declspec.h"

// A minimal set of logging functions that can be bound to a more robust
// logging solution in the future if needed.

// Unless you explicitly define EXLIB_VERBOSE_LOGGING, only Failure messages
// will be logged.  To turn on logging for a specific call, even when
// EXLIB_VERBOSE_LOGGING is unset, pass true as the last argument to the
// following functions: 
// logging of success messages:
//      VerifyZero
//      VerifyZeroLambda
//      VerifyZeroSpelledError
//      Verify
//      VerifyLambda
//      LogTry
// The default final argument is set to false.
// 
// When EXLIB_VERBOSE_LOGGING, the alwaysLog argument is ignored.
// -- Jonathan

#include <string>
#include <iostream>
#include <ctype.h>
#include <type_traits>
#include <cassert>

#include "exlib/xplatform_types.h"
#include "exlib/format.h"
#include "exlib/cout_buffer.h"
#include "exlib/reverse_byte_order.h"

#define DEBUG_LOG_PREFIX "[DEBUG] "

#define WARN_LOG_PREFIX "[WARN] "
#define ERROR_LOG_PREFIX "[ERROR] "
#define ASSERT_LOG_PREFIX "[ASSERT_FAILURE] "

#ifndef DEBUG_LOG_OSTREAM
#define DEBUG_LOG_OSTREAM ex::CoutBuffer()
#endif

#ifndef WARN_LOG_OSTREAM
#define WARN_LOG_OSTREAM ex::CoutBuffer()
#endif

#ifndef ERROR_LOG_OSTREAM
#define ERROR_LOG_OSTREAM ex::CoutBuffer()
#endif

#if defined(_DEBUG) || defined(DEBUG)
#define DebugPrint(s) \
{ \
    DEBUG_LOG_OSTREAM << DEBUG_LOG_PREFIX << (s) << ex::endl \
        << "    " << __FILE__ << " line " << __LINE__ << ": " << __FUNCTION__ \
        << ex::endl; \
}

#define DebugLog(s) \
{ \
    DEBUG_LOG_OSTREAM << DEBUG_LOG_PREFIX << (s) << ex::endl; \
}

#else
#define DebugPrint(s)
#define DebugLog(s)
#define DebugLogNoContext(s)
#endif

#define WarnLog(s) \
{ \
    WARN_LOG_OSTREAM << WARN_LOG_PREFIX << (s) << ex::endl; \
}

#define ErrorLog(s) \
{ \
    ERROR_LOG_OSTREAM << ERROR_LOG_PREFIX << (s) << ex::endl; \
}

#ifndef DEBUG_LOG_STREAM_OSTREAM
#define DEBUG_LOG_STREAM_OSTREAM thread_safe_cout
#endif

namespace ex
{

EXLIB_API
void LogAttempt(
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription);

EXLIB_API
void LogSuccess(
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription);

EXLIB_API
void LogFilter(
    void (*logFunction)(const char *, UInt32, const std::string &),
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription,
    bool alwaysLog);

/*
EXLIB_API
void LogFilter(
    void (*logFunction)(const char *, UInt32, const std::string &),
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription,
    bool (*)(void) alwaysLogFunction)
{
    LogFilter(
        logFunction,
        sourceCodeFile,
        sourceCodeLine,
        operationDescription,
        alwaysLogFunction());
}
*/

template <typename RESULT_T>
std::string CreateFailureMessage(
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription,
    RESULT_T result)
{
    bool operationHasDescription = !operationDescription.empty();
    return operationHasDescription
        ? ex::format(
            "%s line %d: Error %d %s.",
            sourceCodeFile,
            sourceCodeLine,
            result,
            operationDescription.c_str())
        : ex::format(
            "%s line %d: Error %d.",
            sourceCodeFile,
            sourceCodeLine,
            result);
}

EXLIB_API
std::string CreateFailureMessage(
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription,
    const std::string &errorDetail = "");

template <typename RESULT_T, class LOG_FUNCTION_T>
void LogFailure(
    LOG_FUNCTION_T logFunction,
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription,
    RESULT_T result)
{
    std::string message = CreateFailureMessage(
        sourceCodeFile,
        sourceCodeLine,
        operationDescription,
        result);
    logFunction(message);
}

template <typename EXCEPTION_T>
void LogFailureAndThrow(const std::string &message)
{
    ErrorLog(message);
    throw EXCEPTION_T(message);
}

template <typename EXCEPTION_T>
void LogFailureAndThrow(
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription,
    const std::string &errorDetail = "")
{
    LogFailureAndThrow<EXCEPTION_T>(CreateFailureMessage(
        sourceCodeFile,
        sourceCodeLine,
        operationDescription,
        errorDetail));
}

template <typename EXCEPTION_T, typename RESULT_T>
void LogFailureAndThrow(
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription,
    RESULT_T result)
{
    LogFailureAndThrow<EXCEPTION_T>(CreateFailureMessage(
        sourceCodeFile,
        sourceCodeLine,
        operationDescription,
        result));
}

/**
Throws an exception of type EXCEPTION_T if the value of `result` is non-zero;
otherwise does nothing.

If the caller passes a non-empty string for `operationDescription`, this
function includes the string in the exception error message that it passes
to the the exception constructor. The value of `operationDescription`
should be an uncapitalized gerund phrase that describes the operation for
which the result is being checked. For example:

     VerifyZero<MyException>(
         ReadAudioFile(filename, &buffer),
         __FILE__,
         __LINE__,
         "reading audio file");

If `ReadAudioFile` returns 38, this function constructs an instance of
class `MyException`, passing the single `std::string` parameter,
`"audio_file_reader.cpp line 121: Error 38 reading audio file."` to its
constructor. (The file name and line number will vary based on the context.)

@param RESULT_T result A numeric value that indicates success if 0; otherwise
    indicates failure.
@param const std::string & file The source file that is calling this function.
@param UInt32 line The line number in the source file where the call is made.
@param const std::string & operationDescription An uncapitalized gerund
    phrase that describes the operation for which the result is being
    checked.
@tparam EXCEPTION_T The class to throw.
@tparam RESULT_T A numeric data type; presumed to be the return value of an
    operation for which a value of `0` indicates success and any other
    value indicates failure.
*/
template <typename EXCEPTION_T, typename RESULT_T>
void
VerifyZero(
    RESULT_T result,
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription,
    bool alwaysLog = false)
{
    if (result == 0)
    {
        LogFilter(
            &LogSuccess,
            sourceCodeFile,
            sourceCodeLine,
            operationDescription,
            alwaysLog);
    }
    else
    {
        LogFailureAndThrow<EXCEPTION_T>(
            sourceCodeFile,
            sourceCodeLine,
            operationDescription,
            result);
    }
}

#define VERIFY_ZERO_VERBOSE(operationDescription, result, exceptionType) \
    ex::VerifyZero<exceptionType>( \
        result, \
        __FILE__, \
        __LINE__, \
        operationDescription, \
        true)

#define VERIFY_ZERO(operationDescription, result, exceptionType) \
    ex::VerifyZero<exceptionType>( \
        result, \
        __FILE__, \
        __LINE__, \
        operationDescription, \
        false)

template <typename EXCEPTION_T, class OPERATION_FUNCTION_T>
void
VerifyZeroLambda(
    OPERATION_FUNCTION_T verificationLambda,
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription,
    bool alwaysLog = false)
{
    LogFilter(
        &LogAttempt,
        sourceCodeFile,
        sourceCodeLine,
        operationDescription,
        alwaysLog);

    try
    {
        VerifyZero<EXCEPTION_T>(
            verificationLambda(),
            sourceCodeFile,
            sourceCodeLine,
            operationDescription,
            alwaysLog);
    }
    catch (std::exception &ex)
    {
        LogFailureAndThrow<EXCEPTION_T>(
            sourceCodeFile,
            sourceCodeLine,
            operationDescription,
            ex.what());
    }
    catch (std::string &ex)
    {
        LogFailureAndThrow<EXCEPTION_T>(
            sourceCodeFile,
            sourceCodeLine,
            operationDescription,
            ex);
    }
}

#define VERIFY_ZERO_LAMBDA_VERBOSE(operationDescription, operationLambda, exceptionType) \
    ex::VerifyZeroLambda<exceptionType>( \
        operationLambda, \
        __FILE__, \
        __LINE__, \
        operationDescription, \
        true)

#define VERIFY_ZERO_LAMBDA(operationDescription, operationLambda, exceptionType) \
    ex::VerifyZeroLambda<exceptionType>( \
        operationLambda, \
        __FILE__, \
        __LINE__, \
        operationDescription, \
        false)

/*
template <
    typename EXCEPTION_T,
    class VERIFICATION_OPERATION_FUNCTION_T,
    class ALWAYS_LOG_FUNCTION_T>
void
VerifyZeroLambda(
    VERIFICATION_OPERATION_FUNCTION_T verificationLambda,
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription,
    ALWAYS_LOG_FUNCTION_T alwaysLogLambda)
    {
    VerifyZeroLambda(
        verificationLambda,
        sourceCodeFile,
        sourceCodeLine,
        operationDescription,
        alwaysLogLambda());
}
*/

template <typename RESULT_T>
std::string ResultCodeToSpelledError(RESULT_T result)
{
    const char *bigEndianBuffer = reinterpret_cast<const char *>(&result);
    return format(
        "%c%c%c%c",
        bigEndianBuffer[3],
        bigEndianBuffer[2],
        bigEndianBuffer[1],
        bigEndianBuffer[0]);
}

template <typename EXCEPTION_T, typename RESULT_T>
void
VerifyZeroSpelledError(
    RESULT_T result,
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription,
    bool alwaysLog = false)
{
    bool operationHasDescription = !operationDescription.empty();

    if (result == 0)
    {
        LogFilter(
            &LogSuccess,
            sourceCodeFile,
            sourceCodeLine,
            operationDescription,
            alwaysLog);
    }
    else
    {
        std::string spelledError = ResultCodeToSpelledError(result);
        std::string message = operationHasDescription
            ? ex::format(
                "%s line %d: Error %d, %s, %s.", 
                sourceCodeFile,
                sourceCodeLine,
                result, 
                spelledError.c_str(), 
                operationDescription.c_str())
            : ex::format(
                "%s line %d: Error %d, %s",
                sourceCodeFile,
                sourceCodeLine,
                result,
                spelledError.c_str());

        ErrorLog(message);
        throw EXCEPTION_T(message);
    }
}

#define VERIFY_ZERO_SPELLED_VERBOSE( \
    operationDescription, \
    result, \
    exceptionType) \
    ex::VerifyZeroSpelledError<exceptionType>( \
        result, \
        __FILE__, \
        __LINE__, \
        operationDescription, \
        true)

#define VERIFY_ZERO_SPELLED( \
    operationDescription, \
    result, \
    exceptionType) \
    ex::VerifyZeroSpelledError<exceptionType>( \
        result, \
        __FILE__, \
        __LINE__, \
        operationDescription, \
        false)

template <typename EXCEPTION_T, class OPERATION_FUNCTION_T>
void
VerifyZeroSpelledErrorLambda(
    OPERATION_FUNCTION_T operationFunction,
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription,
    bool alwaysLog = false)
{
    LogFilter(
        &LogAttempt,
        sourceCodeFile,
        sourceCodeLine,
        operationDescription,
        alwaysLog);

    try
    {
        VerifyZeroSpelledError<EXCEPTION_T>(
            operationFunction(),
            sourceCodeFile,
            sourceCodeLine,
            operationDescription,
            alwaysLog);
    } catch (EXCEPTION_T &)
    {
        throw;
    }
    catch (std::exception &ex)
    {
        LogFailureAndThrow<EXCEPTION_T>(
            sourceCodeFile,
            sourceCodeLine,
            operationDescription,
            ex.what());
    }
    catch (std::string &ex)
    {
        LogFailureAndThrow<EXCEPTION_T>(
            sourceCodeFile,
            sourceCodeLine,
            operationDescription,
            ex);
    }
}

#define VERIFY_ZERO_SPELLED_LAMBDA_VERBOSE( \
    operationDescription, \
    operationFunction, \
    exceptionType) \
    ex::VerifyZeroSpelledErrorLambda<exceptionType>( \
        operationFunction, \
        __FILE__, \
        __LINE__, \
        operationDescription, \
        true)

#define VERIFY_ZERO_SPELLED_LAMBDA( \
    operationDescription, \
    operationFunction, \
    exceptionType) \
    ex::VerifyZeroSpelledErrorLambda<exceptionType>( \
        operationFunction, \
        __FILE__, \
        __LINE__, \
        operationDescription, \
        false)

template <typename EXCEPTION_T>
void Verify(
    bool result,
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription,
    bool alwaysLog = false)
{
    if (result)
    {
        LogFilter(
            &LogSuccess,
            sourceCodeFile,
            sourceCodeLine,
            operationDescription,
            alwaysLog);
        return;
    }
    else
    {
        LogFailureAndThrow<EXCEPTION_T>(
            sourceCodeFile,
            sourceCodeLine,
            operationDescription);
    }
}

#define VERIFY_VERBOSE(operationDescription, result, exceptionType) \
    ex::Verify<exceptionType>(result, __FILE__, __LINE__, operationDescription, true)

#define VERIFY(operationDescription, result, exceptionType) \
    ex::Verify<exceptionType>(result, __FILE__, __LINE__, operationDescription, false)

template <typename EXCEPTION_T, class OPERATION_FUNCTION_T>
void VerifyLambda(
    OPERATION_FUNCTION_T operationFunction,
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription,
    bool alwaysLog = false)
{
    LogFilter(
        &LogAttempt,
        sourceCodeFile,
        sourceCodeLine,
        operationDescription,
        alwaysLog);

    try
    {
        if (operationFunction())
        {
            LogFilter(
                &LogSuccess,
                sourceCodeFile,
                sourceCodeLine,
                operationDescription,
                alwaysLog);
            return;
        }
        else
        {
            LogFailureAndThrow<EXCEPTION_T>(
                sourceCodeFile,
                sourceCodeLine,
                operationDescription);
        }
    }
    catch (std::exception &ex)
    {
        LogFailureAndThrow<EXCEPTION_T>(
            sourceCodeFile,
            sourceCodeLine,
            operationDescription,
            ex.what());
    }
    catch (std::string &ex)
    {
        LogFailureAndThrow<EXCEPTION_T>(
            sourceCodeFile,
            sourceCodeLine,
            operationDescription,
            ex);
    }
}

#define VERIFY_LAMBDA_VERBOSE( \
    operationDescription, \
    operationFunction, \
    exceptionType) \
    ex::VerifyLambda<exceptionType>( \
        operationFunction, \
        __FILE__, \
        __LINE__, \
        operationDescription, \
        true)

#define VERIFY_LAMBDA( \
    operationDescription, \
    operationFunction, \
    exceptionType) \
    ex::VerifyLambda<exceptionType>( \
        operationFunction, \
        __FILE__, \
        __LINE__, \
        operationDescription, \
        false)

template <
    class LOG_FUNCTION_T,
    class OPERATION_FUNCTION_T>
bool LogTry(
    LOG_FUNCTION_T logFunction,
    OPERATION_FUNCTION_T operationLambda,
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription,
    bool alwaysLog = false)
{
    LogFilter(
        &LogAttempt,
        sourceCodeFile,
        sourceCodeLine,
        operationDescription,
        alwaysLog);

    try
    {
        operationLambda();
        LogFilter(
            &LogSuccess,
            sourceCodeFile,
            sourceCodeLine,
            operationDescription,
            alwaysLog);
        return true;
    }
    catch (std::exception &e)
    {
        LogFailure(
            logFunction,
            sourceCodeFile,
            sourceCodeLine,
            operationDescription,
            e.what());
    }
    catch (std::string &e)
    {
        LogFailure(
            logFunction,
            sourceCodeFile,
            sourceCodeLine,
            operationDescription,
            e);
    }

    return false;
}

#define TRY_LOG_VERBOSE(logger, operationDescription, operationLambda) \
    ex::LogTry( \
        [](const std::string &message) { logger(message); }, \
        operationLambda, \
        __FILE__, \
        __LINE__, \
        operationDescription, \
        true)

#ifdef DEBUG
#define TRY_LOG(logger, operationDescription, operationLambda) \
    ex::LogTry( \
        [](const std::string &message) { logger(message); }, \
        operationLambda, \
        __FILE__, \
        __LINE__, \
        operationDescription, \
        false)
#else
#define TRY_LOG(logger, operationDescription, operationLambda) \
    ex::LogTry( \
        [](const std::string &) {}, \
        operationLambda, \
        __FILE__, \
        __LINE__, \
        operationDescription, \
        false)
#endif

template<typename OPERATION_FUNCTION_T>
std::result_of<OPERATION_FUNCTION_T> func_logger(
    OPERATION_FUNCTION_T fn, 
    const std::string& func_name,
    const std::string& caller_name="")
{
    ex::CoutBuffer cb;
    cb << caller_name << " calling " << func_name << ex::endl;
    cb.flush();
    return fn();
}

}
