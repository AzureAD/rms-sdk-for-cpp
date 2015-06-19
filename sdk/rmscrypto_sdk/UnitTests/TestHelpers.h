/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#define QVERIFY_THROW(expression, ExpectedExceptionType)                        \
  do                                                                            \
  {                                                                             \
    bool caught_ = false;                                                       \
    try { expression; }                                                         \
    catch (ExpectedExceptionType const&) { caught_ = true; }                    \
    catch (...) {}                                                              \
    if (!QTest::qVerify(caught_, # expression ", " # ExpectedExceptionType, "", \
                        __FILE__, __LINE__)) return;                            \
  } while (0)

#endif // TESTHELPERS_H
