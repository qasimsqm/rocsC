#pragma once

#include "exlib/targetver.h"

#ifdef _WIN32

// The following ifdef block is the standard way of creating macros which make
// exporting from a DLL simpler. All files within this DLL are compiled with
// the ROCS_CORE_EXPORTS symbol defined on the command line. This symbol should
// not be defined on any project that uses this DLL. This way any other project
// whose source files include this file see ROCS_CORE_API functions as being
// imported from a DLL, whereas this DLL sees symbols defined with this macro
// as being exported.
#ifdef ROCS_CORE_EXPORTS
#define ROCS_CORE_API __declspec(dllexport)
#define ROCS_CORE_TEMPLATE_DECLSPEC(...) template class ROCS_CORE_API __VA_ARGS__ 
#define ROCS_CORE_STRUCT_TEMPLATE_DECLSPEC(...) template struct ROCS_CORE_API __VA_ARGS__
#else
#define ROCS_CORE_API __declspec(dllimport)
#define ROCS_CORE_TEMPLATE_DECLSPEC(...) extern template class ROCS_CORE_API __VA_ARGS__ 
#define ROCS_CORE_STRUCT_TEMPLATE_DECLSPEC(...) extern template struct ROCS_CORE_API __VA_ARGS__
#endif // ROCS_CORE_EXPORTS

#else // !_WIN32
#define ROCS_CORE_API
#define ROCS_CORE_TEMPLATE_DECLSPEC(...)
#define ROCS_CORE_STRUCT_TEMPLATE_DECLSPEC(...)
#endif // _WIN32

// Usage
// ROCS_CORE_TEMPLATE_DECLSPEC template class ROCS_CORE_API std::vector<int>
// ROCS_CORE_TEMPLATE_DECLSPEC std::list<float>
