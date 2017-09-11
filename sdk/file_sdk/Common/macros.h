// Macros for msvc 2012
#ifdef MSVC12
#define THREAD_LOCAL __declspec(thread)
#define SNPRINTF(buf, len, format, ...)  _snprintf(buf, len, format, __VA_ARGS__)
#define NOEXCEPT
// Macros for msvc 2014
#else
#define THREAD_LOCAL thread_local
#define SNPRINTF(buf, len, format, ...) snprintf(buf, len, format, __VA_ARGS__)
#define NOEXCEPT noexcept
#endif
// Platform macros
#ifndef CPPRESTSTRING
#ifdef _WIN32 
// CPP REST SDK String is platform specific.
#define CPPRESTSDKSTRING(str) base::ConvertStringToWString(str)
#else 
// CPP REST SDK String is platform specific.
#define CPPRESTSDKSTRING(str) str
#endif
#endif // CPPRESTSTRING
