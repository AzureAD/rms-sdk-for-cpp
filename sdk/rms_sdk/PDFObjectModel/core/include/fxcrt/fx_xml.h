#ifndef _FX_XML_H_
#define _FX_XML_H_
#ifndef _FX_BASIC_H_
#include "fx_basic.h"
#endif
class CXML_AttrItem : public CFX_Object
{
public:
    CFX_ByteString	m_QSpaceName;
    CFX_ByteString	m_AttrName;
    CFX_WideString	m_Value;
};
class CXML_AttrMap : public CFX_Object
{
public:
    CXML_AttrMap()
    {
        m_pMap = NULL;
    }
    ~CXML_AttrMap()
    {
        RemoveAll();
    }
    const CFX_WideString*	Lookup(const CFX_ByteString& space, const CFX_ByteString& name) const;
    void					SetAt(const CFX_ByteString& space, const CFX_ByteString& name, const CFX_WideString& value);
    void					RemoveAt(const CFX_ByteString& space, const CFX_ByteString& name);
    void					RemoveAll();
    int						GetSize() const;
    CXML_AttrItem&			GetAt(int index) const;
    CFX_ObjectArray<CXML_AttrItem>*	m_pMap;
};
class CXML_Content : public CFX_Object
{
public:
    CXML_Content() : m_bCDATA(FALSE), m_Content() {}
    ~CXML_Content() {}
    void	Set(FX_BOOL bCDATA, const CFX_WideString& content)
    {
        m_bCDATA = bCDATA;
        m_Content = content;
    }
    FX_BOOL			m_bCDATA;
    CFX_WideString	m_Content;
};
class CXML_Element : public CFX_Object
{
public:
    static CXML_Element*	Parse(const void* pBuffer, size_t size, FX_BOOL bSaveSpaceChars = FALSE, FX_FILESIZE* pParsedSize = NULL, FX_BOOL bSaveSpecialChar = FALSE);
    static CXML_Element*	Parse(IFX_FileRead *pFile, FX_BOOL bSaveSpaceChars = FALSE, FX_FILESIZE* pParsedSize = NULL, FX_BOOL bSaveSpecialChar = FALSE);
    static CXML_Element*	Parse(IFX_BufferRead *pBuffer, FX_BOOL bSaveSpaceChars = FALSE, FX_FILESIZE* pParsedSize = NULL, FX_BOOL bSaveSpecialChar = FALSE);
    CXML_Element(const CFX_ByteString& qSpace, const CFX_ByteString& tagName);
    CXML_Element(const CFX_ByteString& qTagName);
    CXML_Element();
    ~CXML_Element();
    void	Empty();
    CXML_Element*	Clone();
    CFX_ByteString			GetTagName(FX_BOOL bQualified = FALSE) const;
    void					GetTagName(CFX_ByteString &tagName, FX_BOOL bQualified = FALSE) const;
    CFX_ByteString			GetNamespace(FX_BOOL bQualified = FALSE) const;
    void					GetNamespace(CFX_ByteString &nameSpace, FX_BOOL bQualified = FALSE) const;
    CFX_ByteString			GetNamespaceURI(const CFX_ByteString& qName) const;
    void					GetNamespaceURI(const CFX_ByteString& qName, CFX_ByteString &uri) const;
    CXML_Element*			GetParent() const
    {
        return m_pParent;
    }
    FX_DWORD				CountAttrs() const
    {
        return m_AttrMap.GetSize();
    }
    void					GetAttrByIndex(int index, CFX_ByteString &space, CFX_ByteString &name, CFX_WideString &value) const;
    FX_BOOL					HasAttr(const CFX_ByteString& qName) const;
    FX_BOOL					GetAttrValue(const CFX_ByteString& name, CFX_WideString& attribute) const;
    CFX_WideString			GetAttrValue(const CFX_ByteString& name) const
    {
        CFX_WideString attr;
        GetAttrValue(name, attr);
        return attr;
    }
    const CFX_WideString*	GetAttrValuePtr(const CFX_ByteString& name) const;
    FX_BOOL					GetAttrValue(const CFX_ByteString& space, const CFX_ByteString& name, CFX_WideString& attribute) const;
    CFX_WideString			GetAttrValue(const CFX_ByteString& space, const CFX_ByteString& name) const
    {
        CFX_WideString attr;
        GetAttrValue(space, name, attr);
        return attr;
    }
    const CFX_WideString*	GetAttrValuePtr(const CFX_ByteString& space, const CFX_ByteString& name) const;
    FX_BOOL					GetAttrInteger(const CFX_ByteString& name, int& attribute) const;
    int						GetAttrInteger(const CFX_ByteString& name) const
    {
        int attr = 0;
        GetAttrInteger(name, attr);
        return attr;
    }
    FX_BOOL					GetAttrInteger(const CFX_ByteString& space, const CFX_ByteString& name, int& attribute) const;
    int						GetAttrInteger(const CFX_ByteString& space, const CFX_ByteString& name) const
    {
        int attr = 0;
        GetAttrInteger(space, name, attr);
        return attr;
    }
    FX_BOOL					GetAttrFloat(const CFX_ByteString& name, FX_FLOAT& attribute) const;
    FX_FLOAT				GetAttrFloat(const CFX_ByteString& name) const
    {
        FX_FLOAT attr = 0;
        GetAttrFloat(name, attr);
        return attr;
    }
    FX_BOOL					GetAttrFloat(const CFX_ByteString& space, const CFX_ByteString& name, FX_FLOAT& attribute) const;
    FX_FLOAT				GetAttrFloat(const CFX_ByteString& space, const CFX_ByteString& name) const
    {
        FX_FLOAT attr = 0;
        GetAttrFloat(space, name, attr);
        return attr;
    }
    FX_DWORD				CountChildren() const;
    enum ChildType { Invalid, Element, Content};
    ChildType				GetChildType(FX_DWORD index) const;
    CFX_WideString			GetContent(FX_DWORD index) const;
    const CFX_WideString*	GetContentPtr(FX_DWORD index) const;
    CXML_Element*			GetElement(FX_DWORD index) const;
    CXML_Element*			GetElement(const CFX_ByteString& space, const CFX_ByteString& tag) const
    {
        return GetElement(space, tag, 0);
    }
    int						CountElements(const CFX_ByteString& space, const CFX_ByteString& tag) const;
    CXML_Element*			GetElement(const CFX_ByteString& space, const CFX_ByteString& tag, int index) const;
    FX_DWORD				FindElement(CXML_Element *pChild) const;
    void					SetTag(const CFX_ByteString& qSpace, const CFX_ByteString& tagname);
    void					SetTag(const CFX_ByteString& qTagName);
    void					SetAttrValue(const CFX_ByteString& name, const CFX_WideString& value);
    void					SetAttrValue(const CFX_ByteString& name, int value);
    void					SetAttrValue(const CFX_ByteString& name, FX_FLOAT value);
    void					RemoveAttr(const CFX_ByteString& name);
    void					AddChildElement(CXML_Element* pElement);
    void					AddChildContent(const CFX_WideString& content, FX_BOOL bCDATA = FALSE);
    void					InsertChildElement(FX_DWORD index, CXML_Element* pElement);
    void					InsertChildContent(FX_DWORD index, const CFX_WideString& content, FX_BOOL bCDATA = FALSE);
    void					RemoveChildren();
    void					RemoveChild(FX_DWORD index);
    CFX_ByteString			OutputStream(FX_BOOL KeepSpecialCode = FALSE);
    void					OutputStream(CFX_ByteString &xmlStream, FX_BOOL KeepSpecialCode = FALSE);
    void					OutputStream(IFX_FileWrite *pFileWrite, FX_BOOL KeepSpecialCode = FALSE);
protected:
    CXML_Element*			m_pParent;
    CFX_ByteString			m_QSpaceName;
    CFX_ByteString			m_TagName;
    CXML_AttrMap			m_AttrMap;
    CFX_PtrArray			m_Children;
    friend class CXML_Parser;
    friend class CXML_Composer;
};
#endif
