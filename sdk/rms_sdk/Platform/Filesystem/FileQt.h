/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef FILEQT
#define FILEQT
#include "IFile.h"
#include <QFile>
#include "../../ModernAPI/RMSExceptions.h"

namespace rmscore {
namespace platform {
namespace filesystem {
class FileQt : public IFile {
public:

  virtual size_t      Read(common::CharArray& data,
                           size_t             len) override;
  virtual size_t      Write(const char *data,
                            size_t      len) override;

  virtual void        Clear() override;
  virtual void        Close() override;
  virtual std::string ReadAllAsText() override;
  virtual void        AppendText(const common::ByteArray& text) override;

  virtual size_t      GetSize() override;

private:

  QFile impl_;

  static QIODevice::OpenModeFlag toQtMode(FileOpenModes mode)
  {
    switch (mode)
    {
    case FILE_OPEN_WRITE: return QIODevice::WriteOnly;

    case FILE_OPEN_READ: return QIODevice::ReadOnly;

    default: throw exceptions::RMSInvalidArgumentException("unknown FileOpenMode");
    }
  }

public:

  FileQt(const std::string& path);
  bool open(FileOpenModes mode);
};
}
}
} // namespace rmscore { namespace platform { namespace filesystem {

#endif // FILEQT
