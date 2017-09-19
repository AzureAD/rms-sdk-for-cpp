#include <algorithm>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <memory>
#include "zip_file.h"
#include <utility>

using std::vector;
using std::string;
using std::shared_ptr;
using std::unique_ptr;
using std::map;

namespace mip {
namespace file {

ZipFile::ZipFile(shared_ptr<IStream> inputStream) {
  gsf_init();

  unique_ptr<GsfInput, GsfInput_deleter> gsfInputStream(gsf_input_istream_new(inputStream.get()));

  GError *error = nullptr;
  mGsfZipStream = unique_ptr<GsfInfile, GsfInfile_deleter> (gsf_infile_zip_new(gsfInputStream.get(), &error));

  if (!mGsfZipStream) {
    unique_ptr<GError, GError_deleter> err(error);
    throw ZipException("Not a valid zip file", err->message);
  }
}

string ZipFile::GetEntry(const std::string& entryPath) {
  unique_ptr<gchar*, GsfStrSplit_deleter> elems(g_strsplit(entryPath.c_str(), "/", -1));

  auto child = gsf_infile_child_by_aname(mGsfZipStream.get(), (const char**)elems.get());
  if (child == nullptr)
    throw ZipEntryNotFoundException("Entry not found", entryPath);

  unique_ptr<GsfInput, GsfInput_deleter> entry(child);

  gsf_off_t size = gsf_input_size(entry.get());

  auto buf = gsf_input_read(entry.get(), size, nullptr);
  if (!buf)
    throw ZipException("Failed reading entry", entryPath);

  return string((const char*)buf, size);
}

void ZipFile::SetEntry(const std::string& entryPath, const std::string& content) {
  // TODO : Implement.
}

void ZipFile::Commit(std::shared_ptr<IStream> outputStream) {
  // TODO : Implement.
}

} // namespace file
} // namespace mip
