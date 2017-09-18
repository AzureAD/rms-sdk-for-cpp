#ifndef RELATIONSHIPS_H
#define RELATIONSHIPS_H

#include "OPCFileFormat/xml/opcxml.h"
#include <string>

namespace mip {
namespace file {

class RelationshipsXml : public OpcXml {
public:
  RelationshipsXml(const std::string& xml);
  bool UpdateCustomProperties();
};

} // namespace file
} // namespace mip

#endif // RELATIONSHIPS_H
