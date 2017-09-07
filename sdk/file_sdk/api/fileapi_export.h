#ifndef FILEAPIEXPORT_H
#define FILEAPIEXPORT_H

#ifdef _WIN32
  #define DLL_LOCAL_FILE
  #ifdef FILE_LIBRARY
    #define DLL_PUBLIC_FILE __declspec(dllexport)
  #else
    #define DLL_PUBLIC_FILE
  #endif  // ifdef FILE_LIBRARY
#endif

#endif // FILEAPIEXPORT_H
