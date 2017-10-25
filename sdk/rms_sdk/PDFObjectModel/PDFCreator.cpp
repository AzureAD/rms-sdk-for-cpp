#include "PDFCreator.h"
#include "PDFModuleMgr.h"
#include "Basic.h"
#include <cstdio>

namespace rmscore {
namespace pdfobjectmodel {

//////////////////////////////////////////////////////////////////////////
//class CustomCryptoHandler

CustomCryptoHandler::CustomCryptoHandler(std::shared_ptr<PDFCryptoHandler> pPDFCryptoHandler)
{
    m_pPDFCryptoHandler = pPDFCryptoHandler;
}

CustomCryptoHandler::~CustomCryptoHandler()
{
    m_pPDFCryptoHandler = nullptr;
}

FX_BOOL CustomCryptoHandler::Init(CPDF_Dictionary* pEncryptDict, CPDF_SecurityHandler* pSecurityHandler)
{
	return true;
}

FX_DWORD CustomCryptoHandler::DecryptGetSize(FX_DWORD src_size)
{
    if(m_pPDFCryptoHandler)
    {
        uint32_t size = 0;
        size = m_pPDFCryptoHandler->DecryptGetSize(src_size);
        return size;
    }
	return 0;
}

FX_LPVOID CustomCryptoHandler::DecryptStart(FX_DWORD objnum, FX_DWORD gennum)
{
    if(m_pPDFCryptoHandler)
    {
        m_pPDFCryptoHandler->DecryptStart(objnum, gennum);
    }
	return nullptr;
}

FX_BOOL CustomCryptoHandler::DecryptStream(FX_LPVOID context,
                                           FX_LPCBYTE src_buf,
                                           FX_DWORD src_size,
                                           CFX_BinaryBuf& dest_buf)
{
    if(m_pPDFCryptoHandler)
    {
        std::shared_ptr<CFX_BinaryBuf> sharedDestBuf(&dest_buf, [=](CFX_BinaryBuf* pDestBuf)
        {
            pDestBuf = nullptr;
        });
        PDFBinaryBufImpl binBuf(sharedDestBuf);
        return m_pPDFCryptoHandler->DecryptStream((char*)src_buf, src_size, &binBuf);
    }
    return false;
}

FX_BOOL CustomCryptoHandler::DecryptFinish(FX_LPVOID context,
                                           CFX_BinaryBuf& dest_buf)
{
    if(m_pPDFCryptoHandler)
    {
        std::shared_ptr<CFX_BinaryBuf> sharedDestBuf(&dest_buf, [=](CFX_BinaryBuf* pDestBuf)
        {
            pDestBuf = nullptr;
        });
        PDFBinaryBufImpl binBuf(sharedDestBuf);
        return m_pPDFCryptoHandler->DecryptFinish(&binBuf);
    }
    return false;
}

FX_DWORD CustomCryptoHandler::EncryptGetSize(FX_DWORD objnum, FX_DWORD version, FX_LPCBYTE src_buf, FX_DWORD src_size)
{
    if(m_pPDFCryptoHandler)
    {
        return m_pPDFCryptoHandler->EncryptGetSize(objnum, version, (char*)src_buf, src_size);
    }
	return 0;
}

FX_BOOL CustomCryptoHandler::EncryptContent(FX_DWORD objnum, FX_DWORD version, FX_LPCBYTE src_buf, FX_DWORD src_size,
	FX_LPBYTE dest_buf, FX_DWORD& dest_size)
{
    if(m_pPDFCryptoHandler)
    {
        return m_pPDFCryptoHandler->EncryptContent(objnum, version, (char*)src_buf, src_size, (char*)dest_buf, (uint32_t*)(&dest_size));
    }
    return false;
}

FX_BOOL CustomCryptoHandler::ProgressiveEncryptStart(FX_DWORD objnum, FX_DWORD version, FX_DWORD raw_size, FX_BOOL bFlateEncode)
{
    if(m_pPDFCryptoHandler)
    {
        return m_pPDFCryptoHandler->ProgressiveEncryptStart(objnum, version, raw_size);
    }
    return false;
}

FX_BOOL CustomCryptoHandler::ProgressiveEncryptContent(FX_INT32 objnum, FX_DWORD version, FX_LPCBYTE src_buf, FX_DWORD src_size, CFX_BinaryBuf& dest_buf)
{
    if(m_pPDFCryptoHandler)
    {
        std::shared_ptr<CFX_BinaryBuf> sharedDestBuf(&dest_buf, [=](CFX_BinaryBuf* pDestBuf)
        {
            pDestBuf = nullptr;
        });
        PDFBinaryBufImpl binBuf(sharedDestBuf);
        return m_pPDFCryptoHandler->ProgressiveEncryptContent(objnum, version, (char*)src_buf, src_size, &binBuf);
    }

    return false;
}

FX_BOOL CustomCryptoHandler:: ProgressiveEncryptFinish(CFX_BinaryBuf& dest_buf)
{
    if(m_pPDFCryptoHandler)
    {
        std::shared_ptr<CFX_BinaryBuf> sharedDestBuf(&dest_buf, [=](CFX_BinaryBuf* pDestBuf)
        {
            pDestBuf = nullptr;
        });
        PDFBinaryBufImpl binBuf(sharedDestBuf);
        return m_pPDFCryptoHandler->ProgressiveEncryptFinish(&binBuf);
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////
//class CustomProgressiveEncryptHandler

CustomProgressiveEncryptHandler::CustomProgressiveEncryptHandler(CFX_WideString wsTempPath)
{
    m_wsTempPath = wsTempPath;
    m_pCryptoHandler = nullptr;
    m_dwObjNum = 0;
    m_dwVersion = 0;
}

CustomProgressiveEncryptHandler::~CustomProgressiveEncryptHandler()
{
    m_pCryptoHandler = nullptr;
}


IFX_FileStream* CustomProgressiveEncryptHandler::GetTempFile()
{
    CFX_WideString wsPath = m_wsTempPath;
    wsPath += PROGRESSIVE_ENCRYPT_TEMP_FILE;
    IFX_FileStream*	pFileStream = FX_CreateFileStream(wsPath, FX_FILEMODE_Truncate);
    return pFileStream;
}


void CustomProgressiveEncryptHandler::ReleaseTempFile(IFX_FileStream* pFile)
{
    if (pFile)
    {
        pFile->Release();

        CFX_WideString wsPath = m_wsTempPath;
        wsPath += PROGRESSIVE_ENCRYPT_TEMP_FILE;

        CFX_ByteString bsPath = wsPath.UTF8Encode();
        std::remove((const char *)(FX_LPCSTR)bsPath);
    }
}


FX_BOOL CustomProgressiveEncryptHandler::SetCryptoHandler(CPDF_CryptoHandler* pCryptoHandler)
{
    m_pCryptoHandler = (CustomCryptoHandler*)pCryptoHandler;
    return true;
}


FX_DWORD CustomProgressiveEncryptHandler::EncryptGetSize(FX_DWORD objnum, FX_DWORD gennum, FX_LPCBYTE src_buf, FX_DWORD src_size)
{
    FX_DWORD encryptedSize = 0;
    if(m_pCryptoHandler)
    {
        encryptedSize = m_pCryptoHandler->EncryptGetSize(objnum, gennum, src_buf, src_size);
    }
    return encryptedSize;
}


FX_LPVOID CustomProgressiveEncryptHandler::EncryptStart(FX_DWORD objnum, FX_DWORD gennum, FX_DWORD raw_size, FX_BOOL bFlateEncode)
{
    bool bResult = false;
    m_dwObjNum = objnum;
    m_dwVersion = gennum;
    if (m_pCryptoHandler)
    {
        bResult = m_pCryptoHandler->ProgressiveEncryptStart(m_dwObjNum, m_dwVersion, raw_size, bFlateEncode);
    }

    if (bResult)
    {
        return m_pCryptoHandler;
    }

    return NULL;
}


FX_BOOL CustomProgressiveEncryptHandler::EncryptStream(FX_LPVOID context, FX_LPCBYTE src_buf, FX_DWORD src_size, IFX_FileStream* pDest)
{
    if (m_pCryptoHandler)
    {
        CFX_BinaryBuf dest_buf;
        FX_BOOL bResult = m_pCryptoHandler->ProgressiveEncryptContent(m_dwObjNum, m_dwVersion, src_buf, src_size, dest_buf);
        if (bResult)
        {
            bResult = pDest->WriteBlock(dest_buf.GetBuffer(), dest_buf.GetSize());
        }
        return bResult;
    }
    return false;
}


FX_BOOL CustomProgressiveEncryptHandler::EncryptFinish(FX_LPVOID context, IFX_FileStream* pDest)
{
    FX_BOOL bResult = false;
    if (m_pCryptoHandler)
    {
        CFX_BinaryBuf dest_buf;
        bResult = m_pCryptoHandler->ProgressiveEncryptFinish(dest_buf);
        if (bResult)
        {
            bResult = pDest->WriteBlock(dest_buf.GetBuffer(), dest_buf.GetSize());
        }
    }
    m_dwObjNum = 0;
    m_dwVersion = 0;
    return bResult;
}


FX_BOOL CustomProgressiveEncryptHandler::UpdateFilter(CPDF_Dictionary* pDict)
{
    return true;
}

//////////////////////////////////////////////////////////////////////////
//class CustomSecurityHandler

CustomSecurityHandler::CustomSecurityHandler(std::shared_ptr<PDFSecurityHandler> pPDFSecHandler)
{
    m_pPDFSecHandler = pPDFSecHandler;
    m_bEncryptMetadata = true;
}

CustomSecurityHandler::~CustomSecurityHandler()
{
    m_pPDFSecHandler = nullptr;
}

FX_BOOL CustomSecurityHandler::OnInit(CPDF_Parser* pParser, CPDF_Dictionary* pEncryptDict)
{
    if (pEncryptDict->KeyExist("EncryptMetadata"))
    {
        m_bEncryptMetadata = pEncryptDict->GetBoolean("EncryptMetadata");
    }

    CFX_ByteString bsPL = pEncryptDict->GetString("PublishingLicense");

    FX_INT32 irmVersion = pEncryptDict->GetInteger("MicrosoftIRMVersion");
    if(irmVersion <= 1)
    {
        CFX_ByteString bsPLDecoded;
        CFX_Base64Decoder base64Decoder;
        uint32_t lengthDecoded = base64Decoder.Decode(bsPL, bsPLDecoded);

        FX_LPBYTE dest_buf = nullptr;
        FX_DWORD dest_size = 0;
        FlateDecode((FX_LPCBYTE)bsPLDecoded, lengthDecoded, dest_buf, dest_size);

        bsPL.Empty();
        if (dest_buf[0] == 0xef && dest_buf[1] == 0xbb && dest_buf[2] == 0xbf)
        {
            bsPL.Load(dest_buf, dest_size);
            CFX_WideString wsPL = bsPL.UTF8Decode();

            CFX_ByteString bsPLTemp;
            uint8_t sizeWChar = sizeof(wchar_t);
            if(sizeWChar == 4)
            {
                FX_LPBYTE pUCS2 = nullptr;
                FX_DWORD dwUCS2Length = 0;;
                Utility::UCS4ToUCS2(wsPL, &pUCS2, &dwUCS2Length);
                bsPLTemp.Load(pUCS2, dwUCS2Length);
                if(pUCS2 != nullptr)
                {
                    delete [] pUCS2;
                    pUCS2 = nullptr;
                }
            }
            else
            {
                bsPLTemp.Load(((FX_LPCBYTE)(FX_LPCWSTR)wsPL), wsPL.GetLength() * sizeof(wchar_t));
            }

            if(bsPLTemp.GetAt(0) == 0xff && bsPLTemp.GetAt(1) == 0xfe)
            {
                bsPLTemp.Delete(0, 2);
            }

            bsPL = bsPLTemp;
        }
        else
        {
            FX_DWORD sizeUTF8 = dest_size + 3;
            std::shared_ptr<FX_BYTE> sharedUTF8Bytes(new FX_BYTE[sizeUTF8]);
            FX_LPBYTE pUTF8Bytes = sharedUTF8Bytes.get();
            memset(pUTF8Bytes, 0, sizeof(FX_BYTE) * (sizeUTF8));
            pUTF8Bytes[0] = 0xef;
            pUTF8Bytes[1] = 0xbb;
            pUTF8Bytes[2] = 0xbf;

            Utility::UTF16ToUTF8((UTF16*)dest_buf, (UTF16*)(dest_buf + dest_size),
            pUTF8Bytes + 3, pUTF8Bytes + dest_size + 3);

            bsPL.Load(pUTF8Bytes, sizeUTF8);
        }

        FX_Free(dest_buf);
   }

    if(m_pPDFSecHandler)
    {
        uint32_t plSize = bsPL.GetLength();
        unsigned char* publishingLicense = (FX_LPBYTE)(FX_LPCBYTE)bsPL;
        bool bInit = m_pPDFSecHandler->OnInit(publishingLicense, plSize);
        return bInit;
    }

    return true;
}

FX_DWORD CustomSecurityHandler::GetPermissions()
{
    return 0;
}

FX_BOOL CustomSecurityHandler::IsOwner()
{
    return true;
}

FX_BOOL CustomSecurityHandler::GetCryptInfo(int& cipher, FX_LPCBYTE& buffer, int& keylen)
{
    return true;
}

FX_BOOL CustomSecurityHandler::IsMetadataEncrypted()
{
    return m_bEncryptMetadata;
}

CPDF_CryptoHandler* CustomSecurityHandler::CreateCryptoHandler()
{
    CPDF_CryptoHandler* pCryptHd = nullptr;
    std::shared_ptr<PDFCryptoHandler> pPDFCryptoHandler = nullptr;
    if(m_pPDFSecHandler)
    {
        pPDFCryptoHandler = m_pPDFSecHandler->CreateCryptoHandler();
    }
    if(!pPDFCryptoHandler)
    {
        pCryptHd= new CPDF_StandardCryptoHandler;
    }
    else
    {
        pCryptHd = new CustomCryptoHandler(pPDFCryptoHandler);
    }

    //foxit pdf core takes over pCryptHd
    return pCryptHd;
}

//////////////////////////////////////////////////////////////////////////
//class PDFCreatorImpl

std::unique_ptr<PDFCreator> PDFCreator::Create()
{
    std::unique_ptr<PDFCreator> pdfCreator(new PDFCreatorImpl());
    return pdfCreator;
}

PDFCreatorImpl::PDFCreatorImpl()
{

}

PDFCreatorImpl::~PDFCreatorImpl()
{

}

uint32_t PDFCreatorImpl::CreateCustomEncryptedFile(const std::string& inputFilePath,
        const std::string& filterName,
        const std::vector<unsigned char> &publishingLicense,
        std::shared_ptr<PDFCryptoHandler> cryptoHander,
        rmscrypto::api::SharedStream outputIOS)
{
    uint32_t result = PDFCREATOR_ERR_SUCCESS;
    m_filePath = inputFilePath;

    CPDF_Parser pdfParser;
    result = ParsePDFFile(&pdfParser);
    if(PDFCREATOR_ERR_SUCCESS != result)
    {
        return result;
    }

    CPDF_Dictionary* pEncryptionDict = CreateEncryptionDict(filterName, publishingLicense);
    result = CreatePDFFile(&pdfParser, pEncryptionDict, cryptoHander, outputIOS);

    if(pEncryptionDict)
    {
        pEncryptionDict->Release();
        pEncryptionDict = NULL;
    }

    return result;
}

bool PDFCreatorImpl::IsProtectedByPassword(CPDF_Parser *pPDFParser)
{
    CPDF_Dictionary* pEncryptDict = pPDFParser->GetTrailer()->GetDict("Encrypt");
    if (pEncryptDict)
    {
        CFX_ByteString sFilter = pEncryptDict->GetString("Filter");
        if (sFilter.Compare("Standard") == 0)
        {
            return true;
        }
    }
    return false;
}

bool PDFCreatorImpl::IsSigned(CPDF_Parser *pPDFParser)
{
    CPDF_Document* pDoc = pPDFParser->GetDocument();
    CPDF_Dictionary* pRootDict = pDoc->GetRoot();
    if(pRootDict)
    {
        if(CPDF_Dictionary* pAcroFormDict = pRootDict->GetDict("AcroForm"))
        {
            //has been signed?
            if(pAcroFormDict->KeyExist("SigFlags"))
            {
                int nSigFlg = pAcroFormDict->GetInteger("SigFlags");
                if(nSigFlg == 3)
                {
                   return true;
                }
            }
        }
    }
    return false;
}

bool PDFCreatorImpl::IsDynamicXFA(CPDF_Parser *pPDFParser)
{
    CPDF_Document* pDoc = pPDFParser->GetDocument();
    CPDF_Dictionary* pRootDict = pDoc->GetRoot();
    CPDF_Dictionary* pAcroFormDict = nullptr;
    if(!pRootDict) return false;

    pAcroFormDict = pRootDict->GetDict("AcroForm");
    if(!pAcroFormDict) return false;

    if (!pAcroFormDict->KeyExist("XFA"))
    {
        return false;
    }

    CPDF_Object* pXFAObj = pAcroFormDict->GetElement("XFA");
    CPDF_Array* pXFAArry = pXFAObj->GetArray();
    if(!pXFAArry) return false;

    FX_DWORD nCount = pXFAArry->GetCount();
    for (int i = 0; i < nCount; i++)
    {
        CPDF_Object* pObj = pXFAArry->GetElement(i);
        if (NULL != pObj && pObj->GetType() == PDFOBJ_REFERENCE)
        {
            CPDF_Object* pRefObj = (CPDF_Object*)pObj->GetDict();
            if (NULL != pRefObj)
            {
                CPDF_Dictionary* pDic = pRefObj->GetDict();
                if (NULL != pDic && pDic->GetInteger("Length") != 0)
                {
                    if(pRootDict->KeyExist("NeedsRendering"))
                    {
                        bool bDymasticXFA = pRootDict->GetBoolean("NeedsRendering");
                        if(bDymasticXFA)
                            return true;
                        else
                            //is static XFA
                            return false;
                    }
                    else
                    {
                        //is static XFA
                        return false;
                    }
                }
            }
        }
    }
    return false;
}

uint32_t PDFCreatorImpl::ParsePDFFile(CPDF_Parser *pPDFParser)
{
    uint32_t result = PDFCREATOR_ERR_SUCCESS;
    CFX_ByteString bsFilePath = m_filePath.c_str();

    FX_DWORD parseResult = pPDFParser->StartParse(bsFilePath);
    result = ConvertParsingErrCode(parseResult);

    if(result == PDFCREATOR_ERR_SUCCESS)
    {        
        //has been protected by password
        if (IsProtectedByPassword(pPDFParser))
        {
            return PDFCREATOR_ERR_SECURITY;
        }

        //has been signed?
        if(IsSigned(pPDFParser))
        {
            return PDFCREATOR_ERR_FORMAT;
        }

        //is dynamic XFA PDF
        if(IsDynamicXFA(pPDFParser))
        {
            return PDFCREATOR_ERR_FORMAT;
        }
    }

    return result;
}

uint32_t PDFCreatorImpl::ConvertParsingErrCode(FX_DWORD parseResult)
{
    uint32_t result = PDFCREATOR_ERR_SUCCESS;
    switch(parseResult)
    {
        case PDFPARSE_ERROR_SUCCESS:
        {
            result = PDFCREATOR_ERR_SUCCESS;
        }
        break;
        case PDFPARSE_ERROR_FILE:
        {
            result = PDFCREATOR_ERR_FILE;
        }
        break;
        case PDFPARSE_ERROR_FORMAT:
        {
            result = PDFCREATOR_ERR_FORMAT;
        }
        break;
        case PDFPARSE_ERROR_PASSWORD:
        case PDFPARSE_ERROR_HANDLER:
        case PDFPARSE_ERROR_CERT:
        {
            result = PDFCREATOR_ERR_SECURITY;
        }
        break;
        default:
        {
            result = PDFCREATOR_ERR_UNKNOWN;
        }
        break;
    }

    return result;
}

CPDF_Dictionary* PDFCreatorImpl::CreateEncryptionDict(const std::string& filterName, const std::vector<unsigned char> &publishingLicense)
{
    CPDF_Dictionary* pEncryptionDic = new CPDF_Dictionary;

    pEncryptionDic->SetAtName("Filter", filterName.c_str());

    CPDF_Dictionary* pCFDic = new CPDF_Dictionary;

    CPDF_Dictionary* pCryptFilterDic = new CPDF_Dictionary;
    pCryptFilterDic->SetAtName("Type", "CryptFilter");
    pCryptFilterDic->SetAtName("CFM", "None");

    pCFDic->SetAt(filterName.c_str(), pCryptFilterDic);

    pEncryptionDic->SetAt("CF", pCFDic);

    pEncryptionDic->SetAtName("StmF", filterName.c_str());
    pEncryptionDic->SetAtName("StrF", filterName.c_str());
    pEncryptionDic->SetAtName("EFF", filterName.c_str());
    pEncryptionDic->SetAtInteger("MicrosoftIRMVersion", 2);
    pEncryptionDic->SetAtInteger("V", 4);
    pEncryptionDic->SetAtBoolean("EncryptMetadata", true);

    const char* sPL = reinterpret_cast<const char*>(&(*publishingLicense.begin()));
    uint32_t sizePL = publishingLicense.size();
    CFX_ByteString bsPL(sPL, sizePL);
    pEncryptionDic->SetAtString("PublishingLicense", bsPL);
    return pEncryptionDic;
}

uint32_t PDFCreatorImpl::CreatePDFFile(CPDF_Parser *pPDFParser, CPDF_Dictionary* pEncryptionDic, std::shared_ptr<PDFCryptoHandler> cryptoHander, rmscrypto::api::SharedStream outputIOS)
{
    uint32_t result = PDFCREATOR_ERR_SUCCESS;

    CPDF_Document* pPDFDoc = pPDFParser->GetDocument();
    CPDF_Creator pdfCreator(pPDFDoc);

    CustomCryptoHandler cryptoHandler(cryptoHander);

    if(pEncryptionDic && cryptoHander)
    {
        FX_DWORD objNum = pPDFDoc->AddIndirectObject(pEncryptionDic);
        CPDF_Dictionary* trailerDict = pPDFParser->GetTrailer();
        trailerDict->SetAtReference("Encrypt", pPDFDoc, objNum);

        pdfCreator.SetCustomSecurity(pEncryptionDic, &cryptoHandler, true);

        CFX_ByteString bsFilePath = m_filePath.c_str();
        CFX_WideString wsFilePath = bsFilePath.UTF8Decode();
        CustomProgressiveEncryptHandler* progressHandler = new CustomProgressiveEncryptHandler(wsFilePath);
        //pdfCreator takes over progressHandler
        pdfCreator.SetProgressiveEncryptHandler(progressHandler);
    }

    FileStreamImpl fileStream(outputIOS);
    FX_BOOL bCreate = pdfCreator.Create(&fileStream);
    if(!bCreate)
    {
        result = PDFCREATOR_ERR_CREATOR;
    }

    return result;
}

uint32_t PDFCreatorImpl::UnprotectCustomEncryptedFile(
            rmscrypto::api::SharedStream inputIOS,
            const std::string& filterName,
            std::shared_ptr<PDFSecurityHandler> securityHander,
            rmscrypto::api::SharedStream outputIOS)
{
    uint32_t result = PDFCREATOR_ERR_SUCCESS;

    PDFModuleMgrImpl::RegisterSecurityHandler(filterName.c_str(), securityHander);

    FileStreamImpl inputFileStream(inputIOS);
    CPDF_Parser pdfParser;
    FX_DWORD parseResult = pdfParser.StartParse(&inputFileStream);
    result = ConvertParsingErrCode(parseResult);
    if(result != PDFCREATOR_ERR_SUCCESS) return result;

    CPDF_Document* pPDFDoc = pdfParser.GetDocument();
    CPDF_Creator pdfCreator(pPDFDoc);

    CPDF_Dictionary* encryptDic = pdfParser.GetEncryptDict();
    CPDF_Dictionary* trailerDic = pdfParser.GetTrailer();
    if(trailerDic)
    {
        trailerDic->RemoveAt("Encrypt");
    }
    if(encryptDic)
    {
        pPDFDoc->DeleteIndirectObject(encryptDic->GetObjNum());
    }

    pdfCreator.RemoveSecurity();
    pdfCreator.SetCustomSecurity(nullptr, nullptr, false);

    FileStreamImpl outputFileStream(outputIOS);
    FX_BOOL bCreate = pdfCreator.Create(&outputFileStream);
    if(!bCreate)
    {
        result = PDFCREATOR_ERR_CREATOR;
    }

    return result;
}

} // namespace pdfobjectmodel
} // namespace rmscore
