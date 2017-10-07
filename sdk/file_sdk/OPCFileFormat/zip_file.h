#ifndef ZIPAPI_H
#define ZIPAPI_H

#include <Common/exceptions.h>
#include <Common/istream.h>
#include <map>
#include <memory>
#include <gsf/gsf.h>

namespace mip {
namespace file {

struct GsfOutput_deleter {
  void operator()(GsfOutput* obj) const {
    if (!gsf_output_is_closed(obj)) {
      gsf_output_close(obj);
    }
    g_object_unref(G_OBJECT(obj));
  }
};

struct GsfOutfile_deleter {
  void operator()(GsfOutfile* obj) const {
    if (!gsf_output_is_closed(GSF_OUTPUT(obj))) {
      gsf_output_close(GSF_OUTPUT(obj));
    }
    g_object_unref(G_OBJECT(obj));
  }
};

struct GsfOutfile_deleter1 {
  void operator()(GsfOutfile* obj) const {
    if (!gsf_output_is_closed(GSF_OUTPUT(obj))) {
      gsf_output_close(GSF_OUTPUT(obj));
    }
    g_object_unref(G_OBJECT(obj));
  }
};

struct GsfInput_deleter {
  void operator()(GsfInput* obj) const {
    g_object_unref(G_OBJECT(obj));
  }
};

struct GsfInfile_deleter {
  void operator()(GsfInfile* obj) const {
    g_object_unref(G_OBJECT(obj));
  }
};

struct GsfStrSplit_deleter {
  void operator()(gchar** obj) const {
    g_strfreev(obj);
  }
};

struct GError_deleter {
  void operator()(GError* obj) const {
    if(obj != nullptr)
      g_error_free(obj);
  }
};

class ZipFile {
public:
  explicit ZipFile(std::shared_ptr<IStream> inputStream);
  std::string GetEntry(const std::string& entryPath) const;
  void SetEntry(const std::string& entryPath, const std::string& content);
  void Commit(std::shared_ptr<IStream> outputStream);

private:
  static void Clone(std::unique_ptr<GsfInfile, GsfInfile_deleter>& in, std::unique_ptr<GsfOutfile, GsfOutfile_deleter>& out, const std::string& path, std::map<std::string, std::string>& map);
  static void CloneDirEntry(GsfInput* input, GsfOutput* output, const std::string& path, std::map<std::string, std::string>& map);
  static void CloneFileEntry(GsfInput* input, GsfOutput* output, const std::string& path, std::map<std::string, std::string>& map);
  static void AddNewEntries(std::unique_ptr<GsfOutfile, GsfOutfile_deleter>& outfile, std::map<std::string, std::string>& entries);

  std::unique_ptr<GsfInfile, GsfInfile_deleter> mGsfZipStream;
  std::map<std::string, std::string> mEntries;
};

class ZipException : public Exception {
public:
  ZipException(const std::string& error, const std::string& message) : Exception(error, message){}
  explicit ZipException(const std::string& error) : ZipException(error, ""){}
};

class ZipEntryNotFoundException : public ZipException {
public:
  ZipEntryNotFoundException(const std::string& error, const std::string& message) : ZipException(error, message){}
  explicit ZipEntryNotFoundException(const std::string& error) : ZipEntryNotFoundException(error, ""){}
};

} // namespace file
} // namespace mip

#endif // ZIPAPI_H

