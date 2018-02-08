#include "../../include/fxcrt/fx_xml.h"
#include "xml_int.h"
void FX_XML_SplitQualifiedName(FX_BSTR bsFullName, CFX_ByteStringC &bsSpace, CFX_ByteStringC &bsName)
{
    if (bsFullName.IsEmpty()) {
        return;
    }
    FX_INT32 iStart = 0;
    for (; iStart < bsFullName.GetLength(); iStart ++) {
        if (bsFullName.GetAt(iStart) == ':') {
            break;
        }
    }
    if (iStart >= bsFullName.GetLength()) {
        bsName = bsFullName;
    } else {
        bsSpace = CFX_ByteStringC(bsFullName.GetCStr(), iStart);
        iStart ++;
        bsName = CFX_ByteStringC(bsFullName.GetCStr() + iStart, bsFullName.GetLength() - iStart);
    }
}
void CXML_Element::SetTag(const CFX_ByteString& qSpace, const CFX_ByteString& tagname)
{
    m_QSpaceName = qSpace;
    m_TagName = tagname;
}
void CXML_Element::SetTag(const CFX_ByteString& qTagName)
{
    ASSERT(!qTagName.IsEmpty());
    CFX_ByteStringC bsSpace, bsName;
    FX_XML_SplitQualifiedName(qTagName, bsSpace, bsName);
    m_QSpaceName = bsSpace;
    m_TagName = bsName;
}
CXML_Element* CXML_Element::Clone()
{
    CXML_Element* pNewElement = NULL;
    pNewElement = FX_NEW CXML_Element;
    if (!pNewElement) {
        return NULL;
    }
    pNewElement->SetTag(GetNamespace(TRUE), GetTagName());
    FX_DWORD index = 0;
    for (index = 0; index < CountAttrs(); index++) {
        CFX_ByteString sSpace, sName;
        CFX_WideString sValue;
        GetAttrByIndex(index, sSpace, sName, sValue);
        if (!sSpace.IsEmpty()) {
            sName = sSpace + ":" + sName;
        }
        pNewElement->SetAttrValue(sName, sValue);
    }
    for (index = 0; index < CountChildren(); index++) {
        ChildType childType = GetChildType(index);
        switch (childType) {
            case CXML_Element::Element: {
                    CXML_Element* pNewChild = GetElement(index)->Clone();
                    if (pNewChild) {
                        pNewElement->AddChildElement(pNewChild);
                    }
                    break;
                }
            case CXML_Element::Content: {
                    pNewElement->AddChildContent(GetContent(index));
                    break;
                }
            default:
                break;
        }
    }
    return pNewElement;
}
void CXML_Element::SetAttrValue(const CFX_ByteString& name, const CFX_WideString& value)
{
    ASSERT(!name.IsEmpty());
    CFX_ByteStringC bsSpace, bsName;
    FX_XML_SplitQualifiedName(name, bsSpace, bsName);
    m_AttrMap.SetAt(bsSpace, bsName, value);
}
void CXML_Element::SetAttrValue(const CFX_ByteString& name, int value)
{
    ASSERT(!name.IsEmpty());
    char buf[32];
    FXSYS_itoa(value, buf, 10);
    FX_STRSIZE len = (FX_STRSIZE)FXSYS_strlen(buf);
    FX_WCHAR wbuf[32];
    for (FX_STRSIZE i = 0; i < len; i ++) {
        wbuf[i] = buf[i];
    }
    SetAttrValue(name, CFX_WideStringC(wbuf, len));
}
void CXML_Element::SetAttrValue(const CFX_ByteString& name, FX_FLOAT value)
{
    ASSERT(!name.IsEmpty());
    char buf[32];
    FX_STRSIZE len = FX_ftoa(value, buf);
    FX_WCHAR wbuf[32];
    for (FX_STRSIZE i = 0; i < len; i ++) {
        wbuf[i] = buf[i];
    }
    SetAttrValue(name, CFX_WideStringC(wbuf, len));
}
void CXML_Element::RemoveAttr(const CFX_ByteString& name)
{
    ASSERT(!name.IsEmpty());
    CFX_ByteStringC bsSpace, bsName;
    FX_XML_SplitQualifiedName(name, bsSpace, bsName);
    m_AttrMap.RemoveAt(bsSpace, bsName);
}
void CXML_Element::AddChildElement(CXML_Element* pElement)
{
    ASSERT(pElement != NULL);
    if (pElement == NULL) {
        return;
    }
    pElement->m_pParent = this;
    m_Children.Add((FX_LPVOID)Element);
    m_Children.Add(pElement);
}
void CXML_Element::AddChildContent(const CFX_WideString& content, FX_BOOL bCDATA)
{
    CXML_Content* pContent;
    pContent = FX_NEW CXML_Content;
    if (!pContent) {
        return;
    }
    pContent->Set(bCDATA, content);
    m_Children.Add((FX_LPVOID)Content);
    m_Children.Add(pContent);
}
void CXML_Element::InsertChildElement(FX_DWORD index, CXML_Element* pElement)
{
    ASSERT(pElement != NULL);
    if (pElement == NULL) {
        return;
    }
    pElement->m_pParent = this;
    m_Children.InsertAt(index * 2, (FX_LPVOID)Element);
    m_Children.InsertAt(index * 2 + 1, pElement);
}
void CXML_Element::InsertChildContent(FX_DWORD index, const CFX_WideString& content, FX_BOOL bCDATA)
{
    m_Children.InsertAt(index * 2, (FX_LPVOID)Content);
    CXML_Content* pContent;
    pContent = FX_NEW CXML_Content;
    if (!pContent) {
        return;
    }
    pContent->Set(bCDATA, content);
    m_Children.InsertAt(index * 2 + 1, pContent);
}
void CXML_Element::RemoveChild(FX_DWORD index)
{
    if (index * 2 >= (FX_DWORD)m_Children.GetSize()) {
        return;
    }
    ChildType type = (ChildType)(FX_UINTPTR)m_Children.GetAt(index * 2);
    FX_LPVOID addr = m_Children.GetAt(index * 2 + 1);
    if (type == Element) {
        ((CXML_Element*)addr)->RemoveChildren();
        delete (CXML_Element*)addr;
    } else if (type == Content) {
        delete (CXML_Content*)addr;
    }
    m_Children.RemoveAt(index * 2);
    m_Children.RemoveAt(index * 2);
}
static FX_BYTE gs_FXCRT_XML_EntityTypes[256] = {
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0x00, 0xA1, 0x00, 0x00, 0x00, 0xA2, 0xA3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA4, 0x00, 0xA5, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static FX_LPCSTR gs_FXCRT_XML_EntityStr[] = {
    "&#x00;", "&#x01;", "&#x02;", "&#x03;", "&#x04;", "&#x05;", "&#x06;", "&#x07;", "&#x08;", "&#x09;", "&#x0A;", "&#x0B;", "&#x0C;", "&#x0D;", "&#x0E;", "&#x0F;",
    "&#x10;", "&#x11;", "&#x12;", "&#x13;", "&#x14;", "&#x15;", "&#x16;", "&#x17;", "&#x18;", "&#x19;", "&#x1A;", "&#x1B;", "&#x1C;", "&#x1D;", "&#x1E;", "&#x1F;",
    "&#x20;", "&quot;", "&amp;", "&apos;", "&lt;", "&gt;",
};
static FX_BOOL XML_IsEntity(FX_WCHAR ch)
{
    if (ch > 255) {
        return FALSE;
    }
    return gs_FXCRT_XML_EntityTypes[ch] & 0x80;
}
static FX_LPCSTR XML_GetEntity(FX_WCHAR ch)
{
    return gs_FXCRT_XML_EntityStr[gs_FXCRT_XML_EntityTypes[ch] & 0x7F];
}
static void XML_EncodeString(FX_WSTR str, CFX_ByteString& encStr)
{
    CFX_UTF8Encoder encoder;
    for (int i = 0; i < str.GetLength(); i ++) {
        FX_WCHAR ch = str.GetAt(i);
        if (ch == '"') {
            encoder.AppendStr(FX_BSTRC("&quot;"));
        } else if (ch == '<') {
            encoder.AppendStr(FX_BSTRC("&lt;"));
        } else if (ch == '&') {
            encoder.AppendStr(FX_BSTRC("&amp;"));
        } else {
            encoder.Input(ch);
        }
    }
    encStr = encoder.GetResult();
}
static void XML_EncodeContent(FX_WSTR strSrc, CFX_ByteString& encStr, FX_BOOL KeepSpecialCode)
{
    CFX_UTF8Encoder encoder;
    FX_STRSIZE nLen = strSrc.GetLength();
    for (FX_STRSIZE i = 0; i < nLen; i ++) {
        FX_WCHAR ch = strSrc.GetAt(i);
        if(KeepSpecialCode) {
            encoder.Input(ch);
            continue;
        }
        if (ch == ' ') {
            if (i && i + 1 < nLen) {
                encoder.AppendStr(FX_BSTRC(" "));
            } else {
                encoder.AppendStr(FX_BSTRC("&#x20;"));
            }
        } else if (XML_IsEntity(ch)) {
            encoder.AppendStr(XML_GetEntity(ch));
        } else {
            encoder.Input(ch);
        }
    }
    encStr = encoder.GetResult();
}
void CXML_Composer::Compose(CXML_Element* pElement, IFX_FileWrite *pFileWrite, CFX_ByteString &xmlStr)
{
    ComposeElement(pElement, pFileWrite);
    if (pFileWrite && m_Buf.GetLength()) {
        pFileWrite->WriteBlock(m_Buf.GetBuffer(), m_Buf.GetLength());
        m_Buf.Clear();
        return;
    }
    xmlStr = m_Buf.GetByteString();
}
void CXML_Composer::ComposeElement(CXML_Element* pElement, IFX_FileWrite *pFileWrite)
{
    if (pFileWrite && m_Buf.GetLength()) {
        pFileWrite->WriteBlock(m_Buf.GetBuffer(), m_Buf.GetLength());
        m_Buf.Clear();
    }
    m_Buf << FX_BSTRC("<");
    if (!pElement->m_QSpaceName.IsEmpty()) {
        m_Buf << pElement->m_QSpaceName << ":";
    }
    m_Buf << pElement->m_TagName;
    int attr_count = pElement->m_AttrMap.GetSize();
    int i;
    for (i = 0; i < attr_count; i ++) {
        CXML_AttrItem& item = pElement->m_AttrMap.GetAt(i);
        m_Buf << FX_BSTRC(" ");
        if (!item.m_QSpaceName.IsEmpty()) {
            m_Buf << item.m_QSpaceName << ":";
        }
        m_Buf << item.m_AttrName << FX_BSTRC("=\"");
        CFX_ByteString valueStr;
        XML_EncodeString(item.m_Value, valueStr);
        m_Buf << valueStr;
        m_Buf << FX_BSTRC("\"");
    }
    if (pElement->m_Children.GetSize() == 0) {
        m_Buf << FX_BSTRC("/>\r\n");
        return;
    }
    m_Buf << FX_BSTRC(">");
    for (i = 0; i < pElement->m_Children.GetSize(); i += 2) {
        CXML_Element::ChildType type = (CXML_Element::ChildType)(FX_UINTPTR)pElement->m_Children.GetAt(i);
        FX_LPVOID p = pElement->m_Children.GetAt(i + 1);
        if (type == CXML_Element::Content) {
            CXML_Content* pContent = (CXML_Content*)p;
            if(pContent->m_bCDATA) {
                CFX_WideString &content = pContent->m_Content;
                CFX_ByteString utf8Str;
                utf8Str = FX_UTF8Encode(content, content.GetLength());
                m_Buf << FX_BSTRC("<![CDATA[") << utf8Str << FX_BSTRC("]]>");
            } else {
                CFX_ByteString utf8Str;
                XML_EncodeContent(pContent->m_Content, utf8Str, m_KeepSpecialCode);
                m_Buf << utf8Str;
            }
        } else if (type == CXML_Element::Element) {
            CXML_Element* pSubElement = (CXML_Element*)p;
            ComposeElement(pSubElement, pFileWrite);
        }
    }
    m_Buf << FX_BSTRC("</");
    if (!pElement->m_QSpaceName.IsEmpty()) {
        m_Buf << pElement->m_QSpaceName << ":";
    }
    m_Buf << pElement->m_TagName << FX_BSTRC(">\r\n");
}
CFX_ByteString CXML_Element::OutputStream(FX_BOOL KeepSpecialCode)
{
    CXML_Composer composer(KeepSpecialCode);
    CFX_ByteString xmlStream;
    composer.Compose(this, NULL, xmlStream);
    CFX_ByteString result(xmlStream);
    return result;
}
void CXML_Element::OutputStream(CFX_ByteString &xmlStream, FX_BOOL KeepSpecialCode)
{
    CXML_Composer composer(KeepSpecialCode);
    composer.Compose(this, NULL, xmlStream);
}
void CXML_Element::OutputStream(IFX_FileWrite *pFileWrite, FX_BOOL KeepSpecialCode)
{
    if (!pFileWrite) {
        return;
    }
    CXML_Composer composer(KeepSpecialCode);
    CFX_ByteString xmlStream;
    composer.Compose(this, pFileWrite, xmlStream);
}
