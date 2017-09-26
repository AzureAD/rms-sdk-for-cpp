#ifndef STREAMHANDLER_H
#define STREAMHANDLER_H

#include <Common/ifile_format.h>
#include <api/istream_handler.h>

namespace mip {
namespace file {

class DLL_PUBLIC_FILE StreamHandler : public IStreamHandler
{
private:
  std::shared_ptr<IPolicyEngine> mEngine;
  std::shared_ptr<IFileFormat> mFileFormat;

  time_t GetCurrentTime();
  std::string UpdatePropertiesAndCommit(std::shared_ptr<IStream> outputStream, vector<pair<std::string, std::string> > newProperties);

public:
  StreamHandler(std::shared_ptr<IPolicyEngine> engine, std::shared_ptr<IStream> inputStream, const std::string &inputExtension);

  // IStreamHandler interface
  virtual bool IsLabeled() override;
  virtual bool IsProtected() override;
  virtual std::shared_ptr<Tag> GetLabel() override;
  virtual std::string SetLabel(std::shared_ptr<IStream> outputStream, const std::string& labelId, const LabelingOptions& labelingOptions) override;
  virtual std::string SetLabel(std::shared_ptr<IStream> outputStream, std::shared_ptr<ILabel> label, const LabelingOptions& labelingOptions) override;
  virtual std::string DeleteLabel(std::shared_ptr<IStream> outputStream, const std::string& justificationMessage) override;
  virtual void SetProtection(const UserPolicy & policy) override;
  virtual void RemoveProtection() override;

  virtual ~StreamHandler() {
  }
};

} //namespace file
} //namespace mip

#endif // STREAMHANDLER_H
