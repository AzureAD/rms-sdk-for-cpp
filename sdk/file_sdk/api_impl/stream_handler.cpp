#include "stream_handler.h"

#include <iterator>
#include <chrono>
#include <ctime>

#include <api/istream_handler.h>
#include <Common/file_format_factory.h>

using mip::file::FileFormatFactory;

namespace mip {
namespace file {

DLL_PUBLIC_FILE std::shared_ptr<IStreamHandler> IStreamHandler::Create(std::shared_ptr<IPolicyEngine> engine, std::shared_ptr<IStream> inputStream, const std::string &inputExtension) {
  return std::make_shared<StreamHandler>(engine, inputStream, inputExtension);
}

StreamHandler::StreamHandler(std::shared_ptr<IPolicyEngine> engine, std::shared_ptr<IStream> inputStream, const std::string &inputExtension) {
  mEngine = engine;
  mFileFormat = FileFormatFactory::Create(inputStream, inputExtension);
  mProperties = mFileFormat->GetProperties();
  mTags = Tag::FromProperties(mProperties);
}

bool StreamHandler::IsLabeled() {
  return mProperties.size() > 0 && mTags[0].GetEnabled();
}

bool StreamHandler::IsProtected() {
  return false;
}

std::shared_ptr<Tag> StreamHandler::GetLabel() {
  return std::make_shared<Tag>(Tag::FromProperties(mFileFormat->GetProperties())[0]);
}

void StreamHandler::SetLabel(const std::string& labelId, const LabelingOptions& labelingOptions) {
  std::time_t now = GetCurrentTime();

  mTags.clear(); // need to clear all previous labels that was set.
  mTags.push_back(Tag(labelId, "Label", "parentId", labelingOptions.GetOwner(), true, std::ctime(&now))); //will get all the missing data such as label name from UPE when impl;
}

void StreamHandler::SetLabel(std::shared_ptr<ILabel> label, const LabelingOptions& labelingOptions) {
  SetLabel(label->GetId(), labelingOptions);
}

void StreamHandler::DeleteLabel(const std::string& justificationMessage) {
  std::vector<Tag> newLabels;
  for (const auto &label : mTags){
    newLabels.push_back(Tag(label.GetLabelId(),
                            label.GetLabelName(),
                            label.GetLabelParentId(),
                            label.GetOwner(),
                            false,
                            label.GetSetTime(),
                            label.GetMethod(),
                            label.GetSiteId(),
                            label.GetExtendedProperties()));
  }
  mTags = newLabels;
}

void StreamHandler::SetProtection(const UserPolicy& policy) {
}

void StreamHandler::RemoveProtection() {
}

void StreamHandler::Commit(std::shared_ptr<IStream> outputStream, std::string& outputExtension) {
  mFileFormat->UpdateProperties(mProperties, {});
  mFileFormat->Commit(outputStream, outputExtension);
}

time_t StreamHandler::GetCurrentTime() {
  return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

} //namespace file
} //namespace mip
