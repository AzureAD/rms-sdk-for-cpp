/*
 * ======================================================================
 * Copyright (c) Foxit Software, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 *======================================================================
 */

#include "../../../include/fpdfapi/fpdf_parser.h"
#include "../../../include/fpdfapi/fpdf_serial.h"
extern const FX_LPCSTR _PDF_CharType =
    "WRRRRRRRRWWRWWRRRRRRRRRRRRRRRRRR"
    "WRRRRDRRDDRNRNNDNNNNNNNNNNRRDRDR"
    "RRRRRRRRRRRRRRRRRRRRRRRRRRRDRDRR"
    "RRRRRRRRRRRRRRRRRRRRRRRRRRRDRDRR"
    "WRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR"
    "RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR"
    "RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR"
    "RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRW";
#ifndef MAX_PATH
#define MAX_PATH 4096
#endif
static int _hex2dec(char ch)
{
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }
    if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }
    if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    }
    return 0;
}
CFX_ByteString PDF_NameDecode(FX_BSTR bstr)
{
    int size = bstr.GetLength();
    FX_LPCSTR pSrc = bstr.GetCStr();
    if (FXSYS_memchr(pSrc, '#', size) == NULL) {
        return bstr;
    }
    CFX_ByteString result;
    FX_LPSTR pDestStart = result.GetBuffer(size);
    FX_LPSTR pDest = pDestStart;
    for (int i = 0; i < size; i ++) {
        if (pSrc[i] == '#' && i < size - 2) {
            *pDest ++ = _hex2dec(pSrc[i + 1]) * 16 + _hex2dec(pSrc[i + 2]);
            i += 2;
        } else {
            *pDest ++ = pSrc[i];
        }
    }
    result.ReleaseBuffer((FX_STRSIZE)(pDest - pDestStart));
    return result;
}
CFX_ByteString PDF_NameDecode(const CFX_ByteString& orig)
{
    if (FXSYS_memchr((FX_LPCSTR)orig, '#', orig.GetLength()) == NULL) {
        return orig;
    }
    return PDF_NameDecode(CFX_ByteStringC(orig));
}
CFX_ByteString PDF_NameEncode(const CFX_ByteString& orig)
{
    FX_LPBYTE src_buf = (FX_LPBYTE)(FX_LPCSTR)orig;
    int src_len = orig.GetLength();
    int dest_len = 0;
    int i;
    for (i = 0; i < src_len; i ++) {
        FX_BYTE ch = src_buf[i];
        if (ch >= 0x80 || _PDF_CharType[ch] == 'W' || ch == '#' ||
                _PDF_CharType[ch] == 'D') {
            dest_len += 3;
        } else {
            dest_len ++;
        }
    }
    if (dest_len == src_len) {
        return orig;
    }
    CFX_ByteString res;
    FX_LPSTR dest_buf = res.GetBuffer(dest_len);
    dest_len = 0;
    for (i = 0; i < src_len; i ++) {
        FX_BYTE ch = src_buf[i];
        if (ch >= 0x80 || _PDF_CharType[ch] == 'W' || ch == '#' ||
                _PDF_CharType[ch] == 'D') {
            if (_PDF_CharType[ch] == 'W' && ch == 0) {
                continue;
            }
            dest_buf[dest_len++] = '#';
            dest_buf[dest_len++] = "0123456789ABCDEF"[ch / 16];
            dest_buf[dest_len++] = "0123456789ABCDEF"[ch % 16];
        } else {
            dest_buf[dest_len++] = ch;
        }
    }
    dest_buf[dest_len] = 0;
    res.ReleaseBuffer();
    return res;
}
CFX_ByteTextBuf& operator << (CFX_ByteTextBuf& buf, const CPDF_Object* pObj)
{
    return SerializePDFObjectWithObjMapper(buf, pObj, NULL);
}
CFX_ByteTextBuf& SerializePDFObjectWithObjMapper(CFX_ByteTextBuf& buf, const CPDF_Object* pObj, CFX_DWordArray* pObjNumMap)
{
    if (pObj == NULL) {
        buf << FX_BSTRC(" null");
        return buf;
    }
    switch (pObj->GetType()) {
        case PDFOBJ_NULL:
            buf << FX_BSTRC(" null");
            break;
        case PDFOBJ_BOOLEAN:
        case PDFOBJ_NUMBER:
            buf << " " << pObj->GetString();
            break;
        case PDFOBJ_STRING: {
                CFX_ByteString str = pObj->GetString();
                FX_BOOL bHex = ((CPDF_String*)pObj)->IsHex();
                buf << PDF_EncodeString(str, bHex);
                break;
            }
        case PDFOBJ_NAME: {
                CFX_ByteString str = pObj->GetString();
                buf << FX_BSTRC("/") << PDF_NameEncode(str);
                break;
            }
        case PDFOBJ_REFERENCE: {
                CPDF_Reference* p = (CPDF_Reference*)pObj;
                FX_DWORD dwRefObjNum = p->GetRefObjNum();
                FX_DWORD dwRefObjGenNum = p->GetRefObjVersion();
                if(pObjNumMap && dwRefObjNum < pObjNumMap->GetSize()) {
                    FX_DWORD objnum = pObjNumMap->GetAt(dwRefObjNum);
                    if(objnum) {
                        dwRefObjNum = objnum;
                    }
                }
                buf << " " << dwRefObjNum << FX_BSTRC(" ") << dwRefObjGenNum << FX_BSTRC(" R ");
                break;
            }
        case PDFOBJ_ARRAY: {
                CPDF_Array* p = (CPDF_Array*)pObj;
                buf << FX_BSTRC("[");
                for (FX_DWORD i = 0; i < p->GetCount(); i ++) {
                    CPDF_Object* pElement = p->GetElement(i);
                    if (pElement->GetObjNum()) {
                        FX_DWORD dwObjNum = pElement->GetObjNum();
                        if(pObjNumMap && dwObjNum < pObjNumMap->GetSize()) {
                            FX_DWORD objnum = pObjNumMap->GetAt(dwObjNum);
                            if(objnum) {
                                dwObjNum = objnum;
                            }
                        }
                        buf << " " << dwObjNum << FX_BSTRC(" 0 R");
                    } else {
                        SerializePDFObjectWithObjMapper(buf, pElement, pObjNumMap);
                    }
                }
                buf << FX_BSTRC("]");
                break;
            }
        case PDFOBJ_DICTIONARY: {
                CPDF_Dictionary* p = (CPDF_Dictionary*)pObj;
                buf << FX_BSTRC("<<");
                FX_POSITION pos = p->GetStartPos();
                while (pos) {
                    CFX_ByteString key;
                    CPDF_Object* pValue = p->GetNextElement(pos, key);
                    if (pValue->GetType() == PDFOBJ_REFERENCE) {
                        CPDF_Reference* pRef = (CPDF_Reference*)pValue;
                        FX_DWORD dwRefObjNum = pRef->GetRefObjNum();
                        if(pObjNumMap && dwRefObjNum < pObjNumMap->GetSize()) {
                            FX_DWORD objnum = pObjNumMap->GetAt(dwRefObjNum);
                            if(!objnum) {
                                continue;
                            }
                        }
                    }
                    buf << FX_BSTRC("/") << PDF_NameEncode(key);
                    if (pValue->GetObjNum()) {
                        FX_DWORD dwObjNum = pValue->GetObjNum();
                        if(pObjNumMap && dwObjNum < pObjNumMap->GetSize()) {
                            FX_DWORD objnum = pObjNumMap->GetAt(dwObjNum);
                            if(objnum) {
                                dwObjNum = objnum;
                            }
                        }
                        buf << " " << dwObjNum << FX_BSTRC(" 0 R ");
                    } else {
                        SerializePDFObjectWithObjMapper(buf, pValue, pObjNumMap);
                    }
                }
                buf << FX_BSTRC(">>");
                break;
            }
        case PDFOBJ_STREAM: {
                CPDF_Stream* p = (CPDF_Stream*)pObj;
                SerializePDFObjectWithObjMapper(buf, p->GetDict(), pObjNumMap) << FX_BSTRC("stream\r\n");
                CPDF_StreamAcc acc;
                acc.LoadAllData(p, TRUE);
                buf.AppendBlock(acc.GetData(), acc.GetSize());
                buf << FX_BSTRC("\r\nendstream");
                break;
            }
        default:
            ASSERT(FALSE);
            break;
    }
    return buf;
}
