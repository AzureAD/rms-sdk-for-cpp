/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef FILECACHEENCRYPTED_H
#define FILECACHEENCRYPTED_H

#include "types.h"
#include "FileCache.h"
#include "rmsauthExport.h"

namespace rmsauth {

class RMSAUTH_EXPORT FileCacheEncrypted : public FileCache
{
    static const String Tag() { static const String tag = "FileCacheEncrypted"; return tag;}

public:
    FileCacheEncrypted(const String& filePath = "");

protected:
    virtual void readCache() override;
    virtual void writeCache() override;

private:
    virtual const String getCacheName() const override {return FileCacheEncrypted::Tag();}
};

} // namespace rmsauth {

#endif // FILECACHEENCRYPTED_H
