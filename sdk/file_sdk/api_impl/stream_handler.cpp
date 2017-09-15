#include <api/istream_handler.h>
#include "stream_handler.h"

namespace mip {
namespace file {

DLL_PUBLIC_FILE std::shared_ptr<IStreamHandler> IStreamHandler::Create(std::shared_ptr<IPolicyEngine> engine, std::shared_ptr<IStream> inputStream, const std::string &inputExtension) {
  return std::make_shared<StreamHandler>(engine, inputStream, inputExtension);
}

StreamHandler::StreamHandler(std::shared_ptr<IPolicyEngine> engine, std::shared_ptr<IStream> inputStream, const std::string &inputExtension) {
}

bool StreamHandler::IsLabeled() {
  return false;
}

bool StreamHandler::IsProtected() {
  return false;
}

std::shared_ptr<Tag> StreamHandler::GetLabel() {
  return nullptr;
}

void StreamHandler::SetLabel(const std::string& labelId, const LabelingOptions& labelingOptions) {
}

void StreamHandler::SetLabel(std::shared_ptr<ILabel> label, const LabelingOptions& labelingOptions) {
}

void StreamHandler::DeleteLabel(const std::string& justificationMessage) {
}

void StreamHandler::SetProtection(const UserPolicy& policy) {
}

void StreamHandler::RemoveProtection() {
}

void StreamHandler::Commit(std::shared_ptr<IStream> outputStream, std::string& outputExtension) {
}

} //namespace file
} //namespace mip
