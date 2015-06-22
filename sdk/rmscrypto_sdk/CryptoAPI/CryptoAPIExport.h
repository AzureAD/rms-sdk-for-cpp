/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_EXPORT_H_
#define _RMS_LIB_EXPORT_H_

// This code produces too many warnings

#if defined _WIN32 || defined __CYGWIN__
  #ifdef RMS_CRYPTO_LIBRARY
    #ifdef __GNUC__
      #define DLL_PUBLIC_CRYPTO __attribute__ ((dllexport))
    #else
      #define DLL_PUBLIC_CRYPTO __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define DLL_PUBLIC_CRYPTO __attribute__ ((dllimport))
    #else
      #define DLL_PUBLIC_CRYPTO __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define DLL_PUBLIC_CRYPTO __attribute__ ((visibility ("default")))
    #define DLL_LOCAL_CRYPTO  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_PUBLIC_CRYPTO
    #define DLL_LOCAL_CRYPTO
  #endif
#endif

#endif  // _RMS_LIB_EXPORT_H_
