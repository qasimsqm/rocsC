#pragma once

#ifdef _WIN32

// Exclude Windows cryptography, DDE, RPC, shell, and Winsock.
#define WIN32_LEAN_AND_MEAN

// Target Windows XP Service Pack 3.
#define NTDDI_VERSION NTDDI_WINXPSP3

// Target Windows XP.
#define _WIN32_WINNT _WIN32_WINNT_WINXP

#include <SDKDDKVer.h>

#endif // _WIN32

#ifdef _MSC_VER
#define MSC_DISABLE_WARNING(n) \
	__pragma(warning(push)) \
	__pragma(warning(disable: n))
#define MSC_RESTORE_WARNING(n) __pragma(warning(pop))
#else
// Resolve to nothing.
#define MSC_DISABLE_WARNING(n)
#define MSC_RESTORE_WARNING(n)
#endif

// Disable the "unknown pragma" warning.
MSC_DISABLE_WARNING(4068)
