#pragma once

#include "exlib/targetver.h"

#ifdef _WIN32

// The following ifdef block is the standard way of creating macros which make
// exporting from a DLL simpler. All files within this DLL are compiled with
// the EXLIB_EXPORTS symbol defined on the command line. This symbol should not
// be defined on any project that uses this DLL. This way any other project
// whose source files include this file see EXLIB_API functions as being
// imported from a DLL, whereas this DLL sees symbols defined with this macro
// as being exported.
#ifdef EXLIB_EXPORTS
#define EXLIB_API __declspec(dllexport)
#define EXLIB_TEMPLATE_DECLSPEC(...) template class EXLIB_API __VA_ARGS__ 
#else
#define EXLIB_API __declspec(dllimport)
#define EXLIB_TEMPLATE_DECLSPEC(...) extern template class EXLIB_API __VA_ARGS__ 
#endif // EXLIB_API

#include "exlib/win32/nanosleep.h"

#else // !_WIN32
#define EXLIB_API
#define EXLIB_TEMPLATE_DECLSPEC(...)
#endif // _WIN32
