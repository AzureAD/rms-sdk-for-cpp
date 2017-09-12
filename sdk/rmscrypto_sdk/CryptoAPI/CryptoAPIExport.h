/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_EXPORT_H_
#define _RMS_LIB_EXPORT_H_

#ifdef _WIN32
  #define DLL_LOCAL_CRYPTO 
  #ifdef RMS_CRYPTO_LIBRARY
    #define DLL_PUBLIC_CRYPTO __declspec(dllexport)
  #else
    #define DLL_PUBLIC_CRYPTO
  #endif  // ifdef UPE_IMPLEMENTATION
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
