#ifndef COMMON_PLATFORM_H
#define COMMON_PLATFORM_H

#if defined(_WIN32)
#define PLATFORM_WIN
#if defined(__cplusplus_winrt)
#include <winapifamily.h>
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) &&                           \
    !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#define PLATFORM_UWP
#endif // WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
#endif // defined(__cplusplus_winrt)
#elif defined(__APPLE__)
#define PLATFORM_TYPE_APPLE
#elif defined(__ANDROID__)
#define PLATFORM_ANDROID
#elif defined(__linux__)
#define PLATFORM_LINUX
#endif
#if !defined(_WIN32)
#define PLATFORM_TYPE_UNIX
#endif

#endif // COMMON_PLATFORM_H
