/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include "CryptedStreamTests.h"
#include "../CryptoAPI/CryptoAPI.h"
#include "../CryptoAPI/RMSCryptoExceptions.h"
#include "../../rms_sdk/Common/CommonTypes.h"
#include "gtest/gtest.h"

using namespace std;
using std::string;
using std::tuple;
using std::vector;

#define ByteArray rmscore::common::ByteArray
#define TestData vector<tuple<ByteArray, ByteArray, ByteArray, ByteArray, uint8_t>>

TEST (CryptedStreamTests, CryptedStreamToMemory) {
  shared_ptr<stringstream> backingBuffer = make_shared<stringstream>(
    ios::in | ios::out | ios::binary);
  vector<uint8_t> key(16);

  // Test Data consists of:
  // ByteArray aesKey
  // ByteArray plainData
  // ByteArray iv
  // ByteArray encryptedData
  // uint8_t   algorithm
  TestData test_data {
    std::make_tuple(
        ByteArray { 0x7b, 0xdd, 0xca, 0x3c, 0x7f, 0xf9, 0xbd, 0xc8, 0xee, 0xa7, 0xa8, 0xeb, 0x63, 0xd0,
            0x1f, 0x11 },
        ByteArray { 0x50, 0x6C, 0x61, 0x69, 0x6E, 0x20, 0x54, 0x65, 0x78, 0x74 },
        ByteArray { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00 },
        ByteArray { 0xea, 0x21, 0xb4, 0x8f, 0x53, 0x0b, 0xe2, 0x3b, 0x6d, 0x65, 0x4d, 0xb9, 0x30, 0x2d,
            0xd1, 0xff },
        rmscrypto::api::CIPHER_MODE_CBC4K
    )
  };
  for(TestData::iterator it = test_data.begin(); it != test_data.end(); it++) {
    try {
      ByteArray qKey  = std::get<0>(*it);
      ByteArray qData = std::get<1>(*it);
      ByteArray qIV = std::get<2>(*it);
      ByteArray qEncryptedData = std::get<3>(*it);
      rmscrypto::api::CipherMode algo = static_cast<rmscrypto::api::CipherMode>(std::get<4>(*it));

      memcpy(key.data(), qKey.data(), key.size());

      auto backingStream =
        rmscrypto::api::CreateStreamFromStdStream(static_pointer_cast<iostream>(
                                                    backingBuffer));

      auto cryptoStream = rmscrypto::api::CreateCryptoStream(
        algo, key, backingStream);

      cryptoStream->Write(reinterpret_cast<const uint8_t *>(qData.data()),
          qData.size());
      cryptoStream->Flush();

      auto res = backingBuffer->str();
      EXPECT_EQ(
        static_cast<int>(
          res.length()), qEncryptedData.size()) << "Invalid encrypted size!";
      EXPECT_EQ(memcmp(res.data(), qEncryptedData.data(), res.length()), 0) <<
              "Invalid encrypted data!";

      cryptoStream->Seek(0);
      ByteArray dst(qData.size(), 0x00);
      auto read = cryptoStream->Read(
        reinterpret_cast<uint8_t *>(dst.data()), dst.size());
      EXPECT_EQ(read, dst.size()) << "Invalid decrypted size!";
      EXPECT_EQ(memcmp(dst.data(), qData.data(), dst.size()), 0) <<
              "Invalid decrypted data!";
    } catch (const rmscrypto::exceptions::RMSCryptoException& e) {
      FAIL() << e.what() << ", file: " << __FILE__ << ", line: " << __LINE__;
    }
  }
}
