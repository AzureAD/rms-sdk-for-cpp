/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef CRYPTEDSTREAMTESTS_H
#define CRYPTEDSTREAMTESTS_H
#include <QTest>


class CryptedStreamTests : public QObject
{
    Q_OBJECT

private Q_SLOTS:

  void CryptedStreamToMemory_data();
  void CryptedStreamToMemory();
};

#endif // CRYPTEDSTREAMTESTS_H
