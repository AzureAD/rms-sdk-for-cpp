/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef IFILESYSTEM
#define IFILESYSTEM

#include<memory>
#include<string>
#include<vector>
#include"../../Common/CommonTypes.h"
#include"IFile.h"

namespace rmscore { namespace platform { namespace filesystem {

class IFileSystem
{
public:
    virtual std::shared_ptr<IFile> OpenLocalStorageFile(const std::string& filePath, FileOpenModes mode) = 0;
    virtual std::shared_ptr<IFile> OpenProtectedLocalStorageFile(const std::string& filePath, FileOpenModes mode) = 0;

    virtual void DeleteLocalStorageFile(const std::string& filePath) = 0;

    virtual std::vector<std::string> QueryLocalStorageFiles(const std::string& folder, const std::string& pattern) = 0;

public:
    static std::shared_ptr<IFileSystem> Create();
    static void CreateDirectory(const std::string& dirPath);
};

}}} // namespace rmscore { namespace platform { namespace filesystem {

#endif // IFILESYSTEM

