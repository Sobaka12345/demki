#pragma once

#include <string>
#include <iostream>

#ifndef __FUNCTION_NAME__
#	ifdef WIN32    //  WINDOWS
#		define __FUNCTION_NAME__ __FUNCTION__
#	else    //*NIX
#		define __FUNCTION_NAME__ __func__
#	endif
#endif

inline void realAssert(std::string assertType,
    std::string file,
    int line,
    std::string funcName,
    std::string expr,
    bool exprRes,
    std::string message)
{
    if (!exprRes)
    {
        std::cerr << "\t" + assertType + " IN " + file + " : LINE " + std::to_string(line) +
                "\n\tFUNC: " + funcName +
                "()"
                "\n\tEXPRESSION: " +
                expr + (message.empty() ? "\n" : "\n\tMESSAGE: " + message)
                  << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

#define _ASSERT_2(assertType, x, y) \
    realAssert(assertType, __FILE__, __LINE__, __FUNCTION_NAME__, #x, bool(x), y)
#define _ASSERT_1(assertType, x) _ASSERT_2(assertType, x, "")

#define FUNC_CHOOSER(_f1, _f2, _f3, _f4, ...) _f4
#define FUNC_RECOMPOSER(argsWithParentheses)  FUNC_CHOOSER argsWithParentheses
#define CHOOSE_FROM_ARG_COUNT(...)            FUNC_RECOMPOSER((__VA_ARGS__, _ASSERT_2, _ASSERT_1, ))
#define MACRO_CHOOSER(...)                    CHOOSE_FROM_ARG_COUNT(__VA_ARGS__())
#define ASSERT_WITH_TYPE(...)                 MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
#define ASSERT(...)                           ASSERT_WITH_TYPE("ASSERT", __VA_ARGS__)

#ifdef NDEBUG

#	define DASSERT(...) (static_cast<void>(0))

#else

#	define DASSERT(...) ASSERT_WITH_TYPE("DEBUG_ASSERT", __VA_ARGS__)

#endif
