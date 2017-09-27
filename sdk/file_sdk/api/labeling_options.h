#ifndef LABELING_OPTIONS_H
#define LABELING_OPTIONS_H

#include <string>
#include <Common/tag.h>
#include "custom_permissions_options.h"

namespace mip {
namespace file {

class LabelingOptions
{
private:
  std::string mJustificationMessage;
  Method mAssingmentMethod;
  std::string mOwner;
  CustomPermissionsOptions mCustomPermissionsOprions;

public:
  LabelingOptions(const std::string& justificationMessage = "",
                  const Method& assignmentMethod = Method::NONE,
                  const std::string owner = "",
                  const CustomPermissionsOptions& customPermissionsOprions = CustomPermissionsOptions())
                : mJustificationMessage(justificationMessage),
                  mAssingmentMethod(assignmentMethod),
                  mOwner(owner),
                  mCustomPermissionsOprions(customPermissionsOprions){}

  const std::string& GetJustificationMessage() const { return mJustificationMessage; }

  const Method GetAssingmentMethod() const { return mAssingmentMethod; }

  const std::string& GetOwner() const { return mOwner; }

  const CustomPermissionsOptions& GetCustomPermissionsOptions() const { return mCustomPermissionsOprions; }

  inline void SetJustificationMessage(const std::string& justificationMessage) { mJustificationMessage = justificationMessage; }

  inline void SetAssignmentMethod(const Method& assignmentMethod) { mAssingmentMethod = assignmentMethod; }

  inline void SetOwner(const std::string owner) { mOwner = owner; }

  inline void SetCustomPermissionsOptions(const CustomPermissionsOptions& customPermissionsOptions) { mCustomPermissionsOprions = customPermissionsOptions; }
};


} //namespace file
} //namespace mip

#endif // LABELING_OPTIONS_H
