#ifndef CUSTOM_PERMISSIONS_OPTIONS_H
#define CUSTOM_PERMISSIONS_OPTIONS_H

#include <PolicyDescriptor.h>
#include <UserRights.h>

using rmscore::modernapi::PolicyDescriptor;
using rmscore::modernapi::UserRights;

namespace mip {
namespace file {

class CustomPermissionsOptions
{
private:
  PolicyDescriptor mPolicyDescriptor;
public:
  CustomPermissionsOptions(PolicyDescriptor& policyDescriptor = PolicyDescriptor(std::vector<UserRights>()))
                         : mPolicyDescriptor(policyDescriptor){}

  const PolicyDescriptor GetPolicyDescriptor() const { return mPolicyDescriptor; }

  void SetPolicyDescriptor(const PolicyDescriptor& policyDescriptor) { mPolicyDescriptor = policyDescriptor; }
};

} //namespace file
} //namespace mip

#endif // CUSTOM_PERMISSIONS_OPTIONS_H
