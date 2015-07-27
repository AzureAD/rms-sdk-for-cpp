/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef ENCRYPTEDSTREAMTESTS_H
#define ENCRYPTEDSTREAMTESTS_H
#include <QTest>


class EncryptedStreamTests : public QObject
{
    Q_OBJECT

private Q_SLOTS:

  void EncryptedStreamToMemory_data();
  void EncryptedStreamToMemory();
};

#endif // ENCRYPTEDSTREAMTESTS_H
