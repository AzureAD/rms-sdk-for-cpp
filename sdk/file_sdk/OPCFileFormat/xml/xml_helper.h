#ifndef OPCFILEFORMAT_XML_XML_HELPER_H_
#define OPCFILEFORMAT_XML_XML_HELPER_H_

#include <string>
#include <memory>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#include "libxml/tree.h"
#ifdef __cplusplus
}
#endif // __cplusplus

namespace mip {

template<class T>
using UniquePtr = std::unique_ptr<T, void(*)(T*)>;

inline std::string ConvertXmlString(const xmlChar* str) {
  return str ? reinterpret_cast<const char*>(str) : std::string();
}

inline const xmlChar* ConvertXmlString(const std::string& str) {
  return reinterpret_cast<const xmlChar*>(str.c_str());
}

void DeleteXmlString(xmlChar* str);
void DeleteXmlDoc(xmlDocPtr doc);

} // namepsace mip

#endif // OPCFILEFORMAT_XML_XML_HELPER_H_
