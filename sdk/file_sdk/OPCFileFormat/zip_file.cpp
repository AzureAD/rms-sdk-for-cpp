#include <vector>
#include <memory>
#include "zip_file.h"
#include "Common/gsf_input_istream.h"

using std::vector;
using std::string;
using std::shared_ptr;
using std::unique_ptr;
using std::map;


namespace mip {
namespace file {

ZipFile::ZipFile(shared_ptr<IStream> inputStream) : mEntries(){
  gsf_init();

  unique_ptr<GsfInput, GsfInput_deleter> gsfInputStream(gsf_input_istream_new(inputStream.get()));

  GError *error = nullptr;
  mGsfZipStream = unique_ptr<GsfInfile, GsfInfile_deleter> (gsf_infile_zip_new(gsfInputStream.get(), &error));

  if (!mGsfZipStream) {
    unique_ptr<GError, GError_deleter> err(error);
    throw ZipException("Not a valid zip file", err->message);
  }
}

string ZipFile::GetEntry(const string& entryPath) const {
  map<string, string>::iterator it;

  unique_ptr<gchar*, GsfStrSplit_deleter> elems(g_strsplit(entryPath.c_str(), "/", -1));

  auto child = gsf_infile_child_by_aname(mGsfZipStream.get(), const_cast<const char**>(elems.get()));
  if (child == nullptr)
    throw ZipEntryNotFoundException("Entry not found", entryPath);

  unique_ptr<GsfInput, GsfInput_deleter> entry(child);

  gsf_off_t size = gsf_input_size(entry.get());

  auto buf = gsf_input_read(entry.get(), size, nullptr);
  if (!buf)
    throw ZipException("Failed reading entry", entryPath);

  return string(reinterpret_cast<const char*>(buf), size);
}

void ZipFile::SetEntry(const string& entryPath, const string& content) {
   mEntries[entryPath] = content;
}

void ZipFile::CloneFileEntry(GsfInput* input, GsfOutput* output, const string& path, map<string, string>& map) {
  static const int kChunkSize = 314;

  std::map<string, string>::iterator it;
  auto fixedPath = path.substr(0, path.size() - 1);
  if ((it = map.find(fixedPath)) != map.end())
  {
    if (!gsf_output_write(output, it->second.length(), reinterpret_cast<const guint8*>(it->second.c_str()))) {
      throw ZipException("Unable to write to existing entry");
    }
    map.erase(it);
  }
  else {
    size_t len;

    while ((len = gsf_input_remaining(input)) > 0) {
      guint8 const *data;
      if (len > kChunkSize)
        len = kChunkSize;
      if (NULL == (data = gsf_input_read(input, len, NULL))) {
        throw ZipException("Unable to read from entry");
      }
      if (!gsf_output_write(output, len, data)) {
        throw ZipException("Unable to write to existing entry");
      }
    }
  }
}

void ZipFile::CloneDirEntry(GsfInput* input, GsfOutput* output, const string& path, map<string, string>& map) {
  int i, n = gsf_infile_num_children(GSF_INFILE(input));
  for (i = 0; i < n; i++) {
    int level;
    gboolean is_dir;
    char const *name = gsf_infile_name_by_index(GSF_INFILE(input), i);

    unique_ptr<GsfInfile, GsfInfile_deleter> inputPtr(GSF_INFILE(gsf_infile_child_by_index(GSF_INFILE(input), i)));
    if (!inputPtr) {
      // TODO:Unable to get the child by index, this should be warn when having logs.
      continue;
    }

    is_dir = gsf_infile_num_children(inputPtr.get()) >= 0;

    g_object_get(G_OBJECT(inputPtr.get()), "compression-level", &level, NULL);

    unique_ptr<GsfOutfile, GsfOutfile_deleter> outputPtr(GSF_OUTFILE(gsf_outfile_new_child_full(GSF_OUTFILE(output), name, is_dir,
      "compression-level", level,
      NULL)));
    const string newPath = path + string(name) + "/";
    Clone(inputPtr, outputPtr, newPath, map);
  }
}

void ZipFile::Clone(unique_ptr<GsfInfile, GsfInfile_deleter>& in, unique_ptr<GsfOutfile, GsfOutfile_deleter>& out, const string& path, map<string, string>& map){
  if (gsf_input_size(GSF_INPUT(in.get())) > 0) {
    CloneFileEntry(GSF_INPUT(in.get()), GSF_OUTPUT(out.get()), path, map); // Clones file in the zip.
  } else {
    CloneDirEntry(GSF_INPUT(in.get()), GSF_OUTPUT(out.get()), path, map); // clones directory in the zip.
  }
}

void ZipFile::AddNewEntries(unique_ptr<GsfOutfile, GsfOutfile_deleter>& outfile, map<string,string>& entries) {
  for (map<string, string>::iterator it = entries.begin(); it != entries.end(); ++it)
  {
    unique_ptr<gchar*, GsfStrSplit_deleter> elems(g_strsplit(it->first.c_str(), "/", -1));
    auto elemsPtr = elems.get();
    auto firstName = *elemsPtr;
    elemsPtr++;
    unique_ptr<GsfOutput, GsfOutput_deleter> firstChild(gsf_outfile_new_child(outfile.get(), firstName, *elemsPtr != NULL));
    vector<unique_ptr<GsfOutput, GsfOutput_deleter>> children;
    children.push_back(move(firstChild));

    int i = 0;
    while (*elemsPtr) {
      gchar *currentName = *elemsPtr;
      elemsPtr++;
      unique_ptr<GsfOutput, GsfOutput_deleter> currentChild(gsf_outfile_new_child(GSF_OUTFILE(children.at(i).get()), currentName, *elemsPtr != NULL));

      children.push_back(move(currentChild));
      i++;
    }

    unique_ptr<GsfOutput, GsfOutput_deleter> targetChild = move(children.at(children.size() - 1));
    if (!gsf_output_write(targetChild.get(), it->second.length(), reinterpret_cast<const guint8*>(it->second.c_str())))
      throw ZipException("Unable to write on the new created child");
  }
}

void ZipFile::Commit(shared_ptr<IStream> outputStream){
  GError   *error = NULL;

  unique_ptr<GsfOutput, GsfOutput_deleter> output(gsf_output_stdio_new("C:\\Users\\amassi\\AppData\\Local\\Temp\\aa.docx", &error));
  if (!output) {
    unique_ptr<GError, GError_deleter> err(error);
    throw ZipException("Unable to create temp file", err->message);
  }

  unique_ptr<GsfOutfile, GsfOutfile_deleter> outfile(gsf_outfile_zip_new(output.get(), &error));
  if (!outfile) {
    unique_ptr<GError, GError_deleter> err(error);
    throw ZipException("Unable to open temp file", err->message);
  }
  map<string, string> entries(mEntries);
  Clone(mGsfZipStream, outfile, "", entries);

  AddNewEntries(outfile, entries);
}


} // namespace file
} // namespace mip
