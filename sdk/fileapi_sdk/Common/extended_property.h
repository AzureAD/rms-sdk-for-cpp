#ifndef API_IMPL_EXTENDED_PROPERTY_H_
#define API_IMPL_EXTENDED_PROPERTY_H_

#include <string>

namespace mip {

struct ExtendedProperty
{
  std::string vendor;
  std::string key;
  std::string value;
};

}

#endif //API_IMPL_EXTENDED_PROPERTY_H_
