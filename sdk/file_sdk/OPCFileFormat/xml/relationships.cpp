#include "OPCFileFormat/xml/relationships.h"

namespace mip {
namespace file {

RelationshipsXml::RelationshipsXml(const std::string &xml)
    : OpcXml(xml) {
}

bool RelationshipsXml::UpdateCustomProperties() {
  return false;
}

} // namespace file
} // namespace mip
