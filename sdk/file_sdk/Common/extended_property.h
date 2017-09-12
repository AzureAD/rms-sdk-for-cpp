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

  bool operator==(const ExtendedProperty& property) const {
    return EqualsIgnoreCase(key, property.key) &&
        EqualsIgnoreCase(vendor, property.vendor) &&
        value == property.value;
  }
};

} // namespace mip

#endif //API_IMPL_EXTENDED_PROPERTY_H_
