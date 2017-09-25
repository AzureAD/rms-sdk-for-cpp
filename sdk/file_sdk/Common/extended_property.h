#ifndef EXTENDED_PROPERTY_H_
#define EXTENDED_PROPERTY_H_

#include <string>
#include <vector>
#include "string_utils.h"
#include <algorithm>

using std::vector;

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

  static vector<ExtendedProperty>::const_iterator GetMatchingProperty(const vector<ExtendedProperty>& extendedProperties, const ExtendedProperty& extendedProperty){
    return std::find_if(extendedProperties.cbegin(), extendedProperties.cend(), [&extendedProperty](const ExtendedProperty& p) {
        return EqualsIgnoreCase(p.key, extendedProperty.key) && EqualsIgnoreCase(p.vendor, extendedProperty.vendor);
      });
  }
};

} // namespace mip

#endif //API_IMPL_EXTENDED_PROPERTY_H_
