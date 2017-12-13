#include "../../include/fxcrt/fx_ext.h"
static CFX_ByteString _DefMap_GetByteString(CFX_CharMap* pCharMap, const CFX_WideString& widestr)
{
    int src_len = widestr.GetLength();
    int codepage = pCharMap->m_GetCodePage ? pCharMap->m_GetCodePage() : 0;
    int dest_len = FXSYS_WideCharToMultiByte(codepage, 0, widestr, src_len, NULL, 0, NULL, NULL);
    if (dest_len == 0) {
        return CFX_ByteString();
    }
    CFX_ByteString bytestr;
    FX_LPSTR dest_buf = bytestr.GetBuffer(dest_len);
    FXSYS_WideCharToMultiByte(codepage, 0, widestr, src_len, dest_buf, dest_len, NULL, NULL);
    bytestr.ReleaseBuffer(dest_len);
    return bytestr;
}
static CFX_WideString _DefMap_GetWideString(CFX_CharMap* pCharMap, const CFX_ByteString& bytestr)
{
    int src_len = bytestr.GetLength();
    int codepage = pCharMap->m_GetCodePage ? pCharMap->m_GetCodePage() : 0;
    int dest_len = FXSYS_MultiByteToWideChar(codepage, 0, bytestr, src_len, NULL, 0);
    if (dest_len == 0) {
        return CFX_WideString();
    }
    CFX_WideString widestr;
    FX_LPWSTR dest_buf = widestr.GetBuffer(dest_len);
    FXSYS_MultiByteToWideChar(codepage, 0, bytestr, src_len, dest_buf, dest_len);
    widestr.ReleaseBuffer(dest_len);
    return widestr;
}
static const CFX_CharMap g_DefaultMapper = {&_DefMap_GetWideString, &_DefMap_GetByteString, NULL};
static int _DefMap_GetGBKCodePage()
{
    return 936;
}
static int _DefMap_GetUHCCodePage()
{
    return 949;
}
static int _DefMap_GetJISCodePage()
{
    return 932;
}
static int _DefMap_GetBig5CodePage()
{
    return 950;
}
static const CFX_CharMap g_DefaultGBKMapper = {&_DefMap_GetWideString, &_DefMap_GetByteString, &_DefMap_GetGBKCodePage};
static const CFX_CharMap g_DefaultJISMapper = {&_DefMap_GetWideString, &_DefMap_GetByteString, &_DefMap_GetJISCodePage};
static const CFX_CharMap g_DefaultUHCMapper = {&_DefMap_GetWideString, &_DefMap_GetByteString, &_DefMap_GetUHCCodePage};
static const CFX_CharMap g_DefaultBig5Mapper = {&_DefMap_GetWideString, &_DefMap_GetByteString, &_DefMap_GetBig5CodePage};
CFX_CharMap* CFX_CharMap::GetDefaultMapper(FX_INT32 codepage)
{
    switch (codepage) {
        case 0:
            return (CFX_CharMap*)&g_DefaultMapper;
        case 932:
            return (CFX_CharMap*)&g_DefaultJISMapper;
        case 936:
            return (CFX_CharMap*)&g_DefaultGBKMapper;
        case 949:
            return (CFX_CharMap*)&g_DefaultUHCMapper;
        case 950:
            return (CFX_CharMap*)&g_DefaultBig5Mapper;
    }
    return NULL;
}
