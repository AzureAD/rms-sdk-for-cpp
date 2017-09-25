#include "xmp_helper.h"
#include "Common/exceptions.h"

using std::pair;

namespace mip {
namespace file {

XMPHelper& XMPHelper::GetInstance()
{
  static XMPHelper instance;
  return instance;
}

XMPHelper::XMPHelper()
{
  try{
    if (!SXMPMeta::Initialize())
      throw new Exception("SXMPMeta Error");

    if (!SXMPFiles::Initialize(kXMPFiles_IgnoreLocalText))
      throw new Exception("SXMPFiles Error");

    string actualPrefix;
    SXMPMeta::RegisterNamespace(kMsipNamespace, "msip", &actualPrefix);
  }
  catch(XMP_Error ex){
    throw new Exception(ex.GetErrMsg());
  }
}

void XMPHelper::Serialize(SXMPMeta& metadata, const vector<pair<std::string, std::string>>& propertiesToAdd, const vector<string>& keysToRemove) {
  string propertyPath;

  metadata.DeleteProperty(kMsipNamespace, "Labels"); //delete all labels from old format

  for(unsigned i =0; i< keysToRemove.size(); i++)
    metadata.DeleteProperty(kMsipNamespace, keysToRemove[i].c_str());

  for(unsigned j =0; j< propertiesToAdd.size(); j++){
    string value = propertiesToAdd[j].first;
    if (value.empty())
      continue;

    metadata.SetProperty(kMsipNamespace, propertiesToAdd[j].first.c_str(), propertiesToAdd[j].second);
  }
}

vector<pair<string, string>> XMPHelper::Deserialize(const SXMPMeta& metadata) {
  string namespacePrefix;
  vector<pair<string, string>> properties;

  if (!metadata.GetNamespacePrefix(kMsipNamespace, &namespacePrefix))
    return properties; // namespace doesn't exist

  string propertyPath;
  string propertyValue;

  SXMPIterator iterator(metadata, kMsipNamespace, kXMP_IterJustChildren | kXMP_IterOmitQualifiers | kXMP_IterJustLeafName);
  while (iterator.Next(nullptr, &propertyPath, &propertyValue))
  {
    if (propertyPath.length() < namespacePrefix.length())
      continue;

    auto name = propertyPath.erase(0, namespacePrefix.length());
    auto value = propertyValue;

    properties.push_back(pair<string, string>(name, value));
  }

  return properties;
}

const vector<pair<string, string>> XMPHelper::GetProperties(shared_ptr<IStream> fileStream) {
  XMPIOOverIStream xmpStream (fileStream);

  SXMPFiles file;
  if (!file.OpenFile(&xmpStream , kXMP_UnknownFile, kXMPFiles_OpenForRead | kXMPFiles_OpenUseSmartHandler) &&
      !file.OpenFile(&xmpStream , kXMP_UnknownFile, kXMPFiles_OpenForRead | kXMPFiles_OpenUsePacketScanning))
    throw Exception("OpenFile failed");

  try
  {
    SXMPMeta metadata;
    file.GetXMP(&metadata);
    auto properties = Deserialize(metadata);
    file.CloseFile();
    return properties;
  }
  catch(XMP_Error error)
  {
    file.CloseFile();
    throw Exception(error.GetErrMsg());
  }
  catch(...)
  {
    file.CloseFile();
    throw Exception("GetXMP failed");
  }
}

void XMPHelper::UpdateProperties(shared_ptr<IStream> stream, const vector<pair<std::string, std::string> >& propertiesToAdd, const vector<std::string>& keysToRemove) {
  XMPIOOverIStream xmpStream (stream);

  SXMPFiles file;
  if (!file.OpenFile(&xmpStream , kXMP_UnknownFile, kXMPFiles_OpenForUpdate | kXMPFiles_OpenUseSmartHandler) &&
      !file.OpenFile(&xmpStream , kXMP_UnknownFile, kXMPFiles_OpenForUpdate | kXMPFiles_OpenUsePacketScanning))
    throw Exception("OpenFile failed");

  try
  {
    SXMPMeta metadata;
    file.GetXMP(&metadata);
    Serialize(metadata, propertiesToAdd, keysToRemove);

    if (!file.CanPutXMP(metadata))
    {
      file.CloseFile();
      throw Exception("CanPutXMP failed");
    }

    stream->Seek(0);
    file.PutXMP(metadata);
    file.CloseFile();
  }
  catch(XMP_Error error)
  {
    file.CloseFile();
    throw Exception(error.GetErrMsg());
  }
  catch(...)
  {
    file.CloseFile();
    throw Exception("UpdateProperties faield");
  }
}

} // namespace file
} // namespace mip
