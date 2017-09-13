#include "stream_handler.h"

#include <iterator>
#include <chrono>
#include <ctime>

#include <api/istream_handler.h>
#include <file_format_factory.h>

using mip::file::FileFormatFactory;

namespace mip {
namespace file {

DLL_PUBLIC_FILE std::shared_ptr<IStreamHandler> IStreamHandler::Create(std::shared_ptr<IPolicyEngine> engine, std::shared_ptr<IStream> inputStream, const std::string &inputExtension) {
  return std::make_shared<StreamHandler>(engine, inputStream, inputExtension);
}

StreamHandler::StreamHandler(std::shared_ptr<IPolicyEngine> engine, std::shared_ptr<IStream> inputStream, const std::string &inputExtension) {
  mEngine = engine;
  mFileFormat = FileFormatFactory::Create(inputStream, inputExtension);
  mLabels = mFileFormat->GetTags();
}

bool StreamHandler::IsLabeled() {
  return mFileFormat->GetTags().size() > 0;
}

bool StreamHandler::IsProtected() {
  return false;
}

std::shared_ptr<Tag> StreamHandler::GetLabel() {
  return std::make_shared<Tag>(mFileFormat->GetTags()[0]);
}

void StreamHandler::SetLabel(const std::string& labelId, const LabelingOptions& labelingOptions) {
  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  Tag tag(labelId, "Label", "parentId", labelingOptions.GetOwner(), true, std::ctime(&now)); //will get all the missing data such as label name from UPE when impl
  mLabels.push_back(tag);
}

void StreamHandler::SetLabel(std::shared_ptr<ILabel> label, const LabelingOptions& labelingOptions) {
  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

  if (auto ptr = label->GetParent().lock()) {
    Tag tag(label->GetId(), label->GetName(), ptr->GetId(), labelingOptions.GetOwner(), true, std::ctime(&now), labelingOptions.GetAssingmentMethod());
    mLabels.push_back(tag);
  }
}

void StreamHandler::DeleteLabel(const std::string& justificationMessage) {
  mLabels.clear();
}

void StreamHandler::SetProtection(const UserPolicy& policy) {
}

void StreamHandler::RemoveProtection() {
}

void StreamHandler::Commit(std::shared_ptr<IStream> outputStream, std::string& outputExtension) {
  mFileFormat->SetTags(mLabels);
  mFileFormat->Commit(outputStream, outputExtension);
}

} //namespace file
} //namespace mip
