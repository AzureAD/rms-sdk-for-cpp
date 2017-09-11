#ifndef EXTENDED_PROPERTY_H_
#define EXTENDED_PROPERTY_H_

#include <string>

namespace mip {

struct ExtendedProperty
{
  std::string vendor;
  std::string key;
  std::string value;
};

} // namespace mip

#endif //API_IMPL_EXTENDED_PROPERTY_H_
