#ifndef FILEAPIEXPORT_H
#define FILEAPIEXPORT_H

#ifdef _WIN32
  #define DLL_LOCAL_FILE
  #ifdef FILE_LIBRARY
    #define DLL_PUBLIC_FILE __declspec(dllexport)
  #else
    #define DLL_PUBLIC_FILE
  #endif  // ifdef FILE_LIBRARY
#else
  #if __GNUC__ >= 4
    #define DLL_PUBLIC_FILE __attribute__ ((visibility ("default")))
  #else
    #define DLL_PUBLIC_FILE
  #endif
#endif //ifdef _WIN32

#endif // FILEAPIEXPORT_H
