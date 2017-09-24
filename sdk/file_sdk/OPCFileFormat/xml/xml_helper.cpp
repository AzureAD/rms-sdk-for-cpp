#include "OPCFileFormat/xml/xml_helper.h"

namespace mip {

void DeleteXmlString(xmlChar* str) {
  xmlFree(str);
}

void DeleteXmlDoc(xmlDocPtr doc) {
  xmlFreeDoc(doc);
}

} // namepsace mip
