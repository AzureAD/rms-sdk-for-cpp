#ifndef CONTENTTYPES_H
#define CONTENTTYPES_H

#include "OPCFileFormat/xml/opc_xml.h"
#include <string>

namespace mip {
namespace file {

class ContentTypesXml : public OpcXml {
public:
  ContentTypesXml(const std::string& xml);
  bool UpdateCustomProperties();
};

} // namespace file
} // namespace mip

#endif // CONTENTTYPES_H
