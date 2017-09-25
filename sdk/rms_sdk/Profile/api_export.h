/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef PROFILE_API_EXPORT_H_
#define PROFILE_API_EXPORT_H_

#ifdef _WIN32
  #define DLL_LOCAL_RMS 
  #ifdef RMS_LIBRARY
    #define DLL_PUBLIC_RMS __declspec(dllexport)
  #else
    #define DLL_PUBLIC_RMS
  #endif  
#else 
  #if __GNUC__ >= 4 
    #define DLL_PUBLIC_RMS __attribute__ ((visibility ("default"))) 
    #define DLL_LOCAL_RMS  __attribute__ ((visibility ("hidden"))) 
  #else 
    #define DLL_PUBLIC_RMS 
    #define DLL_LOCAL_RMS 
  #endif 
#endif

#endif  // PROFILE_API_EXPORT_H_
