/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef IFILESYSTEMQTIMPL
#define IFILESYSTEMQTIMPL
#include "IFileSystem.h"
#include "../../Common/CommonTypes.h"

namespace rmscore { namespace platform { namespace filesystem {

class FileSystemQt : public IFileSystem
{
private:
    virtual std::shared_ptr<IFile> OpenLocalStorageFile(const std::string& filePath, FileOpenModes mode) override;
    virtual std::shared_ptr<IFile> OpenProtectedLocalStorageFile(const std::string& filePath, FileOpenModes mode) override;

    virtual void DeleteLocalStorageFile(const std::string& filePath) override;

    virtual common::StringArray QueryLocalStorageFiles(const std::string& folder, const std::string& pattern) override;
};

}}} // namespace rmscore { namespace platform { namespace filesystem {

#endif // IFILESYSTEMQTIMPL

