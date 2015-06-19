/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef FILECACHE_H
#define FILECACHE_H

#include "types.h"
#include "TokenCache.h"
#include "rmsauthExport.h"

namespace rmsauth {

class RMSAUTH_EXPORT FileCache : public TokenCache
{
protected:
    static const String Tag() { static const String tag = "FileCache"; return tag;}
    String cacheFilePath_;
    static Mutex fileLock_;

public:
    FileCache(const String& filePath = "");
    void clear() override;

protected:
    virtual void readCache();
    virtual void writeCache();

private:
    virtual void onBeforeAccess(const TokenCacheNotificationArgs& args) override;
    virtual void onAfterAccess(const TokenCacheNotificationArgs& args) override;
    virtual const String getCacheName() const override {return FileCache::Tag();}
};

} // namespace rmsauth {

#endif // FILECACHE_H
