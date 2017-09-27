#include "OPCFileFormat/xml/xml_helper.h"

namespace mip {
namespace file {

void DeleteXmlString(xmlChar* str) {
  xmlFree(str);
}

void DeleteXmlDoc(xmlDocPtr doc) {
  xmlFreeDoc(doc);
}

} // namespace file
} // namepsace mip
