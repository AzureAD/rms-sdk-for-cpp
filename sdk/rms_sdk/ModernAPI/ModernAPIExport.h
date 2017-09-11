/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_CRYPTO_EXPORT_H_
#define _RMS_CRYPTO_EXPORT_H_

#ifdef _WIN32
  #define DLL_LOCAL_RMS 
  #ifdef RMS_LIBRARY
    #define DLL_PUBLIC_RMS __declspec(dllexport)
  #else
    #define DLL_PUBLIC_RMS
  #endif  // ifdef UPE_IMPLEMENTATION
#endif

#endif  // _RMS_CRYPTO_EXPORT_H_
