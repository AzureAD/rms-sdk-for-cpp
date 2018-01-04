#ifndef _FXCRT_EXTENSION_
#define _FXCRT_EXTENSION_
#ifndef _FX_BASIC_H_
#include "fx_basic.h"
#endif
#ifndef _FXCRT_COORDINATES_
#include "fx_coordinates.h"
#endif
#ifndef _FX_XML_H_
#include "fx_xml.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif


FX_FLOAT		FXSYS_tan(FX_FLOAT a);
FX_FLOAT		FXSYS_logb(FX_FLOAT b, FX_FLOAT x);
FX_FLOAT		FXSYS_strtof(FX_LPCSTR pcsStr, FX_INT32 iLength = -1, FX_INT32 *pUsedLen = NULL);
FX_FLOAT		FXSYS_wcstof(FX_LPCWSTR pwsStr, FX_INT32 iLength = -1, FX_INT32 *pUsedLen = NULL);
FX_LPWSTR		FXSYS_wcsncpy(FX_LPWSTR dstStr, FX_LPCWSTR srcStr, size_t count);
FX_INT32		FXSYS_wcsnicmp(FX_LPCWSTR s1, FX_LPCWSTR s2, size_t count);
FX_INT32		FXSYS_strnicmp(FX_LPCSTR s1, FX_LPCSTR s2, size_t count);
inline FX_BOOL	FXSYS_islower(FX_INT32 ch)
{
    return ch >= 'a' && ch <= 'z';
}
inline FX_BOOL	FXSYS_isupper(FX_INT32 ch)
{
    return ch >= 'A' && ch <= 'Z';
}
inline FX_INT32	FXSYS_tolower(FX_INT32 ch)
{
    return ch < 'A' || ch > 'Z' ? ch : (ch + 0x20);
}
inline FX_INT32 FXSYS_toupper(FX_INT32 ch)
{
    return ch < 'a' || ch > 'z' ? ch : (ch - 0x20);
}



FX_DWORD	FX_HashCode_String_GetA(FX_LPCSTR pStr, FX_INT32 iLength, FX_BOOL bIgnoreCase = FALSE);
FX_DWORD	FX_HashCode_String_GetW(FX_LPCWSTR pStr, FX_INT32 iLength, FX_BOOL bIgnoreCase = FALSE);

#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif

FX_LPVOID	FX_Random_MT_Start(FX_DWORD dwSeed);

FX_DWORD	FX_Random_MT_Generate(FX_LPVOID pContext);

void		FX_Random_MT_Close(FX_LPVOID pContext);

void		FX_Random_GenerateBase(FX_LPDWORD pBuffer, FX_INT32 iCount);

void		FX_Random_GenerateMT(FX_LPDWORD pBuffer, FX_INT32 iCount);

void		FX_Random_GenerateCrypto(FX_LPDWORD pBuffer, FX_INT32 iCount);
#ifdef __cplusplus
}
#endif
typedef FX_BYTE (*FX_LPBase64Decoder)(FX_WCHAR wch);
class CFX_Base64Decoder : public CFX_Object
{
public:

    explicit CFX_Base64Decoder(FX_WCHAR wEqual = '=');

    void		SetDecoder(FX_LPBase64Decoder pDecoder);

    FX_INT32	Decode(FX_LPCSTR pSrc, FX_INT32 iSrcLen, FX_LPBYTE pDst);

    FX_INT32	Decode(FX_LPCWSTR pSrc, FX_INT32 iSrcLen, FX_LPBYTE pDst);

    FX_INT32	Decode(const CFX_WideStringC &src, CFX_ByteString &dst);

    FX_INT32	Decode(const CFX_ByteStringC &src, CFX_ByteString &dst);
protected:
    FX_LPVOID	m_pData;
    FX_WCHAR	m_wEqual;
};
#ifdef __cplusplus
extern "C" {
#endif
typedef struct _FX_SYSTEMTIME
{
    FX_WORD wYear;
    FX_WORD wMonth;
    FX_WORD wDayOfWeek;
    FX_WORD wDay;
    FX_WORD wHour;
    FX_WORD wMinute;
    FX_WORD wSecond;
    FX_WORD wMilliseconds;
} FX_SYSTEMTIME;
void		FX_Time_GetSystemTime(FX_SYSTEMTIME* pSystemTime);
typedef struct _FXCRT_DATETIMEZONE
{
    FX_WORD		year;
    FX_WORD		month;
    FX_WORD		day;
    FX_WORD		dayOfWeek;
    FX_WORD		hour;
    FX_WORD		minute;
    FX_WORD		second;
    FX_WORD		milliseconds;
    FX_INT32	tzHour;
    FX_WORD		tzMinute;
} FXCRT_DATETIMEZONE;
void		FXCRT_GetCurrentSystemTime(FXCRT_DATETIMEZONE& dt);
#ifdef __cplusplus
}
#endif
#endif
