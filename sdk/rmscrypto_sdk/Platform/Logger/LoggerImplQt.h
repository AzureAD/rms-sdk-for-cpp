/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _CRYPTO_STREAMS_LIB_LOGGERQTIMPL_H_
#define _CRYPTO_STREAMS_LIB_LOGGERQTIMPL_H_

#include "Logger.h"
#include <fstream>

class LoggerCryptoImplQt : public LoggerCrypto {
public:
    ~LoggerCryptoImplQt();

protected:
    virtual void append(const std::string& prefix, const std::string& record) override;

private:
    LoggerCryptoImplQt();
    friend class LoggerCrypto;
    std::ofstream stream_;
};

#endif // _CRYPTO_STREAMS_LIB_LOGGERQTIMPL_H_
