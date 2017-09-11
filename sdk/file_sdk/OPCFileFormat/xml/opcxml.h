#ifndef OPCXML_H
#define OPCXML_H

#include <string>

namespace mip {
namespace file {

class OpcXml {
protected:
  OpcXml(const std::string& xml);

public:
  std::string Serialize();
};

} // namespace file
} // namespace mip

#endif // OPCXML_H
