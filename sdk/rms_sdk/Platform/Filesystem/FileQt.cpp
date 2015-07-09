/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifdef QTFRAMEWORK
#include <QDir>
#include <QTextStream>
#include <iostream>
#include <cstdio>
#include "FileQt.h"
#include "../Logger/Logger.h"
using namespace std;
using namespace rmscore::platform::logger;

namespace rmscore {
namespace platform {
namespace filesystem {
shared_ptr<IFile>IFile::Create(const string& path, FileOpenModes mode)
{
  // first check if directory exists
  QString directory = QString::fromStdString(path);
  auto    idx       = directory.lastIndexOf(QChar('/'));

  if ((idx > 0) && (idx < directory.size())) {
    directory.resize(idx + 1);

    if (!QFile::exists(directory)) {
      QDir dir;
      dir.mkpath(directory);
    }
  }

  auto p_file = make_shared<FileQt>(path);

  if (!p_file->open(mode))
  {
    return nullptr;
  }

  return p_file;
}

FileQt::FileQt(const string& path)
  :
  impl_(path.c_str())
{}

bool FileQt::open(FileOpenModes mode)
{
  // FIXME.shch: added QIODevice::Text
  bool ok = this->impl_.open(FileQt::toQtMode(mode) | QIODevice::Text);

  if (!ok)
  {
    Logger::Error("Cant't open file '%s'",
                  this->impl_.fileName().toStdString().data());
  }
  return ok;
}

size_t FileQt::Read(common::CharArray& data, size_t len)
{
  data.resize(len);
  QDataStream out(&this->impl_);
  auto readBytes = (size_t)out.readRawData(&data[0], (int)len);

  if (readBytes < len) data.resize(readBytes);
  return readBytes;
}

size_t FileQt::Write(const char *data, size_t len)
{
  QDataStream out(&this->impl_);

  return (size_t)out.writeRawData(data, (int)len);
}

void FileQt::Clear()
{
  auto openmode = this->impl_.openMode();

  this->impl_.close();
  this->impl_.open(openmode | QIODevice::Truncate);
}

void FileQt::Close()
{
  this->impl_.close();
}

string FileQt::ReadAllAsText()
{
  auto data = this->impl_.readAll();

  return string(data);
}

void FileQt::AppendText(const common::ByteArray& text)
{
  QTextStream out(&this->impl_);

  out << QByteArray(reinterpret_cast<const char *>(text.data()),
                    static_cast<int>(text.size()));
}

size_t FileQt::GetSize()
{
  return (size_t) this->impl_.size();
}

}
}
} // namespace rmscore { namespace platform { namespace filesystem {
#endif // ifdef QTFRAMEWORK
