#ifdef SDKWRAPPER_EXPORTS
#define SDKWRAPPER_API __declspec(dllexport)
#else
#define SDKWRAPPER_API __declspec(dllimport)
#endif