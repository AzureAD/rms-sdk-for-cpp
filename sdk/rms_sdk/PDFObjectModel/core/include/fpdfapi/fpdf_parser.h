#ifndef _FPDF_PARSER_
#define _FPDF_PARSER_
#ifndef _FX_BASIC_H_
#include "../fxcrt/fx_ext.h"
#endif
#ifndef _FPDF_OBJECTS_
#include "fpdf_objects.h"
#endif
class CPDF_Document;
class CPDF_WrapperDoc;
class IPDF_DocParser;
class CPDF_Parser;
class CPDF_SecurityHandler;
class CPDF_StandardSecurityHandler;
class CPDF_CryptoHandler;
class CPDF_Object;
class IFX_FileRead;
class CFDF_Parser;
class CPDF_ModuleMgr;
class CFX_PrivateData;
#define FPDFPERM_PRINT			0x0004
#define FPDFPERM_MODIFY			0x0008
#define FPDFPERM_EXTRACT		0x0010
#define FPDFPERM_ANNOT_FORM		0x0020
#define FPDFPERM_FILL_FORM		0x0100
#define FPDFPERM_EXTRACT_ACCESS	0x0200
#define FPDFPERM_ASSEMBLE		0x0400
#define FPDFPERM_PRINT_HIGH		0x0800
#define FPDF_PAGE_MAX_NUM		0xFFFFF
class CPDF_Document : public CFX_PrivateData, public CPDF_IndirectObjects
{
public:

    explicit CPDF_Document(IPDF_DocParser* pParser);

    CPDF_Document();

    ~CPDF_Document();

    IPDF_DocParser*			GetParser() const
    {
        return m_pParser;
    }

    CPDF_Dictionary*		GetRoot() const
    {
        return m_pRootDict;
    }

    CPDF_Dictionary*		GetInfo() const
    {
        return m_pInfoDict;
    }

    void					GetID(CFX_ByteString& id1, CFX_ByteString& id2) const
    {
        id1 = m_ID1;
        id2 = m_ID2;
    }

    int						GetPageCount() const;

    FX_DWORD				GetUserPermissions(FX_BOOL bCheckRevision = FALSE) const;

    FX_BOOL					IsOwner() const;




    void					LoadDoc();

    FX_BOOL					SetFileVersion(int fileVersion);

    FX_INT32				GetFileVersion()
    {
        return m_FileVersion;
    }
protected:

    CPDF_Dictionary*		m_pRootDict;

    CPDF_Dictionary*		m_pInfoDict;

    CFX_ByteString			m_ID1;

    CFX_ByteString			m_ID2;

    FX_BOOL					m_bLinearized;
    FX_BOOL					m_bMetaDataFlate;

    FX_INT32				m_FileVersion;

    CFX_DWordArray			m_PageList;

    int						_GetPageCount() const;
    friend class			CPDF_Creator;
    friend class			CPDF_Parser;



};
#define  PDF_WRAPPERTYPE_NO		 0
#define  PDF_WRAPPERTYPE_FOXIT	 1
#define  PDF_WRAPPERTYPE_PDF2	 2
class CPDF_WrapperDoc : public CFX_Object
{
public:
    CPDF_WrapperDoc(CPDF_Document* pDoc) : m_pDoc(pDoc)
        , m_pSteamFilter(NULL)
        , m_pPayload(NULL)
        , m_pBuffer(NULL)
    {}
    ~CPDF_WrapperDoc()
    {
        if (m_pSteamFilter) {
            delete m_pSteamFilter;
        }
        m_pSteamFilter = NULL;
        if (m_pBuffer) {
            FX_Free(m_pBuffer);
        }
        m_pBuffer = NULL;
    }

    FX_INT32				GetWrapperType() const;

    FX_BOOL					GetCryptographicFilter(CFX_WideString &graphic_filter, FX_FLOAT &version_num) const;

    FX_FILESIZE				GetPayLoadSize() const;

    FX_BOOL					GetPayloadFileName(CFX_WideString &wsFileName) const;

    FX_BOOL					StartGetPayload(IFX_FileWrite* pPayload, IFX_Pause *pPause = NULL);


    FX_INT32				Continue(IFX_Pause *pPause = NULL);
protected:
    CPDF_Document*			m_pDoc;
    CPDF_StreamFilter*		m_pSteamFilter;
    IFX_FileWrite*			m_pPayload;
    FX_LPBYTE				m_pBuffer;
    static const FX_INT32 STREAM_BUFSIZE = 20480;
};

#define PDFWORD_EOF			0
#define PDFWORD_NUMBER		1
#define PDFWORD_TEXT		2
#define PDFWORD_DELIMITER	3
#define PDFWORD_NAME		4
class CPDF_SyntaxParser : public CFX_Object
{
public:

    CPDF_SyntaxParser();

    ~CPDF_SyntaxParser();

    void				InitParser(IFX_FileRead* pFileAccess, FX_DWORD HeaderOffset);

    FX_FILESIZE			SavePos() const
    {
        return m_Pos;
    }

    void				RestorePos(FX_FILESIZE pos)
    {
        m_Pos = pos;
    }

    CPDF_Object*		GetObject(CPDF_IndirectObjects* pObjList, FX_DWORD objnum, FX_DWORD gennum, int level, struct PARSE_CONTEXT* pContext = NULL, FX_BOOL bDecrypt = TRUE);


    CPDF_Object*		GetObjectByStrict(CPDF_IndirectObjects* pObjList, FX_DWORD objnum, FX_DWORD gennum, int level, struct PARSE_CONTEXT* pContext = NULL);

    int					GetDirectNum();
    FX_INT64			GetDirectNum64();

    CFX_ByteString		GetString(FX_DWORD objnum, FX_DWORD gennum);

    CFX_ByteString		GetName();

    CFX_ByteString		GetKeyword();

    void				GetBinary(FX_BYTE* buffer, FX_DWORD size);

    void				ToNextLine();

    void				ToNextWord();

    FX_BOOL				SearchWord(FX_BSTR word, FX_BOOL bWholeWord, FX_BOOL bForward, FX_FILESIZE limit);

    int					SearchMultiWord(FX_BSTR words, FX_BOOL bWholeWord, FX_FILESIZE limit);

    FX_FILESIZE			FindTag(FX_BSTR tag, FX_FILESIZE limit);

    void				SetEncrypt(CPDF_CryptoHandler* pCryptoHandler)
    {
        crypto_handler_ = pCryptoHandler;
    }

    FX_BOOL				IsEncrypted() const
    {
        return crypto_handler_ != NULL;
    }

    FX_BOOL				GetCharAt(FX_FILESIZE pos, FX_BYTE& ch);

    FX_BOOL				ReadBlock(FX_BYTE* pBuf, FX_DWORD size);

    CFX_ByteString		GetNextWord(FX_BOOL& bIsNumber);
    FX_DWORD			GetHeaderOffset() const
    {
        return m_HeaderOffset;
    }
protected:

    virtual FX_BOOL				GetNextChar(FX_BYTE& ch);

    FX_BOOL				GetCharAtBackward(FX_FILESIZE pos, FX_BYTE& ch);

    void				GetNextWord();

    FX_BOOL				IsWholeWord(FX_FILESIZE startpos, FX_FILESIZE limit, FX_LPCBYTE tag, FX_DWORD taglen);

    CFX_ByteString		ReadString();

    CFX_ByteString		ReadHexString();

    CPDF_Stream*		ReadStream(CPDF_Dictionary* pDict, PARSE_CONTEXT* pContext, FX_DWORD objnum, FX_DWORD gennum);

    FX_FILESIZE			m_Pos;

    FX_BOOL				m_bFileStream;

    int					m_MetadataObjnum;
    CFX_DWordArray*		m_pRefStreamObjNums;

    IFX_FileRead*		m_pFileAccess;

    FX_DWORD			m_HeaderOffset;

    FX_FILESIZE			m_FileLen;

    FX_BYTE*			m_pFileBuf;

    FX_DWORD			m_BufSize;

    FX_FILESIZE			m_BufOffset;

    CPDF_CryptoHandler*	crypto_handler_;

    FX_BYTE				m_WordBuffer[257];

    FX_DWORD			m_WordSize;

    FX_BOOL				m_bIsNumber;

    FX_FILESIZE			m_dwWordPos;

    CFX_DWordArray      m_dwRef;
    FX_INT32			m_Level;
    FX_FILESIZE			m_TotalMemoryStreamSize;
    FX_INT32			m_iStatus;
    friend class		CPDF_Parser;
};

#define PDFPARSE_TYPEONLY	1
#define PDFPARSE_NOSTREAM	2
struct PARSE_CONTEXT {

    FX_BOOL		m_Flags;

    FX_FILESIZE	m_DictStart;

    FX_FILESIZE	m_DictEnd;

    FX_FILESIZE	m_DataStart;

    FX_FILESIZE	m_DataEnd;

    CPDF_Object* m_pRootContainer;
};
class IPDF_DocParser : public CFX_Object
{
public:

    virtual FX_DWORD	GetRootObjNum() = 0;

    virtual FX_DWORD	GetInfoObjNum() = 0;

    virtual CPDF_Object*	ParseIndirectObject(CPDF_IndirectObjects* pObjList, FX_DWORD objnum, PARSE_CONTEXT* pContext = NULL) = 0;

    virtual FX_DWORD	GetLastObjNum() = 0;

    virtual CPDF_Array*	GetIDArray() = 0;

    virtual CPDF_Dictionary*	GetEncryptDict() = 0;

    FX_BOOL				IsEncrypted()
    {
        return GetEncryptDict() != NULL;
    }

    virtual FX_DWORD	GetPermissions(FX_BOOL bCheckRevision = FALSE) = 0;

    virtual FX_BOOL		IsOwner() = 0;

    virtual void		DeleteIndirectObject(FX_DWORD objnum) = 0;
};

#define PDFPARSE_ERROR_SUCCESS		0
#define PDFPARSE_ERROR_FILE			1
#define PDFPARSE_ERROR_FORMAT		2
#define PDFPARSE_ERROR_PASSWORD		3
#define PDFPARSE_ERROR_HANDLER		4
#define PDFPARSE_ERROR_CERT			5
#define PDFPARSE_ERROR_TOBECONTINUED 6
#define PDFPARSE_ERROR_FAILED		7
#define PDFPARSE_ERROR_DATA_SCARCITY 8
class CPDF_Parser : public IPDF_DocParser
{
public:

    CPDF_Parser();

    ~CPDF_Parser();

    FX_DWORD			StartParse(FX_LPCSTR filename, FX_BOOL bReParse = FALSE);

    FX_DWORD			StartParse(FX_LPCWSTR filename, FX_BOOL bReParse = FALSE);

    FX_DWORD			StartParse(IFX_FileRead* pFile, FX_BOOL bReParse = FALSE, FX_BOOL bOwnFileRead = TRUE);

    void				CloseParser(FX_BOOL bReParse = FALSE);

    virtual FX_DWORD	GetPermissions(FX_BOOL bCheckRevision = FALSE);

    virtual FX_BOOL		IsOwner();

    void				SetPassword(const FX_CHAR* password);

    CFX_ByteString		GetPassword() const
    {
        return m_Password;
    }

    CPDF_SecurityHandler* GetSecurityHandler()
    {
        return m_pSecurityHandler;
    }

    CPDF_CryptoHandler*	GetCryptoHandler()
    {
        return m_Syntax.crypto_handler_;
    }

    void				SetSecurityHandler(CPDF_SecurityHandler* pSecurityHandler, FX_BOOL bForced = FALSE);

    CPDF_Dictionary*	GetTrailer()
    {
        return m_pTrailer;
    }

    FX_FILESIZE			GetLastXRefOffset() const
    {
        return m_LastXRefOffset;
    }

    FX_BOOL				IsParsingLinearized() const
    {
        return m_bLinearized;
    }

    FX_FILESIZE			GetLinearizedFirstPageXRefOffset() const
    {
        return m_LinearizedFirstPageXRefOffset;
    }

    CPDF_Document*		GetDocument()
    {
        return m_pDocument;
    }
    virtual FX_DWORD	GetRootObjNum();
    virtual FX_DWORD	GetInfoObjNum();
    virtual CPDF_Array*	GetIDArray();
    virtual CPDF_Dictionary*	GetEncryptDict()
    {
        return m_pEncryptDict;
    }
    virtual CPDF_Object*		ParseIndirectObject(CPDF_IndirectObjects* pObjList, FX_DWORD objnum, PARSE_CONTEXT* pContext = NULL);
    virtual FX_DWORD	GetLastObjNum();
    virtual void		DeleteIndirectObject(FX_DWORD objnum);

    FX_FILESIZE			GetObjectOffset(FX_DWORD objnum);

    FX_FILESIZE			GetObjectSize(FX_DWORD objnum);

    int					GetObjectVersion(FX_DWORD objnum);

    void				GetIndirectBinary(FX_DWORD objnum, FX_BYTE*& pBuffer, FX_DWORD& size);

    FX_BOOL				GetFileStreamOption() const
    {
        return m_Syntax.m_bFileStream;
    }

    void				SetFileStreamOption(FX_BOOL b)
    {
        m_Syntax.m_bFileStream = b;
    }

    IFX_FileRead*		GetFileAccess() const
    {
        return m_Syntax.m_pFileAccess;
    }

    int					GetFileVersion() const
    {
        return m_FileVersion;
    }

    FX_BOOL				IsXRefStream() const
    {
        return m_bXRefStream;
    }
    CPDF_Object*		ParseIndirectObjectAt(CPDF_IndirectObjects* pObjList, FX_FILESIZE pos, FX_DWORD objnum,
            struct PARSE_CONTEXT* pContext);

    CPDF_Object*		ParseIndirectObjectAtByStrict(CPDF_IndirectObjects* pObjList, FX_FILESIZE pos, FX_DWORD objnum,
            struct PARSE_CONTEXT* pContext, FX_FILESIZE *pResultPos);
protected:

    CPDF_Document*		m_pDocument;

    CPDF_SyntaxParser	m_Syntax;
    FX_BOOL				m_bOwnFileRead;
    CPDF_Object*		ParseDirect(CPDF_Object* pObj);

    FX_BOOL				LoadAllCrossRefV4(FX_FILESIZE pos);

    FX_BOOL				LoadAllCrossRefV5(FX_FILESIZE pos);

    FX_BOOL				LoadCrossRefV4(FX_FILESIZE pos, FX_FILESIZE streampos, FX_BOOL bSkip, FX_BOOL bFirst);

    FX_BOOL				LoadCrossRefV5(FX_FILESIZE pos, FX_FILESIZE& prev, FX_BOOL bMainXRef);

    CPDF_Dictionary*	LoadTrailerV4();

    FX_BOOL				RebuildCrossRef();

    FX_DWORD			SetEncryptHandler();

    void				ReleaseEncryptHandler();
    FX_BOOL				m_bInitObjectStreamMap;

    CFX_MapPtrToPtr		m_ObjectStreamMap;

    FX_DWORD			m_nTimeCount;

    CFX_MapPtrToPtr		m_ObjStmTimeCountMap;

    CPDF_StreamAcc*		GetObjectStream(FX_DWORD number);

    FX_BOOL				IsLinearizedFile(IFX_FileRead* pFileAccess, FX_DWORD offset);



    int					m_FileVersion;

    CPDF_Dictionary*	m_pTrailer;

    CPDF_Dictionary*	m_pEncryptDict;
    void SetEncryptDictionary(CPDF_Dictionary* pDict);

    FX_FILESIZE			m_LastXRefOffset;

    FX_BOOL				m_bLinearized;

    FX_FILESIZE			m_LinearizedFirstPageXRefOffset;

    FX_BOOL				m_bXRefStream;
    CFX_DWordArray		m_RefStreamObjNums;


    CPDF_SecurityHandler*	m_pSecurityHandler;

    FX_BOOL					m_bForceUseSecurityHandler;

    CFX_ByteString		m_Password;

    CFX_FileSizeArray	m_CrossRef;

    CFX_ByteArray		m_V5Type;

    CFX_FileSizeArray	m_SortedOffset;

    CFX_WordArray		m_ObjVersion;
    CFX_ArrayTemplate<CPDF_Dictionary *>	m_Trailers;

    FX_BOOL				m_bVersionUpdated;

    FX_BOOL				m_bRebuildXRef;

    CPDF_Object*		m_pLinearized;

    FX_DWORD			m_dwFirstPageNo;

    FX_DWORD			m_dwXrefStartObjNum;

    FX_INT32				m_nLevel;
    friend class		CPDF_Creator;
    friend class		CPDF_PDFVersionChecker;
private:
    FX_BOOL				GetIndirectBinary(FX_DWORD objnum, FX_LPBYTE& pBuffer, FX_DWORD& buffSize,
                                          FX_DWORD& leftSize, FX_FILESIZE& savedPos);
};
#define FXCIPHER_NONE	0
#define FXCIPHER_RC4	1
#define FXCIPHER_AES	2
#define FXCIPHER_AES2   3
class CPDF_SecurityHandler : public CFX_Object
{
public:

    virtual ~CPDF_SecurityHandler() {}

    virtual FX_BOOL		OnInit(CPDF_Parser* pParser, CPDF_Dictionary* pEncryptDict) = 0;

    virtual FX_DWORD	GetPermissions() = 0;

    virtual FX_BOOL		IsOwner() = 0;

    virtual FX_BOOL		GetCryptInfo(int& cipher, FX_LPCBYTE& buffer, int& keylen) = 0;

    virtual FX_BOOL		IsMetadataEncrypted()
    {
        return TRUE;
    }

    virtual CPDF_CryptoHandler*	CreateCryptoHandler() = 0;

    virtual CPDF_StandardSecurityHandler* GetStandardHandler()
    {
        return NULL;
    }
};
#define PDF_ENCRYPT_CONTENT				0
class CPDF_StandardSecurityHandler : public CPDF_SecurityHandler
{
public:
    CPDF_StandardSecurityHandler();

    virtual ~CPDF_StandardSecurityHandler();
    virtual FX_BOOL		OnInit(CPDF_Parser* pParser, CPDF_Dictionary* pEncryptDict);
    virtual FX_DWORD	GetPermissions();
    virtual FX_BOOL		IsOwner()
    {
        return m_bOwner;
    }
    virtual FX_BOOL		GetCryptInfo(int& cipher, FX_LPCBYTE& buffer, int& keylen);
    virtual FX_BOOL		IsMetadataEncrypted();
    virtual CPDF_CryptoHandler*	CreateCryptoHandler();
    virtual CPDF_StandardSecurityHandler* GetStandardHandler()
    {
        return this;
    }

    void				OnCreate(CPDF_Dictionary* pEncryptDict, CPDF_Array* pIdArray,
                                 FX_LPCBYTE user_pass, FX_DWORD user_size,
                                 FX_LPCBYTE owner_pass, FX_DWORD owner_size, FX_DWORD type = PDF_ENCRYPT_CONTENT);

    void				OnCreate(CPDF_Dictionary* pEncryptDict, CPDF_Array* pIdArray,
                                 FX_LPCBYTE user_pass, FX_DWORD user_size, FX_DWORD type = PDF_ENCRYPT_CONTENT);

    CFX_ByteString		GetUserPassword(FX_LPCBYTE owner_pass, FX_DWORD pass_size);
    CFX_ByteString		GetUserPassword(FX_LPCBYTE owner_pass, FX_DWORD pass_size, FX_INT32 key_len);

    int					CheckPassword(FX_LPCBYTE password, FX_DWORD pass_size, FX_BOOL bOwner, FX_LPBYTE key);
    int					CheckPassword(FX_LPCBYTE password, FX_DWORD pass_size, FX_BOOL bOwner, FX_LPBYTE key, int key_len);
private:

    int					m_Version;

    int					m_Revision;

    CPDF_Parser*		m_pParser;

    CPDF_Dictionary*	m_pEncryptDict;

    FX_BOOL				LoadDict(CPDF_Dictionary* pEncryptDict);
    FX_BOOL				LoadDict(CPDF_Dictionary* pEncryptDict, FX_DWORD type, int& cipher, int& key_len);

    FX_BOOL				CheckUserPassword(FX_LPCBYTE password, FX_DWORD pass_size,
                                          FX_BOOL bIgnoreEncryptMeta, FX_LPBYTE key, FX_INT32 key_len);

    FX_BOOL				CheckOwnerPassword(FX_LPCBYTE password, FX_DWORD pass_size, FX_LPBYTE key, FX_INT32 key_len);
    FX_BOOL				AES256_CheckPassword(FX_LPCBYTE password, FX_DWORD size, FX_BOOL bOwner, FX_LPBYTE key);
    void				AES256_SetPassword(CPDF_Dictionary* pEncryptDict, FX_LPCBYTE password, FX_DWORD size, FX_BOOL bOwner, FX_LPCBYTE key);
    void				AES256_SetPerms(CPDF_Dictionary* pEncryptDict, FX_DWORD permission, FX_BOOL bEncryptMetadata, FX_LPCBYTE key);
    void				OnCreate(CPDF_Dictionary* pEncryptDict, CPDF_Array* pIdArray,
                                 FX_LPCBYTE user_pass, FX_DWORD user_size,
                                 FX_LPCBYTE owner_pass, FX_DWORD owner_size, FX_BOOL bDefault, FX_DWORD type);
    FX_BOOL				CheckSecurity(FX_INT32 key_len);
    FX_BOOL				CheckSecurity(CFX_ByteString password, FX_INT32 key_len);

    FX_BOOL				m_bOwner;

    FX_DWORD			m_Permissions;

    int					m_Cipher;

    FX_BYTE				m_EncryptKey[32];

    int					m_KeyLen;
};
class CPDF_CryptoHandler : public CFX_Object
{
public:

    virtual ~CPDF_CryptoHandler() {}

    virtual FX_BOOL		Init(CPDF_Dictionary* pEncryptDict, CPDF_SecurityHandler* pSecurityHandler) = 0;

    virtual FX_DWORD	DecryptGetSize(FX_DWORD src_size) = 0;

    virtual FX_LPVOID	DecryptStart(FX_DWORD objnum, FX_DWORD gennum) = 0;

    virtual FX_BOOL		DecryptStream(FX_LPVOID context, FX_LPCBYTE src_buf, FX_DWORD src_size, CFX_BinaryBuf& dest_buf) = 0;

    virtual FX_BOOL		DecryptFinish(FX_LPVOID context, CFX_BinaryBuf& dest_buf) = 0;


    virtual FX_DWORD	EncryptGetSize(FX_DWORD objnum, FX_DWORD gennum, FX_LPCBYTE src_buf, FX_DWORD src_size) = 0;

    virtual FX_BOOL		EncryptContent(FX_DWORD objnum, FX_DWORD gennum, FX_LPCBYTE src_buf, FX_DWORD src_size,
                                       FX_LPBYTE dest_buf, FX_DWORD& dest_size) = 0;

    void				Decrypt(FX_DWORD objnum, FX_DWORD gennum, CFX_ByteString& str);
};
class CPDF_StandardCryptoHandler : public CPDF_CryptoHandler
{
public:

    CPDF_StandardCryptoHandler();

    virtual ~CPDF_StandardCryptoHandler();

    FX_BOOL				Init(int cipher, FX_LPCBYTE key, int keylen);
    virtual FX_BOOL		Init(CPDF_Dictionary* pEncryptDict, CPDF_SecurityHandler* pSecurityHandler);
    virtual FX_DWORD	DecryptGetSize(FX_DWORD src_size);
    virtual FX_LPVOID	DecryptStart(FX_DWORD objnum, FX_DWORD gennum);
    virtual FX_BOOL		DecryptStream(FX_LPVOID context, FX_LPCBYTE src_buf, FX_DWORD src_size, CFX_BinaryBuf& dest_buf);
    virtual FX_BOOL		DecryptFinish(FX_LPVOID context, CFX_BinaryBuf& dest_buf);
    virtual FX_DWORD	EncryptGetSize(FX_DWORD objnum, FX_DWORD gennum, FX_LPCBYTE src_buf, FX_DWORD src_size);
    virtual FX_BOOL		EncryptContent(FX_DWORD objnum, FX_DWORD gennum, FX_LPCBYTE src_buf, FX_DWORD src_size,
                                       FX_LPBYTE dest_buf, FX_DWORD& dest_size);
protected:

    virtual void		CryptBlock(FX_BOOL bEncrypt, FX_DWORD objnum, FX_DWORD gennum, FX_LPCBYTE src_buf, FX_DWORD src_size,
                                   FX_LPBYTE dest_buf, FX_DWORD& dest_size);
    virtual FX_LPVOID	CryptStart(FX_DWORD objnum, FX_DWORD gennum, FX_BOOL bEncrypt);
    virtual FX_BOOL		CryptStream(FX_LPVOID context, FX_LPCBYTE src_buf, FX_DWORD src_size, CFX_BinaryBuf& dest_buf, FX_BOOL bEncrypt);
    virtual FX_BOOL		CryptFinish(FX_LPVOID context, CFX_BinaryBuf& dest_buf, FX_BOOL bEncrypt);

    FX_BYTE				m_EncryptKey[32];

    int					m_KeyLen;

    int					m_Cipher;

    FX_LPBYTE			m_pAESContext;
};
class CPDF_ProgressiveEncryptHandler : public CFX_Object
{
public:
    virtual ~CPDF_ProgressiveEncryptHandler() {}

    virtual IFX_FileStream*		GetTempFile() = 0;
    virtual void				ReleaseTempFile(IFX_FileStream* pFile) = 0;
    virtual FX_BOOL				SetCryptoHandler(CPDF_CryptoHandler* pCryptoHandler) = 0;

    virtual FX_DWORD			EncryptGetSize(FX_DWORD objnum, FX_DWORD gennum, FX_LPCBYTE src_buf, FX_DWORD src_size) = 0;

    virtual FX_LPVOID			EncryptStart(FX_DWORD objnum, FX_DWORD gennum, FX_DWORD raw_size, FX_BOOL bFlateEncode) = 0;

    virtual FX_BOOL				EncryptStream(FX_LPVOID context, FX_LPCBYTE src_buf, FX_DWORD src_size, IFX_FileStream* pDest) = 0;

    virtual FX_BOOL				EncryptFinish(FX_LPVOID context, IFX_FileStream* pDest) = 0;

    virtual FX_BOOL				UpdateFilter(CPDF_Dictionary* pDict) = 0;
};
CFX_ByteString PDF_NameDecode(FX_BSTR orig);
CFX_ByteString PDF_NameDecode(const CFX_ByteString& orig);
CFX_ByteString PDF_NameEncode(const CFX_ByteString& orig);
CFX_ByteString PDF_EncodeString(const CFX_ByteString& src, FX_BOOL bHex = FALSE);
CFX_WideString PDF_DecodeText(const CFX_ByteString& str, CFX_CharMap* pCharMap = NULL, const CFX_ByteString& endcoding = "");
CFX_WideString PDF_DecodeText(FX_LPCBYTE pData, FX_DWORD size, CFX_CharMap* pCharMap = NULL, const CFX_ByteString& endcoding = "");
CFX_ByteString PDF_EncodeText(FX_LPCWSTR pString, int len = -1, CFX_CharMap* pCharMap = NULL);

void FlateEncode(const FX_BYTE* src_buf, FX_DWORD src_size, FX_LPBYTE& dest_buf, FX_DWORD& dest_size);
FX_DWORD FlateDecode(const FX_BYTE* src_buf, FX_DWORD src_size, FX_LPBYTE& dest_buf, FX_DWORD& dest_size);
FX_DWORD RunLengthDecode(const FX_BYTE* src_buf, FX_DWORD src_size, FX_LPBYTE& dest_buf, FX_DWORD& dest_size);

#endif
