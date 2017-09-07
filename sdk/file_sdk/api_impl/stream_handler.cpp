#include "stream_handler.h"

namespace mip {
namespace file {

StreamHandler::StreamHandler()
{

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

void StreamHandler::Commit(std::shared_ptr<rmscrypto::api::IStream> outputStream, std::string& outputExtension) {
}

} //namespace file
} //namespace mip
