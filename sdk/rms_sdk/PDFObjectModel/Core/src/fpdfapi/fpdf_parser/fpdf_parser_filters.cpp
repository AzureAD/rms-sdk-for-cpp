/*
 * ======================================================================
 * Copyright (c) Foxit Software, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 *======================================================================
 */

#include "../../fx_zlib.h"
#include "../../../include/fpdfapi/fpdf_parser.h"
#include "../../../include/fxcodec/fx_codec.h"
#include "../../../include/fpdfapi/fpdf_module.h"
#include "filters_int.h"
CFX_DataFilter::CFX_DataFilter()
{
    m_bEOF = FALSE;
    m_pDestFilter = NULL;
    m_SrcPos = 0;
    m_bExhaustBuffer = TRUE;
    m_FilterInBuffer.EstimateSize(FPDF_FILTER_BUFFER_SIZE, FPDF_FILTER_BUFFER_SIZE);
    m_bAbort = FALSE;
}
CFX_DataFilter::~CFX_DataFilter()
{
    if (m_pDestFilter) {
        delete m_pDestFilter;
    }
}
void CFX_DataFilter::SetDestFilter(CFX_DataFilter* pFilter)
{
    if (m_pDestFilter) {
        m_pDestFilter->SetDestFilter(pFilter);
    } else {
        m_pDestFilter = pFilter;
    }
}
void CFX_DataFilter::FilterIn(FX_LPCBYTE src_buf, size_t src_size, CFX_BinaryBuf& dest_buf)
{
    if (m_bEOF && NeedNewSrc()) {
        return;
    }
    m_SrcPos += src_size;
    if (m_pDestFilter) {
        if (m_pDestFilter->IsExhaustBuffer()) {
            m_FilterInBuffer.Clear();
            v_FilterIn(src_buf, src_size, m_FilterInBuffer);
        }
        m_pDestFilter->FilterIn(m_FilterInBuffer.GetBuffer(), m_FilterInBuffer.GetSize(), dest_buf);
    } else {
        v_FilterIn(src_buf, src_size, dest_buf);
    }
}
void CFX_DataFilter::ResetStatistics()
{
    if (m_pDestFilter) {
        v_ResetStatistics();
        m_pDestFilter->ResetStatistics();
    } else {
        v_ResetStatistics();
    }
}
void CFX_DataFilter::FilterFinish(CFX_BinaryBuf& dest_buf)
{
    if (m_pDestFilter) {
        if (m_pDestFilter->IsExhaustBuffer()) {
            m_FilterInBuffer.Clear();
            v_FilterFinish(m_FilterInBuffer);
        }
        if (m_FilterInBuffer.GetSize()) {
            m_pDestFilter->FilterIn(m_FilterInBuffer.GetBuffer(), m_FilterInBuffer.GetSize(), dest_buf);
        }
        m_pDestFilter->FilterFinish(dest_buf);
    } else {
        v_FilterFinish(dest_buf);
    }
    m_bEOF = TRUE;
}
FX_BOOL CFX_DataFilter::NeedNewSrc()
{
    if (!m_bExhaustBuffer) {
        return FALSE;
    }
    FX_BOOL bExhaustBuffer = m_bExhaustBuffer;
    CFX_DataFilter* pDataFilter = m_pDestFilter;
    while (pDataFilter) {
        if (pDataFilter->IsExhaustBuffer()) {
            bExhaustBuffer = pDataFilter->IsExhaustBuffer();
            pDataFilter = pDataFilter->m_pDestFilter;
        } else {
            return FALSE;
        }
    }
    return bExhaustBuffer;
}
FX_BOOL CFX_DataFilter::AbortAll()
{
    CFX_DataFilter* pDataFilter = this;
    while (pDataFilter) {
        if (pDataFilter->Abort()) {
            return TRUE;
        }
        pDataFilter = pDataFilter->m_pDestFilter;
    }
    return FALSE;
}
void CFX_DataFilter::ReportEOF(FX_DWORD left_input)
{
    if (m_bEOF) {
        return;
    }
    m_bEOF = TRUE;
    m_SrcPos -= left_input;
}
CFX_DataFilter* FPDF_CreateFilter(FX_BSTR name, const CPDF_Dictionary* pParam, int width, int height)
{
    FX_DWORD id = name.GetID();
    switch (id) {
        case FXBSTR_ID('F', 'l', 'a', 't'):
        case FXBSTR_ID('F', 'l', 0, 0):
        case FXBSTR_ID('L', 'Z', 'W', 'D'):
        case FXBSTR_ID('L', 'Z', 'W', 0): {
                CFX_DataFilter* pFilter;
                if (id == FXBSTR_ID('L', 'Z', 'W', 'D') || id == FXBSTR_ID('L', 'Z', 'W', 0)) {
                    pFilter = FX_NEW CPDF_LzwFilter(pParam ? pParam->GetInteger("EarlyChange", 1) : 1);
                } else {
                    pFilter = FX_NEW CPDF_FlateFilter;
                }
                if ((pParam ? pParam->GetInteger("Predictor", 1) : 1) > 1) {
                    CFX_DataFilter* pPredictor = FX_NEW CPDF_PredictorFilter(pParam->GetInteger(FX_BSTRC("Predictor"), 1),
                                                 pParam->GetInteger(FX_BSTRC("Colors"), 1), pParam->GetInteger(FX_BSTRC("BitsPerComponent"), 8),
                                                 pParam->GetInteger(FX_BSTRC("Columns"), 1));
                    pFilter->SetDestFilter(pPredictor);
                }
                return pFilter;
            }
        case FXBSTR_ID('A', 'S', 'C', 'I'):
            if (name == "ASCIIHexDecode") {
                return FX_NEW CPDF_AsciiHexFilter;
            }
            return FX_NEW CPDF_Ascii85Filter;
        case FXBSTR_ID('A', 'H', 'x', 0):
            return FX_NEW CPDF_AsciiHexFilter;
        case FXBSTR_ID('A', '8', '5', 0):
            return FX_NEW CPDF_Ascii85Filter;
        case FXBSTR_ID('R', 'u', 'n', 'L'):
            return FX_NEW CPDF_RunLenFilter;
        case FXBSTR_ID('C', 'C', 'I', 'T'):
        case FXBSTR_ID('D', 'C', 'T', 'D'):
        default:
            return NULL;
    }
}
CFX_DataFilter* _FPDF_CreateFilterFromDict(CPDF_Dictionary* pDict)
{
    CPDF_Object* pDecoder = pDict->GetElementValue("Filter");
    if (pDecoder == NULL) {
        return NULL;
    }
    CFX_DataFilter* pFirstFilter = NULL;
    int width = pDict->GetInteger(FX_BSTRC("Width")), height = pDict->GetInteger(FX_BSTRC("Height"));
    CPDF_Object* pParams = pDict->GetElementValue("DecodeParms");
    if (pDecoder->GetType() == PDFOBJ_ARRAY) {
        if (pParams && pParams->GetType() != PDFOBJ_ARRAY) {
            pParams = NULL;
        }
        for (FX_DWORD i = 0; i < ((CPDF_Array*)pDecoder)->GetCount(); i ++) {
            CFX_ByteString name = ((CPDF_Array*)pDecoder)->GetString(i);
            CPDF_Dictionary* pParam = NULL;
            if (pParams) {
                pParam = ((CPDF_Array*)pParams)->GetDict(i);
            }
            CFX_DataFilter* pDestFilter = FPDF_CreateFilter(name, pParam, width, height);
            if (pDestFilter) {
                if (pFirstFilter == NULL) {
                    pFirstFilter = pDestFilter;
                } else {
                    pFirstFilter->SetDestFilter(pDestFilter);
                }
            }
        }
    } else {
        if (pParams && pParams->GetType() != PDFOBJ_DICTIONARY) {
            pParams = NULL;
        }
        pFirstFilter = FPDF_CreateFilter(pDecoder->GetString(), (CPDF_Dictionary*)pParams, width, height);
    }
    return pFirstFilter;
}
#define FPDF_DECRYPTFILTER_BUFFER_SIZE	FPDF_FILTER_BUFFER_SIZE * 52
CPDF_StreamFilter* CPDF_Stream::GetStreamFilter(FX_BOOL bRaw) const
{
    CFX_DataFilter* pFirstFilter = NULL;
    FX_UINT32 srcBufferSize = FPDF_FILTER_BUFFER_SIZE;
    if (crypto_handler_) {
        srcBufferSize = GetRawSize();
        if (srcBufferSize > FPDF_DECRYPTFILTER_BUFFER_SIZE) {
            srcBufferSize = FPDF_DECRYPTFILTER_BUFFER_SIZE;
        }
        pFirstFilter = FX_NEW CPDF_DecryptFilter(crypto_handler_, m_ObjNum, m_GenNum, srcBufferSize);
    }
    if (!bRaw) {
        CFX_DataFilter* pFilter = _FPDF_CreateFilterFromDict(m_pDict);
        if (pFilter) {
            if (pFirstFilter == NULL) {
                pFirstFilter = pFilter;
            } else {
                pFirstFilter->SetDestFilter(pFilter);
            }
        }
    }
    CPDF_StreamFilter* pStreamFilter = FX_NEW CPDF_StreamFilter;
    pStreamFilter->m_pStream = this;
    pStreamFilter->m_pFilter = pFirstFilter;
    pStreamFilter->m_pBuffer = NULL;
    pStreamFilter->m_SrcOffset = 0;
    pStreamFilter->m_SrcBufferSize = srcBufferSize;
    if (!pStreamFilter->m_pSrcBuffer) {
        pStreamFilter->m_pSrcBuffer = FX_Alloc(FX_BYTE, pStreamFilter->m_SrcBufferSize);
    }
    if (pStreamFilter->m_pSrcBuffer) {
        FXSYS_memset32(pStreamFilter->m_pSrcBuffer, 0, pStreamFilter->m_SrcBufferSize);
    }
    return pStreamFilter;
}
CPDF_StreamFilter::~CPDF_StreamFilter()
{
    if (m_pFilter) {
        delete m_pFilter;
    }
    if (m_pBuffer) {
        delete m_pBuffer;
    }
    if (m_pSrcBuffer) {
        FX_Free(m_pSrcBuffer);
    }
}
size_t CPDF_StreamFilter::ReadBlock(FX_LPBYTE buffer, size_t buf_size)
{
    if (m_pFilter == NULL) {
        if (m_pStream->GetRawSize() <= 0) {
            return 0;
        }
        FX_DWORD read_size = m_pStream->GetRawSize() - m_SrcOffset;
        if (read_size == 0) {
            return 0;
        }
        if (read_size > buf_size) {
            read_size = buf_size;
        }
        if (m_pStream->ReadRawData(m_SrcOffset, buffer, read_size)) {
            m_SrcOffset += read_size;
            return read_size;
        }
        m_Status = Error;
        return 0;
    }
    FX_DWORD read_size = 0;
    if (m_pBuffer) {
        read_size = ReadLeftOver(buffer, buf_size);
        if (read_size == buf_size) {
            return read_size;
        }
        buffer += read_size;
        buf_size -= read_size;
    }
    if (!m_pSrcBuffer) {
        return 0;
    }
    ASSERT(m_pBuffer == NULL);
    if (m_pFilter->IsEOF() && m_pFilter->NeedNewSrc()) {
        return read_size;
    }
    m_pBuffer = FX_NEW CFX_BinaryBuf;
    m_pBuffer->EstimateSize(FPDF_FILTER_BUFFER_SIZE, FPDF_FILTER_BUFFER_SIZE);
    m_BufOffset = 0;
    while (1) {
        int src_size = 0;
        if (m_pFilter->NeedNewSrc() || m_pFilter->AbortAll()) {
            src_size = m_pStream->GetRawSize() - m_SrcOffset;
            if (src_size == 0) {
                m_pFilter->FilterFinish(*m_pBuffer);
                break;
            }
            if (src_size > m_SrcBufferSize) {
                src_size = m_SrcBufferSize;
            }
            if (!m_pStream->ReadRawData(m_SrcOffset, m_pSrcBuffer, src_size)) {
                m_Status = Error;
                return 0;
            }
            if (src_size) {
                m_pFilter->ResetStatistics();
            }
            m_SrcOffset += src_size;
        }
        m_pFilter->FilterIn(m_pSrcBuffer, src_size, *m_pBuffer);
        if (m_pBuffer->GetSize() >= (int)buf_size) {
            break;
        }
    }
    return read_size + ReadLeftOver(buffer, buf_size);
}
size_t CPDF_StreamFilter::ReadLeftOver(FX_LPBYTE buffer, size_t buf_size)
{
    size_t read_size = m_pBuffer->GetSize() - m_BufOffset;
    if (read_size > buf_size) {
        read_size = buf_size;
    }
    FXSYS_memcpy32(buffer, m_pBuffer->GetBuffer() + m_BufOffset, read_size);
    m_BufOffset += read_size;
    if (m_BufOffset == (FX_FILESIZE)m_pBuffer->GetSize()) {
        delete m_pBuffer;
        m_pBuffer = NULL;
    }
    return read_size;
}
CPDF_DecryptFilter::CPDF_DecryptFilter(CPDF_CryptoHandler* pCryptoHandler, FX_DWORD objnum, FX_DWORD gennum, FX_UINT32 newInBufferSize)
{
    crypto_handler_ = pCryptoHandler;
    m_pContext = NULL;
    m_ObjNum = objnum;
    m_GenNum = gennum;
    m_FilterInBuffer.EstimateSize(newInBufferSize, newInBufferSize);
}
CPDF_DecryptFilter::~CPDF_DecryptFilter()
{
    CFX_BinaryBuf buf;
    if (m_pContext) {
        crypto_handler_->DecryptFinish(m_pContext, buf);
    }
}
void CPDF_DecryptFilter::v_FilterIn(FX_LPCBYTE src_buf, size_t src_size, CFX_BinaryBuf& dest_buf)
{
    if (m_pContext == NULL) {
        m_pContext = crypto_handler_->DecryptStart(m_ObjNum, m_GenNum);
    }
    crypto_handler_->DecryptStream(m_pContext, src_buf, src_size, dest_buf);
}
void CPDF_DecryptFilter::v_FilterFinish(CFX_BinaryBuf& dest_buf)
{
    m_bEOF = TRUE;
    if (m_pContext == NULL) {
        return;
    }
    crypto_handler_->DecryptFinish(m_pContext, dest_buf);
    m_pContext = NULL;
}
extern "C" {
    static void* my_alloc_func (void* opaque, unsigned int items, unsigned int size)
    {
        return FX_Alloc(FX_BYTE, items * size);
    }
    static void   my_free_func  (void* opaque, void* address)
    {
        FX_Free(address);
    }
    void* FPDFAPI_FlateInit(void* (*alloc_func)(void*, unsigned int, unsigned int),
                            void (*free_func)(void*, void*));
    void FPDFAPI_FlateInput(void* context, const unsigned char* src_buf, unsigned int src_size);
    int FPDFAPI_FlateOutput(void* context, unsigned char* dest_buf, unsigned int dest_size);
    int FPDFAPI_FlateGetTotalIn(void* context);
    int FPDFAPI_FlateGetAvailIn(void* context);
    int FPDFAPI_FlateGetAvailOut(void* context);
    void FPDFAPI_FlateEnd(void* context);
}
CPDF_FlateFilter::CPDF_FlateFilter()
{
    m_pContext = NULL;
    FXSYS_memset32(m_DestBuffer, 0, sizeof(m_DestBuffer));
}
CPDF_FlateFilter::~CPDF_FlateFilter()
{
    if (m_pContext) {
        FPDFAPI_FlateEnd(m_pContext);
    }
}
void CPDF_FlateFilter::v_FilterIn(FX_LPCBYTE src_buf, size_t src_size, CFX_BinaryBuf& dest_buf)
{
    m_SrcPos -= src_size;
    if (m_pContext == NULL) {
        m_pContext = FPDFAPI_FlateInit(my_alloc_func, my_free_func);
    }
    if (m_bExhaustBuffer) {
        FPDFAPI_FlateInput(m_pContext, src_buf, src_size);
    }
    int oldTotalIn = FPDFAPI_FlateGetTotalIn(m_pContext);
    while (1) {
        int ret = FPDFAPI_FlateOutput(m_pContext, m_DestBuffer, FPDF_FILTER_BUFFER_SIZE);
        int out_size = FPDF_FILTER_BUFFER_SIZE - FPDFAPI_FlateGetAvailOut(m_pContext);
        dest_buf.AppendBlock(m_DestBuffer, out_size);
        m_bExhaustBuffer = FPDFAPI_FlateGetAvailIn(m_pContext) == 0;
        if (ret == Z_BUF_ERROR) {
            break;
        }
        if (ret != Z_OK) {
            m_bExhaustBuffer = TRUE;
            m_SrcPos += (FPDFAPI_FlateGetTotalIn(m_pContext) - oldTotalIn);
            ReportEOF(FPDFAPI_FlateGetAvailIn(m_pContext));
            break;
        }
        if (out_size == FPDF_FILTER_BUFFER_SIZE) {
            m_SrcPos += (FPDFAPI_FlateGetTotalIn(m_pContext) - oldTotalIn);
            break;
        }
    }
}
CPDF_LzwFilter::CPDF_LzwFilter(FX_BOOL bEarlyChange)
{
    m_bEarlyChange = bEarlyChange ? 1 : 0;
    FXSYS_memset32(m_CodeArray, 0, sizeof(m_CodeArray));
    m_CodeLen = 9;
    m_nCodes = 0;
    m_nLeftBits = 0;
    m_LeftBits = 0;
    m_OldCode = (FX_DWORD) - 1;
    m_LastChar = 0;
    FXSYS_memset32(m_DecodeStack, 0, sizeof(m_DecodeStack));
    m_StackLen = 0;
}
void CPDF_LzwFilter::v_FilterIn(FX_LPCBYTE src_buf, size_t src_size, CFX_BinaryBuf& dest_buf)
{
    for (FX_DWORD i = 0; i < src_size; i ++) {
        if (m_nLeftBits + 8 < m_CodeLen) {
            m_nLeftBits += 8;
            m_LeftBits = (m_LeftBits << 8) | src_buf[i];
            continue;
        }
        FX_DWORD new_bits = m_CodeLen - m_nLeftBits;
        FX_DWORD code = (m_LeftBits << new_bits) | (src_buf[i] >> (8 - new_bits));
        m_nLeftBits = 8 - new_bits;
        m_LeftBits = src_buf[i] % (1 << m_nLeftBits);
        if (code < 256) {
            dest_buf.AppendByte((FX_BYTE)code);
            m_LastChar = (FX_BYTE)code;
            if (m_OldCode != -1) {
                AddCode(m_OldCode, m_LastChar);
            }
            m_OldCode = code;
        } else if (code == 256) {
            m_CodeLen = 9;
            m_nCodes = 0;
            m_OldCode = (FX_DWORD) - 1;
        } else if (code == 257) {
            ReportEOF(src_size - i - 1);
            return;
        } else {
            if (m_OldCode == -1) {
                ReportEOF(src_size - i - 1);
                return;
            }
            m_StackLen = 0;
            if (code >= m_nCodes + 258) {
                if (m_StackLen < sizeof(m_DecodeStack)) {
                    m_DecodeStack[m_StackLen++] = m_LastChar;
                }
                DecodeString(m_OldCode);
            } else {
                DecodeString(code);
            }
            dest_buf.AppendBlock(NULL, m_StackLen);
            FX_LPBYTE pOutput = dest_buf.GetBuffer() + dest_buf.GetSize() - m_StackLen;
            for (FX_DWORD cc = 0; cc < m_StackLen; cc ++) {
                pOutput[cc] = m_DecodeStack[m_StackLen - cc - 1];
            }
            m_LastChar = m_DecodeStack[m_StackLen - 1];
            if (m_OldCode < 256) {
                AddCode(m_OldCode, m_LastChar);
            } else if (m_OldCode - 258 >= m_nCodes) {
                ReportEOF(src_size - i - 1);
                return;
            } else {
                AddCode(m_OldCode, m_LastChar);
            }
            m_OldCode = code;
        }
    }
}
void CPDF_LzwFilter::AddCode(FX_DWORD prefix_code, FX_BYTE append_char)
{
    if (m_nCodes + m_bEarlyChange == 4094) {
        return;
    }
    m_CodeArray[m_nCodes ++] = (prefix_code << 16) | append_char;
    if (m_nCodes + m_bEarlyChange == 512 - 258) {
        m_CodeLen = 10;
    } else if (m_nCodes + m_bEarlyChange == 1024 - 258) {
        m_CodeLen = 11;
    } else if (m_nCodes + m_bEarlyChange == 2048 - 258) {
        m_CodeLen = 12;
    }
}
void CPDF_LzwFilter::DecodeString(FX_DWORD code)
{
    while (1) {
        int index = code - 258;
        if (index < 0 || index >= (int)m_nCodes) {
            break;
        }
        FX_DWORD data = m_CodeArray[index];
        if (m_StackLen >= sizeof(m_DecodeStack)) {
            return;
        }
        m_DecodeStack[m_StackLen++] = (FX_BYTE)data;
        code = data >> 16;
    }
    if (m_StackLen >= sizeof(m_DecodeStack)) {
        return;
    }
    m_DecodeStack[m_StackLen++] = (FX_BYTE)code;
}
CPDF_PredictorFilter::CPDF_PredictorFilter(int predictor, int colors, int bpc, int cols)
{
    m_bTiff = predictor < 10;
    m_pRefLine = NULL;
    m_pCurLine = NULL;
    m_iLine = 0;
    m_LineInSize = 0;
    m_Bpp = (colors * bpc + 7) / 8;
    m_Pitch = (colors * bpc * cols + 7) / 8;
    if (!m_bTiff) {
        m_Pitch ++;
    }
}
CPDF_PredictorFilter::~CPDF_PredictorFilter()
{
    if (m_pCurLine) {
        FX_Free(m_pCurLine);
    }
    if (m_pRefLine) {
        FX_Free(m_pRefLine);
    }
}
static FX_BYTE PaethPredictor(int a, int b, int c)
{
    int p = a + b - c;
    int pa = FXSYS_abs(p - a);
    int pb = FXSYS_abs(p - b);
    int pc = FXSYS_abs(p - c);
    if (pa <= pb && pa <= pc) {
        return (FX_BYTE)a;
    }
    if (pb <= pc) {
        return (FX_BYTE)b;
    }
    return (FX_BYTE)c;
}
static void PNG_PredictorLine(FX_LPBYTE cur_buf, FX_LPBYTE ref_buf, int pitch, int Bpp)
{
    FX_BYTE tag = cur_buf[0];
    if (tag == 0) {
        return;
    }
    cur_buf ++;
    if (ref_buf) {
        ref_buf ++;
    }
    for (int byte = 0; byte < pitch; byte ++) {
        FX_BYTE raw_byte = cur_buf[byte];
        switch (tag) {
            case 1:	{
                    FX_BYTE left = 0;
                    if (byte >= Bpp) {
                        left = cur_buf[byte - Bpp];
                    }
                    cur_buf[byte] = raw_byte + left;
                    break;
                }
            case 2: {
                    FX_BYTE up = 0;
                    if (ref_buf) {
                        up = ref_buf[byte];
                    }
                    cur_buf[byte] = raw_byte + up;
                    break;
                }
            case 3: {
                    FX_BYTE left = 0;
                    if (byte >= Bpp) {
                        left = cur_buf[byte - Bpp];
                    }
                    FX_BYTE up = 0;
                    if (ref_buf) {
                        up = ref_buf[byte];
                    }
                    cur_buf[byte] = raw_byte + (up + left) / 2;
                    break;
                }
            case 4: {
                    FX_BYTE left = 0;
                    if (byte >= Bpp) {
                        left = cur_buf[byte - Bpp];
                    }
                    FX_BYTE up = 0;
                    if (ref_buf) {
                        up = ref_buf[byte];
                    }
                    FX_BYTE upper_left = 0;
                    if (byte >= Bpp && ref_buf) {
                        upper_left = ref_buf[byte - Bpp];
                    }
                    cur_buf[byte] = raw_byte + PaethPredictor(left, up, upper_left);
                    break;
                }
        }
    }
}
void CPDF_PredictorFilter::v_FilterIn(FX_LPCBYTE src_buf, size_t src_size, CFX_BinaryBuf& dest_buf)
{
    if (m_pCurLine == NULL) {
        m_pCurLine = FX_Alloc(FX_BYTE, m_Pitch);
        if (!m_bTiff) {
            m_pRefLine = FX_Alloc(FX_BYTE, m_Pitch);
        }
    }
    while (1) {
        FX_DWORD read_size = m_Pitch - m_LineInSize;
        if (read_size > src_size) {
            read_size = src_size;
        }
        FXSYS_memcpy32(m_pCurLine + m_LineInSize, src_buf, read_size);
        m_LineInSize += read_size;
        if (m_LineInSize < m_Pitch) {
            break;
        }
        src_buf += read_size;
        src_size -= read_size;
        if (m_bTiff) {
            for (FX_DWORD byte = m_Bpp; byte < m_Pitch; byte ++) {
                m_pCurLine[byte] += m_pCurLine[byte - m_Bpp];
            }
            dest_buf.AppendBlock(m_pCurLine, m_Pitch);
        } else {
            PNG_PredictorLine(m_pCurLine, m_iLine ? m_pRefLine : NULL, m_Pitch - 1, m_Bpp);
            dest_buf.AppendBlock(m_pCurLine + 1, m_Pitch - 1);
            m_iLine ++;
            FX_LPBYTE temp = m_pCurLine;
            m_pCurLine = m_pRefLine;
            m_pRefLine = temp;
        }
        m_LineInSize = 0;
    }
}
CPDF_Ascii85Filter::CPDF_Ascii85Filter()
{
    m_State = 0;
    m_CharCount = 0;
    m_CurDWord = 0;
}
extern const FX_LPCSTR _PDF_CharType;
void CPDF_Ascii85Filter::v_FilterIn(FX_LPCBYTE src_buf, size_t src_size, CFX_BinaryBuf& dest_buf)
{
    for (FX_DWORD i = 0; i < src_size; i ++) {
        FX_BYTE byte = src_buf[i];
        if (_PDF_CharType[byte] == 'W') {
            continue;
        }
        switch (m_State) {
            case 0:
                if (byte >= '!' && byte <= 'u') {
                    int digit = byte - '!';
                    m_CurDWord = digit;
                    m_CharCount = 1;
                    m_State = 1;
                } else if (byte == 'z') {
                    int zero = 0;
                    dest_buf.AppendBlock(&zero, 4);
                } else if (byte == '~') {
                    m_State = 2;
                }
                break;
            case 1: {
                    if (byte >= '!' && byte <= 'u') {
                        int digit = byte - '!';
                        m_CurDWord = m_CurDWord * 85 + digit;
                        m_CharCount ++;
                        if (m_CharCount == 5) {
                            for (int i = 0; i < 4; i ++) {
                                dest_buf.AppendByte((FX_BYTE)(m_CurDWord >> (3 - i) * 8));
                            }
                            m_State = 0;
                        }
                    } else if (byte == '~') {
                        if (m_CharCount > 1) {
                            int i;
                            for (i = m_CharCount; i < 5; i ++) {
                                m_CurDWord = m_CurDWord * 85 + 84;
                            }
                            for (i = 0; i < m_CharCount - 1; i ++) {
                                dest_buf.AppendByte((FX_BYTE)(m_CurDWord >> (3 - i) * 8));
                            }
                        }
                        m_State = 2;
                    }
                    break;
                }
            case 2:
                if (byte == '>') {
                    ReportEOF(src_size - i - 1);
                    return;
                }
                break;
        }
    }
}
CPDF_AsciiHexFilter::CPDF_AsciiHexFilter()
{
    m_State = 0;
    m_FirstDigit = 0;
}
void CPDF_AsciiHexFilter::v_FilterIn(FX_LPCBYTE src_buf, size_t src_size, CFX_BinaryBuf& dest_buf)
{
    for (FX_DWORD i = 0; i < src_size; i ++) {
        FX_BYTE byte = src_buf[i];
        if (_PDF_CharType[byte] == 'W') {
            continue;
        }
        int digit;
        if (byte >= '0' && byte <= '9') {
            digit = byte - '0';
        } else if (byte >= 'a' && byte <= 'f') {
            digit = byte - 'a' + 10;
        } else if (byte >= 'A' && byte <= 'F') {
            digit = byte - 'A' + 10;
        } else {
            if (m_State) {
                dest_buf.AppendByte(m_FirstDigit * 16);
            }
            ReportEOF(src_size - i - 1);
            return;
        }
        if (m_State == 0) {
            m_FirstDigit = digit;
            m_State ++;
        } else {
            dest_buf.AppendByte(m_FirstDigit * 16 + digit);
            m_State --;
        }
    }
}
CPDF_RunLenFilter::CPDF_RunLenFilter()
{
    m_State = 0;
    m_Count = 0;
    m_SrcOffset = 0;
    m_TotalDestSize = 0;
}
void CPDF_RunLenFilter::v_FilterIn(FX_LPCBYTE src_buf, size_t src_size, CFX_BinaryBuf& dest_buf)
{
    m_SrcPos -= src_size;
    if (m_TotalDestSize >= 20 * 1024 * 1024) {
        m_bAbort = TRUE;
        return;
    }
    FX_DWORD oldSrcOffset = m_SrcOffset;
    for (; m_SrcOffset < src_size; m_SrcOffset++) {
        FX_BYTE byte = src_buf[m_SrcOffset];
        switch (m_State) {
            case 0: {
                    if (byte < 128) {
                        m_State = 1;
                        m_Count = byte + 1;
                    } else if (byte == 128) {
                        m_bExhaustBuffer = TRUE;
                        m_SrcPos += (m_SrcOffset - oldSrcOffset + 1);
                        ReportEOF(src_size - m_SrcOffset - 1);
                        return;
                    } else {
                        m_State = 2;
                        m_Count = 257 - byte;
                    }
                }
                break;
            case 1: {
                    dest_buf.AppendByte(byte);
                    m_Count --;
                    if (m_Count == 0) {
                        m_State = 0;
                    }
                    m_TotalDestSize += 1;
                    if (dest_buf.GetSize() >= FPDF_FILTER_BUFFER_SIZE) {
                        m_SrcOffset++;
                        m_bExhaustBuffer = (m_SrcOffset >= src_size);
                        m_SrcPos += (m_SrcOffset - oldSrcOffset);
                        if (src_size && m_SrcOffset >= src_size) {
                            m_SrcOffset = 0;
                        }
                        return;
                    }
                }
                break;
            case 2: {
                    dest_buf.AppendBlock(NULL, m_Count);
                    FXSYS_memset8(dest_buf.GetBuffer() + dest_buf.GetSize() - m_Count, byte, m_Count);
                    m_State = 0;
                    m_TotalDestSize += m_Count;
                    if (dest_buf.GetSize() >= FPDF_FILTER_BUFFER_SIZE) {
                        m_SrcOffset++;
                        m_bExhaustBuffer = (m_SrcOffset >= src_size);
                        m_SrcPos += (m_SrcOffset - oldSrcOffset);
                        if (src_size && m_SrcOffset >= src_size) {
                            m_SrcOffset = 0;
                        }
                        return;
                    }
                }
                break;
        }
    }
    if (src_size && m_SrcOffset >= src_size) {
        m_bExhaustBuffer = TRUE;
        m_SrcPos += (m_SrcOffset - oldSrcOffset);
        m_SrcOffset = 0;
    }
}
