#ifndef CUSTOMPROPERTIES_H
#define CUSTOMPROPERTIES_H

#include "opcxml.h"
#include <string>

namespace mip {
namespace file {

class CustomPropertiesXml : public OpcXml {
public:
  CustomPropertiesXml(const std::string& xml);
};

} // namespace file
} // namespace mip

#endif // CUSTOMPROPERTIES_H
