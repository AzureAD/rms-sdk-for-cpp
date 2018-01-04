#include "../../include/fxcrt/fx_ext.h"
#include "../../include/fdrm/fx_crypt.h"
#include "extension.h"
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
#include <wincrypt.h>
#else
#include <ctime>
#endif
FX_HFILE FX_File_Open(FX_BSTR fileName, FX_DWORD dwMode)
{
    IFXCRT_FileAccess* pFA = FXCRT_FileAccess_Create();
    if (pFA && !pFA->Open(fileName, dwMode)) {
        pFA->Release();
        return NULL;
    }
    return (FX_HFILE)pFA;
}
FX_HFILE FX_File_Open(FX_WSTR fileName, FX_DWORD dwMode)
{
    IFXCRT_FileAccess* pFA = FXCRT_FileAccess_Create();
    if (pFA && !pFA->Open(fileName, dwMode)) {
        pFA->Release();
        return NULL;
    }
    return (FX_HFILE)pFA;
}
void FX_File_Close(FX_HFILE hFile)
{
    FXSYS_assert(hFile != NULL);
    ((IFXCRT_FileAccess*)hFile)->Close();
    ((IFXCRT_FileAccess*)hFile)->Release();
}
FX_FILESIZE FX_File_GetSize(FX_HFILE hFile)
{
    FXSYS_assert(hFile != NULL);
    return ((IFXCRT_FileAccess*)hFile)->GetSize();
}
FX_FILESIZE FX_File_GetPosition(FX_HFILE hFile)
{
    FXSYS_assert(hFile != NULL);
    return ((IFXCRT_FileAccess*)hFile)->GetPosition();
}
FX_FILESIZE FX_File_SetPosition(FX_HFILE hFile, FX_FILESIZE pos)
{
    FXSYS_assert(hFile != NULL);
    return ((IFXCRT_FileAccess*)hFile)->SetPosition(pos);
}
size_t FX_File_Read(FX_HFILE hFile, void* pBuffer, size_t szBuffer)
{
    FXSYS_assert(hFile != NULL);
    return ((IFXCRT_FileAccess*)hFile)->Read(pBuffer, szBuffer);
}
size_t FX_File_ReadPos(FX_HFILE hFile, void* pBuffer, size_t szBuffer, FX_FILESIZE pos)
{
    FXSYS_assert(hFile != NULL);
    return ((IFXCRT_FileAccess*)hFile)->ReadPos(pBuffer, szBuffer, pos);
}
size_t FX_File_Write(FX_HFILE hFile, const void* pBuffer, size_t szBuffer)
{
    FXSYS_assert(hFile != NULL);
    return ((IFXCRT_FileAccess*)hFile)->Write(pBuffer, szBuffer);
}
size_t FX_File_WritePos(FX_HFILE hFile, const void* pBuffer, size_t szBuffer, FX_FILESIZE pos)
{
    FXSYS_assert(hFile != NULL);
    return ((IFXCRT_FileAccess*)hFile)->WritePos(pBuffer, szBuffer, pos);
}
FX_BOOL FX_File_Flush(FX_HFILE hFile)
{
    FXSYS_assert(hFile != NULL);
    return ((IFXCRT_FileAccess*)hFile)->Flush();
}
FX_BOOL FX_File_Truncate(FX_HFILE hFile, FX_FILESIZE szFile)
{
    FXSYS_assert(hFile != NULL);
    return ((IFXCRT_FileAccess*)hFile)->Truncate(szFile);
}
IFX_FileStream* FX_CreateFileStream(FX_LPCSTR filename, FX_DWORD dwModes)
{
    IFXCRT_FileAccess* pFA = FXCRT_FileAccess_Create();
    if (!pFA) {
        return NULL;
    }
    if (!pFA->Open(filename, dwModes)) {
        pFA->Release();
        return NULL;
    }
    return FX_NEW CFX_CRTFileStream(pFA);
}
IFX_FileStream* FX_CreateFileStream(FX_LPCWSTR filename, FX_DWORD dwModes)
{
    IFXCRT_FileAccess* pFA = FXCRT_FileAccess_Create();
    if (!pFA) {
        return NULL;
    }
    if (!pFA->Open(filename, dwModes)) {
        pFA->Release();
        return NULL;
    }
    return FX_NEW CFX_CRTFileStream(pFA);
}
IFX_FileWrite* FX_CreateFileWrite(FX_LPCSTR filename)
{
    return FX_CreateFileStream(filename, FX_FILEMODE_Truncate);
}
IFX_FileWrite* FX_CreateFileWrite(FX_LPCWSTR filename)
{
    return FX_CreateFileStream(filename, FX_FILEMODE_Truncate);
}
IFX_FileRead* FX_CreateFileRead(FX_LPCSTR filename)
{
    return FX_CreateFileStream(filename, FX_FILEMODE_ReadOnly);
}
IFX_FileRead* FX_CreateFileRead(FX_LPCWSTR filename)
{
    return FX_CreateFileStream(filename, FX_FILEMODE_ReadOnly);
}
IFX_MemoryStream* FX_CreateMemoryStream(FX_LPBYTE pBuffer, size_t dwSize, FX_BOOL bTakeOver)
{
    return FX_NEW CFX_MemoryStream(pBuffer, dwSize, bTakeOver);
}
IFX_MemoryStream* FX_CreateMemoryStream(FX_BOOL bConsecutive)
{
    return FX_NEW CFX_MemoryStream(bConsecutive);
}
#ifdef __cplusplus
extern "C" {
#endif
FX_FLOAT FXSYS_tan(FX_FLOAT a)
{
    return (FX_FLOAT)tan(a);
}
FX_FLOAT FXSYS_logb(FX_FLOAT b, FX_FLOAT x)
{
    return FXSYS_log(x) / FXSYS_log(b);
}
FX_FLOAT FXSYS_strtof(FX_LPCSTR pcsStr, FX_INT32 iLength, FX_INT32 *pUsedLen)
{
    FXSYS_assert(pcsStr != NULL);
    if (iLength < 0) {
        iLength = (FX_INT32)FXSYS_strlen(pcsStr);
    }
    CFX_WideString ws = CFX_WideString::FromLocal(pcsStr, iLength);
    return FXSYS_wcstof((FX_LPCWSTR)ws, iLength, pUsedLen);
}
FX_FLOAT FXSYS_wcstof(FX_LPCWSTR pwsStr, FX_INT32 iLength, FX_INT32 *pUsedLen)
{
    FXSYS_assert(pwsStr != NULL);
    if (iLength < 0) {
        iLength = (FX_INT32)FXSYS_wcslen(pwsStr);
    }
    if (iLength == 0) {
        return 0.0f;
    }
    FX_INT32 iUsedLen = 0;
    FX_BOOL bNegtive = FALSE;
    switch (pwsStr[iUsedLen]) {
        case '-':
            bNegtive = TRUE;
        case '+':
            iUsedLen++;
            break;
    }
    FX_FLOAT fValue = 0.0f;
    while (iUsedLen < iLength) {
        FX_WCHAR wch = pwsStr[iUsedLen];
        if (wch >= L'0' && wch <= L'9') {
            fValue = fValue * 10.0f + (wch - L'0');
        } else {
            break;
        }
        iUsedLen++;
    }
    if (iUsedLen < iLength && pwsStr[iUsedLen] == L'.') {
        FX_FLOAT fPrecise = 0.1f;
        while (++iUsedLen < iLength) {
            FX_WCHAR wch = pwsStr[iUsedLen];
            if (wch >= L'0' && wch <= L'9') {
                fValue += (wch - L'0') * fPrecise;
                fPrecise *= 0.1f;
            } else {
                break;
            }
        }
    }
    if (pUsedLen) {
        *pUsedLen = iUsedLen;
    }
    return bNegtive ? -fValue : fValue;
}
FX_LPWSTR FXSYS_wcsncpy(FX_LPWSTR dstStr, FX_LPCWSTR srcStr, size_t count)
{
    FXSYS_assert(dstStr != NULL && srcStr != NULL && count > 0);
    for (size_t i = 0; i < count; ++i)
        if ((dstStr[i] = srcStr[i]) == L'\0') {
            break;
        }
    return dstStr;
}
FX_INT32 FXSYS_wcsnicmp(FX_LPCWSTR s1, FX_LPCWSTR s2, size_t count)
{
    FXSYS_assert(s1 != NULL && s2 != NULL && count > 0);
    FX_WCHAR wch1 = 0, wch2 = 0;
    while (count-- > 0) {
        wch1 = (FX_WCHAR)FXSYS_tolower(*s1++);
        wch2 = (FX_WCHAR)FXSYS_tolower(*s2++);
        if (wch1 != wch2) {
            break;
        }
    }
    return wch1 - wch2;
}
FX_INT32 FXSYS_strnicmp(FX_LPCSTR s1, FX_LPCSTR s2, size_t count)
{
    FXSYS_assert(s1 != NULL && s2 != NULL && count > 0);
    FX_CHAR ch1 = 0, ch2 = 0;
    while (count-- > 0) {
        ch1 = (FX_CHAR)FXSYS_tolower(*s1++);
        ch2 = (FX_CHAR)FXSYS_tolower(*s2++);
        if (ch1 != ch2) {
            break;
        }
    }
    return ch1 - ch2;
}
FX_DWORD FX_HashCode_String_GetA(FX_LPCSTR pStr, FX_INT32 iLength, FX_BOOL bIgnoreCase)
{
    FXSYS_assert(pStr != NULL);
    if (iLength < 0) {
        iLength = (FX_INT32)FXSYS_strlen(pStr);
    }
    FX_LPCSTR pStrEnd = pStr + iLength;
    FX_DWORD dwHashCode = 0;
    if (bIgnoreCase) {
        while (pStr < pStrEnd) {
            dwHashCode = 31 * dwHashCode + FXSYS_tolower(*pStr++);
        }
    } else {
        while (pStr < pStrEnd) {
            dwHashCode = 31 * dwHashCode + *pStr ++;
        }
    }
    return dwHashCode;
}
FX_DWORD FX_HashCode_String_GetW(FX_LPCWSTR pStr, FX_INT32 iLength, FX_BOOL bIgnoreCase)
{
    FXSYS_assert(pStr != NULL);
    if (iLength < 0) {
        iLength = (FX_INT32)FXSYS_wcslen(pStr);
    }
    FX_LPCWSTR pStrEnd = pStr + iLength;
    FX_DWORD dwHashCode = 0;
    if (bIgnoreCase) {
        while (pStr < pStrEnd) {
            dwHashCode = 1313 * dwHashCode + FXSYS_tolower(*pStr++);
        }
    } else {
        while (pStr < pStrEnd) {
            dwHashCode = 1313 * dwHashCode + *pStr ++;
        }
    }
    return dwHashCode;
}
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
FX_LPVOID FX_Random_MT_Start(FX_DWORD dwSeed)
{
    FX_LPMTRANDOMCONTEXT pContext = FX_Alloc(FX_MTRANDOMCONTEXT, 1);
    if (!pContext) {
        return NULL;
    }
    pContext->mt[0] = dwSeed;
    FX_DWORD &i = pContext->mti;
    FX_LPDWORD pBuf = pContext->mt;
    for (i = 1; i < MT_N; i ++) {
        pBuf[i] = (1812433253UL * (pBuf[i - 1] ^ (pBuf[i - 1] >> 30)) + i);
    }
    pContext->bHaveSeed = TRUE;
    return pContext;
}
FX_DWORD FX_Random_MT_Generate(FX_LPVOID pContext)
{
    FXSYS_assert(pContext != NULL);
    FX_LPMTRANDOMCONTEXT pMTC = (FX_LPMTRANDOMCONTEXT)pContext;
    FX_DWORD v;
    static FX_DWORD mag[2] = {0, MT_Matrix_A};
    FX_DWORD &mti = pMTC->mti;
    FX_LPDWORD pBuf = pMTC->mt;
    if ((int)mti < 0 || mti >= MT_N) {
        if (mti > MT_N && !pMTC->bHaveSeed) {
            return 0;
        }
        FX_DWORD kk;
        for (kk = 0; kk < MT_N - MT_M; kk ++) {
            v = (pBuf[kk] & MT_Upper_Mask) | (pBuf[kk + 1] & MT_Lower_Mask);
            pBuf[kk] = pBuf[kk + MT_M] ^ (v >> 1) ^ mag[v & 1];
        }
        for (; kk < MT_N - 1; kk ++) {
            v = (pBuf[kk] & MT_Upper_Mask) | (pBuf[kk + 1] & MT_Lower_Mask);
            pBuf[kk] = pBuf[kk + (MT_M - MT_N)] ^ (v >> 1) ^ mag[v & 1];
        }
        v = (pBuf[MT_N - 1] & MT_Upper_Mask) | (pBuf[0] & MT_Lower_Mask);
        pBuf[MT_N - 1] = pBuf[MT_M - 1] ^ (v >> 1) ^ mag[v & 1];
        mti = 0;
    }
    v = pBuf[mti ++];
    v ^= (v >> 11);
    v ^= (v << 7) & 0x9d2c5680UL;
    v ^= (v << 15) & 0xefc60000UL;
    v ^= (v >> 18);
    return v;
}
void FX_Random_MT_Close(FX_LPVOID pContext)
{
    FXSYS_assert(pContext != NULL);
    FX_Free(pContext);
}
void FX_Random_GenerateMT(FX_LPDWORD pBuffer, FX_INT32 iCount)
{
    FX_DWORD dwSeed;
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
    if (!FX_GenerateCryptoRandom(&dwSeed, 1)) {
        FX_Random_GenerateBase(&dwSeed, 1);
    }
#else
    FX_Random_GenerateBase(&dwSeed, 1);
#endif
    FX_LPVOID pContext = FX_Random_MT_Start(dwSeed);
    while (iCount -- > 0) {
        *pBuffer ++ = FX_Random_MT_Generate(pContext);
    }
    FX_Random_MT_Close(pContext);
}
void FX_Random_GenerateBase(FX_LPDWORD pBuffer, FX_INT32 iCount)
{
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
    SYSTEMTIME st1, st2;
    ::GetSystemTime(&st1);
    do {
        ::GetSystemTime(&st2);
    } while (FXSYS_memcmp32(&st1, &st2, sizeof(SYSTEMTIME)) == 0);
    FX_DWORD dwHash1 = FX_HashCode_String_GetA((FX_LPCSTR)&st1, sizeof(st1), TRUE);
    FX_DWORD dwHash2 = FX_HashCode_String_GetA((FX_LPCSTR)&st2, sizeof(st2), TRUE);
    ::srand((dwHash1 << 16) | (FX_DWORD)dwHash2);
#else
    time_t tmLast = time(NULL), tmCur;
    while ((tmCur = time(NULL)) == tmLast);
    ::srand((tmCur << 16) | (tmLast & 0xFFFF));
#endif
    while (iCount -- > 0) {
        *pBuffer ++ = (FX_DWORD)((::rand() << 16) | (::rand() & 0xFFFF));
    }
}
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
FX_BOOL FX_GenerateCryptoRandom(FX_LPDWORD pBuffer, FX_INT32 iCount)
{
    HCRYPTPROV hCP = NULL;
    if (!::CryptAcquireContext(&hCP, NULL, NULL, PROV_RSA_FULL, 0) || hCP == NULL) {
        return FALSE;
    }
    ::CryptGenRandom(hCP, iCount * sizeof(FX_DWORD), (FX_LPBYTE)pBuffer);
    ::CryptReleaseContext(hCP, 0);
    return TRUE;
}
#endif
void FX_Random_GenerateCrypto(FX_LPDWORD pBuffer, FX_INT32 iCount)
{
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
    FX_GenerateCryptoRandom(pBuffer, iCount);
#else
    FX_Random_GenerateBase(pBuffer, iCount);
#endif
}
#ifdef __cplusplus
}
#endif
const static FX_BYTE g_FX_Base64_Decoder[128] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0x3F,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
    0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
FX_BYTE FXCRT_DefBase64Decoder(FX_WCHAR wch)
{
    return g_FX_Base64_Decoder[wch & 0x7F];
}
CFX_Base64Decoder::CFX_Base64Decoder(FX_WCHAR wEqual)
{
    m_pData = (FX_LPVOID)FXCRT_DefBase64Decoder;
    m_wEqual = wEqual;
}
void CFX_Base64Decoder::SetDecoder(FX_LPBase64Decoder pDecoder)
{
    m_pData = (FX_LPVOID)pDecoder;
}
FX_INT32 CFX_Base64Decoder::Decode(FX_LPCSTR pSrc, FX_INT32 iSrcLen, FX_LPBYTE pDst)
{
    FXSYS_assert(pSrc != NULL);
    if (iSrcLen < 1) {
        return 0;
    }
    while (iSrcLen > 0 && pSrc[iSrcLen - 1] == '=') {
        iSrcLen --;
    }
    if (iSrcLen < 1) {
        return 0;
    }
    if (pDst == NULL) {
        FX_INT32 iDstLen = iSrcLen / 4 * 3;
        iSrcLen %= 4;
        if (iSrcLen == 1) {
            iDstLen += 1;
        } else if (iSrcLen == 2) {
            iDstLen += 1;
        } else if (iSrcLen == 3) {
            iDstLen += 2;
        }
        return iDstLen;
    }
    FX_BYTE c = 0, b1 = 0, b2 = 0, b3 = 0;
    FX_INT32 iChars = 4, iBytes = 0;
    FX_LPBYTE pDstEnd = pDst;
    while (iSrcLen > 0) {
        if (iSrcLen > 3) {
            iSrcLen -= 4;
        } else {
            iChars = iSrcLen;
            iSrcLen = 0;
        }
        iBytes = 1;
        c = g_FX_Base64_Decoder[(FX_INT32)pSrc[0]];
        b1 = c << 2;
        if (iChars > 1) {
            c = g_FX_Base64_Decoder[(FX_INT32)pSrc[1]];
            b1 |= (c >> 4);
            b2 = c << 4;
            if (iChars > 2) {
                iBytes = 2;
                c = g_FX_Base64_Decoder[(FX_INT32)pSrc[2]];
                b2 |= (c >> 2);
                b3 = c << 6;
                if (iChars > 3) {
                    iBytes = 3;
                    b3 |= g_FX_Base64_Decoder[(FX_INT32)pSrc[3]];
                }
            }
        }
        *pDstEnd ++ = b1;
        if (iBytes > 1) {
            *pDstEnd ++ = b2;
        }
        if (iBytes > 2) {
            *pDstEnd ++ = b3;
        }
        pSrc += iChars;
    }
    return (FX_INT32)(pDstEnd - pDst);
}
FX_INT32 CFX_Base64Decoder::Decode(FX_LPCWSTR pSrc, FX_INT32 iSrcLen, FX_LPBYTE pDst)
{
    FXSYS_assert(pSrc != NULL);
    if (iSrcLen < 1) {
        return 0;
    }
    while (iSrcLen > 0 && pSrc[iSrcLen - 1] == m_wEqual) {
        iSrcLen --;
    }
    if (iSrcLen < 1) {
        return 0;
    }
    if (pDst == NULL) {
        FX_INT32 iDstLen = iSrcLen / 4 * 3;
        iSrcLen %= 4;
        if (iSrcLen == 1) {
            iDstLen += 1;
        } else if (iSrcLen == 2) {
            iDstLen += 1;
        } else if (iSrcLen == 3) {
            iDstLen += 2;
        }
        return iDstLen;
    }
    FX_BYTE c = 0, b1 = 0, b2 = 0, b3 = 0;
    FX_INT32 iChars = 4, iBytes;
    FX_LPBYTE pDstEnd = pDst;
    while (iSrcLen > 0) {
        if (iSrcLen > 3) {
            iSrcLen -= 4;
        } else {
            iChars = iSrcLen;
            iSrcLen = 0;
        }
        iBytes = 1;
        c = ((FX_LPBase64Decoder)m_pData)(pSrc[0]);
        b1 = c << 2;
        if (iChars > 1) {
            c = ((FX_LPBase64Decoder)m_pData)(pSrc[1]);
            b1 |= (c >> 4);
            b2 = c << 4;
            if (iChars > 2) {
                iBytes = 2;
                c = ((FX_LPBase64Decoder)m_pData)(pSrc[2]);
                b2 |= (c >> 2);
                b3 = c << 6;
                if (iChars > 3) {
                    iBytes = 3;
                    b3 |= ((FX_LPBase64Decoder)m_pData)(pSrc[3]);
                }
            }
        }
        *pDstEnd ++ = b1;
        if (iBytes > 1) {
            *pDstEnd ++ = b2;
        }
        if (iBytes > 2) {
            *pDstEnd ++ = b3;
        }
        pSrc += iChars;
    }
    return (FX_INT32)(pDstEnd - pDst);
}
FX_INT32 CFX_Base64Decoder::Decode(const CFX_WideStringC &src, CFX_ByteString &dst)
{
    FX_INT32 iDstLen = Decode(src.GetPtr(), src.GetLength(), NULL);
    if (iDstLen > 0) {
        FX_LPBYTE pBuf = (FX_LPBYTE)dst.GetBuffer(iDstLen);
        Decode(src.GetPtr(), src.GetLength(), pBuf);
        dst.ReleaseBuffer(iDstLen);
    }
    return iDstLen;
}
FX_INT32 CFX_Base64Decoder::Decode(const CFX_ByteStringC &src, CFX_ByteString &dst)
{
    CFX_WideString ws = CFX_WideString::FromUTF8(src.GetCStr(), src.GetLength());
    return Decode(ws, dst);
}

void FX_Time_GetSystemTime(FX_SYSTEMTIME* pSystemTime)
{
    if (!pSystemTime) return;
#if _FX_OS_ == _FX_WIN32_DESKTOP_
    ::GetSystemTime((LPSYSTEMTIME)pSystemTime);
#else
    timeval curTime;
    gettimeofday(&curTime, NULL);
    struct tm st;
    gmtime_r(&curTime.tv_sec, &st);
    pSystemTime->wYear = st.tm_year + 1900;
    pSystemTime->wMonth = st.tm_mon + 1;
    pSystemTime->wDayOfWeek = st.tm_wday;
    pSystemTime->wDay = st.tm_mday;
    pSystemTime->wHour = st.tm_hour;
    pSystemTime->wMinute = st.tm_min;
    pSystemTime->wSecond = st.tm_sec;
    pSystemTime->wMilliseconds = curTime.tv_usec / 1000;
#endif
}
void FXCRT_GetCurrentSystemTime(FXCRT_DATETIMEZONE& dt)
{
    FXSYS_memset(&dt, 0, sizeof(dt));

#if _FX_OS_ == _FX_LINUX_DESKTOP
    FX_SYSTEMTIME sysTime;
    FX_Time_GetSystemTime(&sysTime);
    tzset();
    FX_INT32 tzHour = __timezone / 3600 * -1;
    FX_INT32 tzMin = (FXSYS_abs(__timezone) % 3600) / 60;
#elif _FX_OS_ == _FX_WIN32_DESKTOP_
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);
    _tzset();
    long timezone_second;
    _get_timezone(&timezone_second);
    FX_INT32 tzHour = timezone_second / 3600 * -1;
    FX_INT32 tzMin = (FXSYS_abs(timezone_second) % 3600) / 60;
#endif

    dt.year = sysTime.wYear;
    dt.month = sysTime.wMonth;
    dt.day = sysTime.wDay;
    dt.dayOfWeek = sysTime.wDayOfWeek;
    dt.hour = sysTime.wHour;
    dt.minute = sysTime.wMinute;
    dt.second = sysTime.wSecond;
    dt.milliseconds = sysTime.wMilliseconds;
    dt.tzHour = tzHour;
    dt.tzMinute = tzMin;
}
