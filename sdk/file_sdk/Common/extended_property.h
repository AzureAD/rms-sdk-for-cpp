#ifndef EXTENDED_PROPERTY_H_
#define EXTENDED_PROPERTY_H_

#include <string>

namespace mip {

struct ExtendedProperty
{
  std::string vendor;
  std::string key;
  std::string value;

  bool operator==(const ExtendedProperty& property)
  {
    return key == property.key &&
           value == property.value &&
           vendor == property.vendor;
  }
};

} // namespace mip

#endif //API_IMPL_EXTENDED_PROPERTY_H_
