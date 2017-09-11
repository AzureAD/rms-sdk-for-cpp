#ifndef OPCXML_H
#define OPCXML_H

#include <string>

namespace mip {
namespace file {

class OpcXml {
public:
  OpcXml(const std::string& xml);

  std::string Serialize();
};

} // namespace file
} // namespace mip

#endif // OPCXML_H
