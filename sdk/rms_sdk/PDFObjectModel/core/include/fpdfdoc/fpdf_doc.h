#ifndef _FPDF_DOC_H_
#define _FPDF_DOC_H_
#ifndef _FPDF_PARSER_
#include "../fpdfapi/fpdf_parser.h"
#endif
class CPDF_NameTree;
class CPDF_FileSpec;
class CPDF_NameTree : public CFX_Object
{
public:

    CPDF_NameTree(CPDF_Dictionary* pParent, FX_BSTR sCategory);

    int					SetValue(CPDF_Document* pDoc, const CFX_ByteString& csName, CPDF_Object* pValue);

    int					GetCount() const;


    CPDF_Dictionary*	GetRoot() const
    {
        return m_pRoot;
    }

protected:
    CPDF_Dictionary*		m_pParent;

    CPDF_Dictionary*		m_pRoot;
    CFX_ByteString			m_bsCategory;
};
class CPDF_FileSpec : public CFX_Object
{
public:

    CPDF_FileSpec();

    CPDF_FileSpec(CPDF_Object *pObj)
    {
        m_pObj = pObj;
    }

    operator CPDF_Object*() const
    {
        return m_pObj;
    }

    FX_BOOL			IsURL() const;

    FX_BOOL			GetFileName(CFX_WideString &wsFileName) const;

    CPDF_Stream*	GetFileStream() const;

    void			SetFileName(FX_WSTR wsFileName, FX_BOOL bURL = FALSE);

    void			SetEmbeddedFile(CPDF_IndirectObjects *pDocument, IFX_FileRead*pFile, FX_WSTR file_path);
protected:

    CPDF_Object		*m_pObj;
};

#define  CPDF_METADATATYPE_INFO				0
#define  CPDF_METADATATYPE_XML				1
#define  CPDF_METADATATYPE_BOTH				2

#define  FX_ERR_CODE_METADATA_NOTFOUND_KEY			1
#define  FX_ERR_CODE_METADATA_NOTFOUND_KEYVALUE		2

class CPDF_Metadata: public CFX_Object
{
public:
    CPDF_Metadata();
    ~CPDF_Metadata();

    FX_BOOL				LoadDoc(CPDF_Document *pDoc, FX_BOOL bKeepSpecialCode = FALSE);
    FX_BOOL				LoadStream(CPDF_Stream *pStream, FX_BOOL bKeepSpecialCode = FALSE) const;
    CXML_Element*		GetRoot() const;
    CXML_Element*		GetRDF() const;
    FX_INT32			GetAllCustomKeys(CFX_WideStringArray& keys) const;
    FX_BOOL				DeleteCustomKey(FX_WSTR wsItem);
    FX_INT32			GetAllRdfXMLKeys(CFX_WideStringArray& keys);
    FX_INT32			GetRdfXMLString(FX_WSTR wsNamespace, FX_WSTR wsKey, CFX_WideString &wsStr);
    FX_INT32			GetString(FX_WSTR wsItem, CFX_WideString &wsStr, FX_INT32& bUseInfoOrXML) const;
    FX_BOOL				SetString(FX_WSTR wsItem, const CFX_WideString& wsStr);
    FX_INT32			GetStringArray(FX_WSTR wsItem, CFX_WideStringArray &wsStrArray, FX_INT32& bUseInfoOrXML) const;
    FX_BOOL				SetStringArray(FX_WSTR wsItem, const CFX_WideStringArray& wsStrArray);
    FX_BOOL				SetDateTime(FX_WSTR wsItem, const FXCRT_DATETIMEZONE& dt);
    FX_INT32			GetDateTime(FX_WSTR wsItem, FXCRT_DATETIMEZONE& dt,  FX_INT32& bUseInfoOrXML) const;
    FX_BOOL				SyncUpdate();
    FX_BOOL				CreateNewMetadata();

protected:
    FX_INT32			ParseRDF();
    FX_BOOL				CreateDocInfoDict() const;
    FX_BOOL				SetMetadataStrArrayToInfo(FX_BSTR key, const CFX_WideString& keyValue);
    FX_BOOL				SetMetadataStrArrayToXML(FX_BSTR key, const CFX_WideString& keyValue, FX_BOOL bAuthorFirst = FALSE, FX_BOOL bKeywordsPdf = FALSE);
    FX_BOOL				SetDCMetadataStrArrayToXML(FX_BSTR key, const CFX_WideString& keyValue, FX_BOOL bAdd = FALSE, FX_BOOL bAuthorFirst = FALSE);
    FX_BOOL				SetPDFAMetadataStrArrayToXML(FX_BSTR key, const CFX_WideString& keyValue, FX_BOOL bKeywordsNeedParse = TRUE);
    FX_BOOL				SetXMPOrPDFOrPDFXMetadataStrArrayToXML(FX_BSTR key, const CFX_WideString& keyValue, FX_BOOL bKeywordsNeedParse = TRUE);
    FX_INT32			CompareModDT() const;
    FX_INT32			GetStandardMetadataContent(FX_BSTR key, CFX_WideString &keyValue, FX_INT32& bUseInfoOrXML) const;
    FX_INT32			GetStringFromInfo(FX_BSTR key, CFX_WideString& keyValue) const;
    FX_INT32			GetStringFromXML(FX_BSTR key, CFX_WideStringArray& keyArray, FX_INT32& keywordsUsePDFOrDC) const;
    FX_INT32			GetDCMetadataStringFromXML(FX_BSTR key, CFX_WideStringArray& keyArray) const;
    FX_INT32			GetPDFAMetadataStringFromXML(FX_BSTR key, CFX_WideStringArray& keyArray) const;
    FX_INT32			GetXMPOrPDFOrPDFXMetadataStringFromXML(FX_BSTR key, CFX_WideStringArray& keyArray, FX_BOOL bPDF = FALSE) const;
    FX_BOOL				SyncUpdateMetadata();
    FX_BOOL				SyncUpdateInfoFromXML();
    FX_BOOL				SyncUpdateXMLFromInfo();
    void				AddCreationDateText();
    void				SyncUpdateCustomMetadataInfoFromXML(FX_BOOL bInfo = FALSE);
    void				SyncUpdateCustomMetadataXMLFromInfo();

protected:
    FX_LPVOID			m_pData;
    FX_BOOL				m_bRDFParsed;
    CFX_PtrArray		m_XMLValueArray;
};

#endif
