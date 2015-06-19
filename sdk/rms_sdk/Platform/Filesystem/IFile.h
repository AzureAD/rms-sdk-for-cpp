/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef IFILE
#define IFILE
#include <memory>
#include<string>
#include<fstream>
#include"../../Common/CommonTypes.h"

namespace rmscore { namespace platform { namespace filesystem {

enum FileOpenModes
{
    FILE_OPEN_WRITE,
    FILE_OPEN_READ
};

class IFile
{
public:
    virtual size_t Read(common::CharArray& data, size_t len) = 0;
    virtual size_t Write(const char* data, size_t len) = 0;

    virtual void Clear() = 0;
    virtual void Close() = 0;
    virtual std::string ReadAllAsText() = 0;
    virtual void AppendText(const common::ByteArray& text) = 0;

    virtual size_t GetSize() = 0;

public:
    static std::shared_ptr<IFile> Create(const std::string& path, FileOpenModes mode);
};

}}} // namespace rmscore { namespace platform { namespace filesystem {
#endif // IFILE

