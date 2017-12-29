#ifndef _FPDF_SERIAL_
#define _FPDF_SERIAL_
#ifndef _FPDF_OBJECTS_
#include "fpdf_objects.h"
#endif
#ifndef _FPDF_PARSER_
#include "fpdf_parser.h"
#endif
class CPDF_ObjectStream;
class CPDF_XRefStream;
CFX_ByteTextBuf& operator << (CFX_ByteTextBuf& buf, const CPDF_Object* pObj);
CFX_ByteTextBuf& SerializePDFObjectWithObjMapper(CFX_ByteTextBuf& buf, const CPDF_Object* pObj, CFX_DWordArray* pObjNumMap);
class CPDF_CreatorOption : public CFX_Object
{
public:
	virtual ~CPDF_CreatorOption() {}

	virtual IFX_FileStream*		GetTempFile(CPDF_Object* pObj) = 0;
	virtual void				ReleaseTempFile(IFX_FileStream* pFile) = 0;
};
#define FPDFCREATE_INCREMENTAL		1
#define FPDFCREATE_NO_ORIGINAL		2
#define FPDFCREATE_PROGRESSIVE		4
#define FPDFCREATE_OBJECTSTREAM		8
class CPDF_Creator : public CFX_Object
{
public:

    explicit CPDF_Creator(CPDF_Document* pDoc);

    ~CPDF_Creator();

    void				SetCustomSecurity(CPDF_Dictionary* pEncryptDict, CPDF_CryptoHandler* pCryptoHandler,
                                          FX_BOOL bEncryptMetadata);


    void				RemoveSecurity();

    FX_BOOL				Create(FX_LPCWSTR filename, FX_DWORD flags = 0);

    FX_BOOL				Create(FX_LPCSTR filename, FX_DWORD flags = 0);

    FX_BOOL				Create(IFX_StreamWrite* pFile, FX_DWORD flags = 0);

    FX_INT32			Continue(IFX_Pause *pPause = NULL);

    FX_BOOL				SetFileVersion(FX_INT32 fileVersion = 17);

    void				SetProgressiveEncryptHandler(CPDF_ProgressiveEncryptHandler* pHandler);
    void				SetCreatorOption(CPDF_CreatorOption* pOption);

protected:

    CPDF_Document*		m_pDocument;

    CPDF_Parser*		m_pParser;

    FX_BOOL				m_bCompress;

    FX_BOOL				m_bSecurityChanged;

    CPDF_Dictionary*	m_pEncryptDict;
    FX_DWORD			m_dwEnryptObjNum;

    FX_BOOL				m_bStandardSecurity;

    CPDF_CryptoHandler*	crypto_handler_;
    FX_BOOL				m_bNewCrypto;

    FX_BOOL				m_bEncryptMetadata;

    CPDF_Object*		m_pMetadata;

    CPDF_XRefStream*	m_pXRefStream;

    FX_INT32			m_ObjectStreamSize;

    FX_DWORD			m_dwLastObjNum;
    FX_BOOL				Create(FX_DWORD flags);
    void				ResetStandardSecurity();
    void				Clear();
    FX_INT32			WriteDoc_Stage1(IFX_Pause *pPause);
    FX_INT32			WriteDoc_Stage2(IFX_Pause *pPause);
    FX_INT32			WriteDoc_Stage3(IFX_Pause *pPause);
    FX_INT32			WriteDoc_Stage4(IFX_Pause *pPause);

    CFX_FileBufferArchive	m_File;

    FX_FILESIZE			m_Offset;
    FX_FILESIZE			m_FilePos;
    FX_FILESIZE			m_MaxOffset;
    void				InitOldObjNumOffsets();
    void				InitNewObjNumOffsets();
    void				AppendNewObjNum(FX_DWORD objbum);
    FX_INT32			WriteOldIndirectObject(FX_DWORD objnum);
    FX_INT32			WriteOldObjs(IFX_Pause *pPause);
    FX_INT32			WriteNewObjs(FX_BOOL bIncremental, IFX_Pause *pPause);
    FX_INT32			WriteIndirectObj(const CPDF_Object* pObj);
    FX_INT32			WriteDirectObj(FX_DWORD objnum, const CPDF_Object* pObj, FX_BOOL bEncrypt = TRUE);
    FX_INT32			WriteIndirectObjectToStream(FX_DWORD objnum, const CPDF_Object* pObj);
    FX_INT32			WriteIndirectObj(FX_DWORD objnum, const CPDF_Object* pObj);
    FX_INT32			WriteIndirectObjectToStream(FX_DWORD objnum, FX_LPCBYTE pBuffer, FX_DWORD dwSize);
    FX_INT32			AppendObjectNumberToXRef(FX_DWORD objnum);
    void				InitID(FX_BOOL bDefault = TRUE);
    FX_INT32			WriteStream(const CPDF_Object* pStream, FX_DWORD objnum, CPDF_CryptoHandler* pCrypto);
    FX_INT32			WriteStream(const CPDF_Object* pStream, FX_DWORD objnum);
	FX_INT32			WriteStreamWithOption(const CPDF_Object* pStream, FX_DWORD objnum);
    virtual FX_WORD		GetObjectVersion(FX_DWORD objNum);

    FX_INT32			m_iStage;
    FX_DWORD			m_dwFlags;
    FX_POSITION			m_Pos;
    FX_FILESIZE			m_XrefStart;

    CFX_FileSizeListArray	m_ObjectOffset;

    CFX_DWordListArray		m_ObjectSize;
    CFX_DWordArray		m_NewObjNumArray;

    CPDF_Array*			m_pIDArray;
    CPDF_ProgressiveEncryptHandler*	m_pProgressiveEncrypt;
    CPDF_CreatorOption*	m_pOption;
    friend class CPDF_ObjectStream;
    friend class CPDF_XRefStream;
    friend FX_INT32 PDF_CreatorAppendObject(CPDF_Creator* pCreator, const CPDF_Object* pObj, CFX_FileBufferArchive *pFile, FX_FILESIZE& offset, const CFX_WordArray* pObjVersion);
    friend FX_FILESIZE PDF_CreatorWriteTrailer(CPDF_Creator* pCreator, CPDF_Document* pDocument, CFX_FileBufferArchive* pFile, CPDF_Array* pIDArray, FX_BOOL bCompress);
    CFX_ByteString	GenerateFileVersion(FX_INT32 fileVersion);
};
class IPDF_WrapperCreator
{
public:

    virtual void		Release() = 0;

    virtual void		SetWrapperData(FX_BSTR bsType, FX_INT32 iVersion, FX_BSTR bsApplication, FX_BSTR bsURI, FX_BSTR bsDescription) = 0;

    virtual FX_BOOL		Create(IFX_StreamWrite* pFile) = 0;
};
IPDF_WrapperCreator*	FPDF_WrapperCreator_Create(CPDF_Document* pWrapperDoc, FX_DWORD dwWrapperOffset);
class IPDF_UnencryptedWrapperCreator
{
public:

    virtual void		Release() = 0;

    virtual void		SetPayloadInfo(FX_WSTR wsSubType, FX_WSTR wsFileName, FX_WSTR wsDescription = FX_WSTRC(L""), FX_FLOAT version_num = 0.f) = 0;

    virtual void		SetPayLoad(IFX_FileRead* pPayload) = 0;

    virtual FX_BOOL		Create(IFX_FileWrite* pFile, FX_DWORD flags = 0) = 0;

    virtual FX_INT32	        Continue(IFX_Pause *pPause = NULL) = 0;
    virtual void		SetCreatorOption(CPDF_CreatorOption* pOption = NULL) = 0;
};
IPDF_UnencryptedWrapperCreator*	FPDF_UnencryptedWrapperCreator_Create(CPDF_Document* pWrapperDoc);
#endif
