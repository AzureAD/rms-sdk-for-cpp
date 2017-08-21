#ifndef FILE_FORMAT_BASE_H
#define FILE_FORMAT_BASE_H

#include <string>
#include <vector>
#include "../../rmscrypto_sdk/CryptoAPI/IStream.h"
#include "tag.h"
using std::string;
using std::vector;

namespace mip{
namespace file {

class IFileFormat
{
public:
  IFileFormat(rmscrypto::api::SharedStream& file, string extension);

  virtual string GetOriginalExtension() = 0;

  virtual void SetTags(vector<Tag>& tags);

  virtual vector<Tag> GetTags();

  virtual void Commit(rmscrypto::api::SharedStream& file, string& extension) = 0;

private:
  string& mExtension;
  rmscrypto::api::SharedStream& mFile;
  vector<Tag> mTags;
};

} //namespace file
} //namespace mip

#endif // FILE_FORMAT_BASE_H
