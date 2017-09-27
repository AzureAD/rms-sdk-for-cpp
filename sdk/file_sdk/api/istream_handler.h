#ifndef ISTREAMHANDLER_H
#define ISTREAMHANDLER_H

#include <memory>

#include <Common/istream.h>
#include "custom_permissions_options.h"
#include "label.h"
#include "fileapi_export.h"
#include "ipolicy_engine.h"
#include "labeling_options.h"

using mip::file::ILabel;
using mip::file::LabelingOptions;
using mip::file::IPolicyEngine;

namespace mip {
namespace file {

class IStreamHandler {
public:
  DLL_PUBLIC_FILE static std::shared_ptr<IStreamHandler> Create(std::shared_ptr<IPolicyEngine> engine, std::shared_ptr<IStream> inputStream, const std::string& inputExtension);

  virtual bool IsLabeled() = 0;

  virtual bool IsProtected() = 0;

  virtual std::shared_ptr<Tag> GetLabel() = 0;

  virtual void SetLabel(std::shared_ptr<IStream> outputStream, const std::string& labelId, const LabelingOptions& labelingOptions, std::string& newExtention) = 0;

  virtual void SetLabel(std::shared_ptr<IStream> outputStream, std::shared_ptr<ILabel> label, const LabelingOptions& labelingOptions, std::string& newExtention) = 0;

  virtual void DeleteLabel(std::shared_ptr<IStream> outputStream, const std::string& justificationMessage, std::string &newExtention) = 0;

  virtual void SetProtection(std::shared_ptr<IStream> outputStream, const CustomPermissionsOptions& customPermissionsOptions, std::string& newExtention) = 0;

  virtual void RemoveProtection(std::shared_ptr<IStream> outputStream, std::string& newExtention) = 0;

  virtual ~ IStreamHandler () {
  }

protected:
  IStreamHandler () {
  }

};

} //namespace file
} //namespace mip

#endif // ISTREAMHANDLER_H
