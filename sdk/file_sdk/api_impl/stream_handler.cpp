#include "stream_handler.h"

#include <iterator>
#include <chrono>
#include <ctime>

#include <api/istream_handler.h>
#include <Common/file_format_factory.h>
#include <Common/exceptions.h>

using mip::file::FileFormatFactory;

namespace mip {
namespace file {

DLL_PUBLIC_FILE std::shared_ptr<IStreamHandler> IStreamHandler::Create(std::shared_ptr<IPolicyEngine> engine, std::shared_ptr<IStream> inputStream, const std::string &inputExtension) {
  return std::make_shared<StreamHandler>(engine, inputStream, inputExtension);
}

StreamHandler::StreamHandler(std::shared_ptr<IPolicyEngine> engine, std::shared_ptr<IStream> inputStream, const std::string &inputExtension) {
  if (inputStream->Size() = 0) {
    throw mip::file::Exception("inputStream size is 0 (zero)");
  }
  mEngine = engine;
  mFileFormat = FileFormatFactory::Create(inputStream, inputExtension);
}

bool StreamHandler::IsLabeled() {
  return mFileFormat->GetProperties().size() > 0 && Tag::FromProperties(mFileFormat->GetProperties())[0].GetEnabled();
}

bool StreamHandler::IsProtected() {
  return false;
}

std::shared_ptr<Tag> StreamHandler::GetLabel() {
  return std::make_shared<Tag>(Tag::FromProperties(mFileFormat->GetProperties())[0]);
}

std::string StreamHandler::SetLabel(std::shared_ptr<IStream> outputStream, const std::string& labelId, const LabelingOptions& labelingOptions) {
  std::time_t now = GetCurrentTime();
  // will be replaced with label and UPE data
  Tag newTag(labelId, "labelName", "parent", labelingOptions.GetOwner(), true, std::ctime(&now), labelingOptions.GetAssingmentMethod());

  return UpdatePropertiesAndCommit(outputStream, newTag.ToProperties());
}

std::string StreamHandler::SetLabel(std::shared_ptr<IStream> outputStream, std::shared_ptr<ILabel> label, const LabelingOptions& labelingOptions) {
  return SetLabel(outputStream, label->GetId(), labelingOptions);
}



std::string StreamHandler::DeleteLabel(std::shared_ptr<IStream> outputStream, const std::string& justificationMessage) {
  vector<Tag> newTags(Tag::FromProperties(mFileFormat->GetProperties()));
  for (Tag &tag : newTags) {
    tag.SetEnabled(false);
  }

  return UpdatePropertiesAndCommit(outputStream, Tag::ToProperties(newTags));
}

void StreamHandler::SetProtection(const UserPolicy& policy) {
}

void StreamHandler::RemoveProtection() {
}

time_t StreamHandler::GetCurrentTime() {
  return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

std::string StreamHandler::UpdatePropertiesAndCommit(std::shared_ptr<IStream> outputStream, vector<pair<string,string>> newProperties) {
  vector<string> keysToRemove;
  for(pair<string,string> const& property: mFileFormat->GetProperties()) {
    keysToRemove.push_back(property.first);
  }

  mFileFormat->UpdateProperties(newProperties, keysToRemove);
  std::string newExtention;
  mFileFormat->Commit(outputStream, newExtention);

  return newExtention;
}

} //namespace file
} //namespace mip
