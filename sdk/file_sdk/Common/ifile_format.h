#ifndef IFILE_FORMAT_H
#define IFILE_FORMAT_H

#include <string>
#include <vector>
#include <IStream.h>
#include "tag.h"

using std::string;
using std::vector;
using rmscrypto::api::IStream;

namespace mip {
namespace file {

class IFileFormat
{
public:
  // Returns the extension of the plain file if the file is protected
  virtual string GetOriginalExtension() = 0;

  // Returns current tags to the tags form the file if not set
  virtual const vector<Tag> GetTags() = 0;

  // Sets tags for the file and the tags will be applied after calling Commit
  virtual void SetTags(const vector<Tag>& tags) = 0;

  // Saves original file to the output file with tags and protection and sets the new file extension to newExtension parameter
  virtual void Commit(std::shared_ptr<IStream> file, string& newExtension) = 0;

};

} //namespace file
} //namespace mip

#endif // FILE_FORMAT_BASE_H
