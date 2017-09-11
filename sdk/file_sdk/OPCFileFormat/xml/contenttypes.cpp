#include "xml/contenttypes.h"

namespace mip {
namespace file {

ContentTypesXml::ContentTypesXml(const std::string &xml)
    : OpcXml(xml) {
}

bool ContentTypesXml::UpdateCustomProperties() {
    return false;
}

} // namespace file
} // namespace mip
