#ifndef FILE_SDK_IFILE_FORMAT_H
#define FILE_SDK_IFILE_FORMAT_H

#include <string>
#include <vector>
#include "istream.h"

using std::string;
using std::vector;
using std::pair;

namespace mip {
namespace file {

class IFileFormat
{
public:
  // Returns the extension of the plain file if the file is protected
  virtual string GetOriginalExtension() = 0;

  // Returns current properties or the properties form the file if not set
  virtual const vector<pair<string, string>> GetProperties() = 0;

  // Sets properties for the file and the properties will be added/removed after calling Commit
  virtual void UpdateProperties(const vector<pair<string, string>>& propertiesToAdd, const vector<string>& keysToRemove) = 0;

  // Saves original file to the output file with tags and protection and sets the new file extension to newExtension parameter
  virtual void Commit(std::shared_ptr<IStream> outputStream, string& newExtension) = 0;

  virtual ~IFileFormat() = default;
};

} //namespace file
} //namespace mip

#endif // FILE_SDK_IFILE_FORMAT_H
