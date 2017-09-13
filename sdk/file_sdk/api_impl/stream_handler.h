#ifndef STREAMHANDLER_H
#define STREAMHANDLER_H

#include <api/istream_handler.h>

namespace mip {
namespace file {

class DLL_PUBLIC_FILE StreamHandler : public IStreamHandler
{
public:
  StreamHandler(std::shared_ptr<IPolicyEngine> engine, std::shared_ptr<IStream> inputStream, const std::string &inputExtension);

  // IStreamHandler interface
  bool IsLabeled() override;
  bool IsProtected() override;
  std::shared_ptr<Tag> GetLabel() override;
  void SetLabel(const std::string& labelId, const LabelingOptions& labelingOptions) override;
  void SetLabel(std::shared_ptr<ILabel> label, const LabelingOptions& labelingOptions) override;
  void DeleteLabel(const std::string& justificationMessage) override;
  void SetProtection(const UserPolicy & policy) override;
  void RemoveProtection() override;
  void Commit(std::shared_ptr<IStream> outputStream, std::string& outputExtension) override;
};

} //namespace file
} //namespace mip

#endif // STREAMHANDLER_H
