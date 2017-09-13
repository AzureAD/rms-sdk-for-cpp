#ifndef ILABEL_H
#define ILABEL_H

#include <memory>
#include <string>
#include <vector>

#include "api/fileapi_export.h"

namespace mip {
namespace file {

// Abstraction for a single Microsoft Information Protection label.
class ILabel {
public:
  virtual ~ILabel() { }
  virtual const std::string& GetId() const = 0;
  // TODO: Think about locale.
  virtual const std::string& GetName() const = 0;
  virtual const std::string& GetDescription() const = 0;
  // View only label cannot be applied, and is used for display purposes only.
  virtual bool IsViewOnly() const = 0;
  // Color values are of the format "#RRGGBB" where each of RR, GG, BB is a hexadecimal 0-f.
  virtual const std::string& GetColor() const = 0;
  virtual const std::vector<std::shared_ptr<ILabel>>& GetChildren() const = 0;
  virtual std::weak_ptr<ILabel> GetParent() const = 0;

protected:
  ILabel() { }
};

} //namespace file
} //namespace mip

#endif // ILABEL_H
