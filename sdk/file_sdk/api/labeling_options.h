#ifndef LABELING_OPTIONS_H
#define LABELING_OPTIONS_H

#include <string>
#include <Common/tag.h>

namespace mip {
namespace file {

class LabelingOptions
{
private:
  std::string mJustificationMessage;
  Method mAssingmentMethod;
  std::string mOwner;

public:
  LabelingOptions(const std::string& justificationMessage = "",
                  const Method& assignmentMethod = Method::NONE,
                  const std::string owner = "")
                : mJustificationMessage(justificationMessage),
                  mAssingmentMethod(assignmentMethod),
                  mOwner(owner) {}

  inline std::string& GetJustificationMessage() const { return std::string(mJustificationMessage); }

  inline Method& GetAssingmentMethod() const { return Method(mAssingmentMethod); }

  inline std::string& GetOwner() const {return std::string(mOwner); }

  inline void SetJustificationMessage(const std::string& justificationMessage) { mJustificationMessage = justificationMessage; }

  inline void SetAssignmentMethod(const Method& assignmentMethod) { mAssingmentMethod = assignmentMethod; }

  inline void SetOwner(const std::string owner) { mOwner = owner; }
};


} //namespace file
} //namespace mip

#endif // LABELING_OPTIONS_H
