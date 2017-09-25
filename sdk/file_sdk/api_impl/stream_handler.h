#ifndef STREAMHANDLER_H
#define STREAMHANDLER_H

#include <Common/ifile_format.h>
#include <api/istream_handler.h>

namespace mip {
namespace file {

class DLL_PUBLIC_FILE StreamHandler : public IStreamHandler
{
private:
  vector<pair<string, string>> mProperties; // holds the labels to commit; Tag will be replaced with ILabel when impl
  vector<Tag> mTags;
  time_t GetCurrentTime();

protected:
  std::shared_ptr<IPolicyEngine> mEngine;
  std::shared_ptr<IFileFormat> mFileFormat;

public:
  StreamHandler(std::shared_ptr<IPolicyEngine> engine, std::shared_ptr<IStream> inputStream, const std::string &inputExtension);

  // IStreamHandler interface
  virtual bool IsLabeled() override;
  virtual bool IsProtected() override;
  virtual std::shared_ptr<Tag> GetLabel() override;
  virtual void SetLabel(const std::string& labelId, const LabelingOptions& labelingOptions) override;
  virtual void SetLabel(std::shared_ptr<ILabel> label, const LabelingOptions& labelingOptions) override;
  virtual void DeleteLabel(const std::string& justificationMessage) override;
  virtual void SetProtection(const UserPolicy & policy) override;
  virtual void RemoveProtection() override;
  virtual void Commit(std::shared_ptr<IStream> outputStream, std::string& outputExtension) override;

  virtual ~StreamHandler() {
  }
};

} //namespace file
} //namespace mip

#endif // STREAMHANDLER_H
