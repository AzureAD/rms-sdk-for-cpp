#ifndef EXTENDED_PROPERTY_H_
#define EXTENDED_PROPERTY_H_

#include <string>
#include "string_utils.h"
namespace mip {

struct ExtendedProperty
{
  std::string vendor;
  std::string key;
  std::string value;

  bool operator==(const ExtendedProperty& property) const
  {
    return EqualsIgnoreCase(key, property.key) &&
           EqualsIgnoreCase(value, property.value) &&
           EqualsIgnoreCase(vendor, property.vendor);
  }
};

} // namespace mip

#endif //API_IMPL_EXTENDED_PROPERTY_H_
