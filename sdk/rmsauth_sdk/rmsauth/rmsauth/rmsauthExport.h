/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMSAUTHEXPORT
#define RMSAUTHEXPORT

#include <QtGlobal>
#if defined(RMSAUTH_LIBRARY)
#  define RMSAUTH_EXPORT Q_DECL_EXPORT
#else
#  define RMSAUTH_EXPORT Q_DECL_IMPORT
#endif

#endif  // RMSAUTHEXPORT
