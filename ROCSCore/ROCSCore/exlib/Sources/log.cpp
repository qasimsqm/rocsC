#include "exlib/log.h"

namespace ex
{

static inline
std::string PhraseToSentence(const std::string &phrase)
{
    return static_cast<char>(toupper(phrase[0])) + phrase.substr(1) + ".";
}

#ifdef DEBUG
EXLIB_API
void LogAttempt(
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription)
{
    bool operationHasDescription = !operationDescription.empty();
    DebugLog(ex::format(
        "%s line %d: %s",
        sourceCodeFile,
        sourceCodeLine,
        (operationHasDescription
            ? PhraseToSentence(operationDescription).c_str()
            : "Executing.")));
}

EXLIB_API
void LogSuccess(
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription)
{
    bool operationHasDescription = !operationDescription.empty();
    DebugLog(ex::format(
        "%s line %d: Succeeded %s.",
        sourceCodeFile,
        sourceCodeLine,
        operationHasDescription
            ? operationDescription.c_str()
            : "executing"));
}
#else
EXLIB_API
void LogAttempt(
    const char *,
    UInt32,
    const std::string &)
{}

EXLIB_API
void LogSuccess(
    const char *,
    UInt32,
    const std::string &)
{}
#endif

EXLIB_API
void LogFilter(
    void (*logFunction)(const char *, UInt32, const std::string &),
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription,
    bool alwaysLog)
{
#ifdef EXLIB_VERBOSE_LOGGING
    logFunction(sourceCodeFile, sourceCodeLine, operationDescription);
#else
    if (alwaysLog)
    {
        logFunction(sourceCodeFile, sourceCodeLine, operationDescription);
    }
#endif
}

EXLIB_API
std::string CreateFailureMessage(
    const char *sourceCodeFile,
    UInt32 sourceCodeLine,
    const std::string &operationDescription,
    const std::string &errorDetail)
{
    bool operationHasDescription = !operationDescription.empty();
    std::string message = operationHasDescription
        ? ex::format(
            "%s line %d: Error.",
            sourceCodeFile,
            sourceCodeLine)
        : ex::format(
            "%s line %d: Error %s.",
            sourceCodeFile,
            sourceCodeLine,
            operationDescription.c_str());

    if (!errorDetail.empty())
    {
        message = message + "\n        " + errorDetail;
    }

    return message;
}

}
