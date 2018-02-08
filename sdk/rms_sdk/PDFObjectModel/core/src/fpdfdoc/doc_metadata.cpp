#include "../../include/fpdfdoc/fpdf_doc.h"
#include "../../include/fxcrt/fx_xml.h"
#include "pdf_datetime.h"
#define  CPDF_METADATA_KEYWORDS_PDF			0
#define  CPDF_METADATA_KEYWORDS_DC			1
#define  CPDF_METADATA_KEYWORDS_ALL			2
static FX_WORD keywordsSeparator[] = {0x3B, 0xFF1B, 0x2C, 0xFF0C, 0x3001, 0x0D, 0x0A};
static FX_WORD DoubleQuotation[] = {0x22, 0x22, 0x201C, 0x201D};
static FX_WORD authorSeperator[] = {0x3B, 0x20};
static FX_LPCSTR bsKeyType[] = {"Title", "Subject", "Author", "Keywords", "Producer", "Creator", "Trapped", "CreationDate", "ModDate"};
static FX_BOOL setNewDCToXML(CXML_Element* pXMLElement, CXML_Element* pDCXMLElement, CFX_ByteString bsXMLSpace, CFX_ByteString bsXMLKey, CFX_WideStringArray& wsValue);
class CPDF_KeyValueStringArray
{
public:
    static void					WideStringArrayToWideString(FX_BSTR bsItem, const CFX_WideStringArray& wsArray , CFX_WideString& wsStr, FX_BOOL bKeypdf = TRUE);
    static void					GetAuthorArray(const CFX_WideString& wsValue, CFX_WideStringArray& wsArray, FX_BOOL bArray = FALSE);
    static void					GetKeyWordsArray(const CFX_WideString& wsValue, CFX_WideStringArray& wsArray);
    static FX_BOOL				CompareKeywords(const CFX_WideStringArray& wsArray, const CFX_WideString& wsValue);
    static CFX_WideString		GetFirst(const CFX_WideString& keyValue);
protected:
    static FX_BOOL		FindSeparate(const CFX_WideString& wsStr);
    static FX_INT32		SeparateDoubleQuotation(FX_INT32& index, CFX_WideString& ws, CFX_WideStringArray& wsArray);
    static FX_INT32		FilterSpace(const CFX_WideString& wsStr, FX_INT32 index);
};
class CPDF_MetadataRDFValue : public CFX_Object
{
public:
    CFX_ByteString bsNameSpace;
    CFX_ByteString bsName;
    CFX_WideString wsContent;
};
static FX_INT32 GetXMPArrayValue(CXML_Element* pXMLElement, CFX_WideStringArray& keyArray)
{
    CFX_ByteString strArrayKey;
    CFX_ByteString strArrayKeyArr[] = { "Seq", "Bag", "Alt" };
    for (FX_INT32 q = 0; q < sizeof(strArrayKeyArr) / sizeof(strArrayKeyArr[0]); q++) {
        if (0 != pXMLElement->CountElements(FX_BSTRC("rdf"), strArrayKeyArr[q])) {
            strArrayKey = strArrayKeyArr[q];
            break;
        }
    }
    if (strArrayKey.IsEmpty()) {
        return -1;
    }
    CXML_Element* pSubXMLElement = pXMLElement->GetElement(FX_BSTRC("rdf"), strArrayKey);
    if (!pSubXMLElement) {
        return -1;
    }
    int dwCountArray = pSubXMLElement->CountElements(FX_BSTRC("rdf"), FX_BSTRC("li"));
    if (strArrayKey.Equal("Alt") && dwCountArray > 0) {
        dwCountArray = 1;
    }
    for (int i = 0; i < dwCountArray; i++) {
        CXML_Element* pLiXMLElement = pSubXMLElement->GetElement(FX_BSTRC("rdf"), FX_BSTRC("li"), i);
        if (!pLiXMLElement) {
            continue;
        }
        CFX_WideString wsStr = pLiXMLElement->GetContent(0);
        keyArray.Add(wsStr);
    }
    return 0;
}
FX_BOOL CPDF_KeyValueStringArray::FindSeparate(const CFX_WideString& wsStr)
{
    for (FX_INT32 i = 0; i < wsStr.GetLength(); i++) {
        FX_WCHAR wchar = wsStr.GetAt(i);
        for (FX_INT32 j = 0; j < sizeof(keywordsSeparator) / sizeof(keywordsSeparator[0]); j++) {
            if (wchar == keywordsSeparator[j]) {
                if (wchar == 0x0D) {
                    if (wsStr.GetAt(i + 1) == 0x0A) {
                        return TRUE;
                    }
                    continue;
                }
                return TRUE;
            }
        }
    }
    return FALSE;
}
void CPDF_KeyValueStringArray::WideStringArrayToWideString(FX_BSTR bsItem, const CFX_WideStringArray& wsArray , CFX_WideString& wsStr, FX_BOOL bKeypdf)
{
    wsStr = L"";
    CFX_WideString wsValue;
    FX_BOOL bSepa = FALSE;
    if (bsItem == FX_BSTRC("Author") || (bsItem == FX_BSTRC("Keywords") && bKeypdf)) {
        bSepa = TRUE;
    }
    for (FX_INT32 i = 0; i < wsArray.GetSize(); i++) {
        wsValue = *wsArray.GetDataPtr(i);
        if (bSepa && FindSeparate(wsValue)) {
            FX_INT32 j = 0;
            do {
                FX_WCHAR wchar = wsValue.GetAt(j++);
                if (wchar == DoubleQuotation[1]) {
                    wsValue.Insert(j++, DoubleQuotation[1]);
                }
            } while(j < wsValue.GetLength());
            wsValue = CFX_WideString(L"\"") + wsValue + CFX_WideString(L"\"");
        }
        wsStr += wsValue;
        if (i != wsArray.GetSize() - 1) {
            wsStr += L"; ";
        }
    }
}
FX_INT32 CPDF_KeyValueStringArray::SeparateDoubleQuotation(FX_INT32& index, CFX_WideString& ws, CFX_WideStringArray& wsArray)
{
    CFX_WideString wsStr;
    FX_WCHAR wchar = ws.GetAt(index);
    if (wchar == DoubleQuotation[3]) {
        wsStr = ws.Mid(index + 1, ws.GetLength() - index - 1);
        wsArray.Add(wsStr);
        return 0;
    }
    FX_WCHAR wsEnd = 0;
    if (wchar == DoubleQuotation[0]) {
        wsEnd = DoubleQuotation[1];
    } else if (wchar == DoubleQuotation[2]) {
        wsEnd = DoubleQuotation[3];
    }
    if (wsEnd != 0) {
        FX_INT32 k = ++index;
        FX_WCHAR next = 0;
        FX_INT32 afterIndex = 0;
        do {
            wchar = ws.GetAt(index);
            if (wsEnd == DoubleQuotation[1] &&  index < ws.GetLength() - 1) {
                next = ws.GetAt(index + 1);
                afterIndex = index + 1;
            }
            if (wchar == wsEnd && next != wsEnd) {
                break;
            } else if (wchar == wsEnd && next == wsEnd) {
                ws.Delete(afterIndex, 1);
            }
            index++;
        } while (index < ws.GetLength());
        if (index < ws.GetLength()) {
            wsStr = ws.Mid(k, index - k);
            if (wsStr.GetLength() != 0) {
                wsArray.Add(wsStr);
            }
            return 1;
        } else {
            wsStr = ws.Mid(k, ws.GetLength() - k);
            wsArray.Add(wsStr);
            return 0;
        }
    }
    return -1;
}
FX_INT32 CPDF_KeyValueStringArray::FilterSpace(const CFX_WideString& ws, FX_INT32 index)
{
    CFX_WideString wsStr;
    do {
        wsStr = ws.GetAt(index++);
    } while (wsStr == L" " && index < ws.GetLength());
    if (wsStr != L" ") {
        index --;
    }
    return index;
}
void CPDF_KeyValueStringArray::GetAuthorArray(const CFX_WideString& wsValue, CFX_WideStringArray& wsArray, FX_BOOL bArray)
{
    CFX_WideString ws = wsValue;
    CFX_WideString wsStr;
    FX_STRSIZE star = 0;
    FX_INT32 i = 0, j = 0;
    FX_BOOL flag = TRUE;
    i = FilterSpace(ws, i);
    for(; i < ws.GetLength(); i++) {
        FX_WCHAR wchar = ws.GetAt(i);
        if (flag && !bArray) {
            FX_INT32 ret = SeparateDoubleQuotation(i, ws, wsArray);
            if (ret == 0) {
                return;
            }
            if (ret == 1) {
                star = FilterSpace(ws, i + 1);
                i = star - 1;
                continue;
            }
        }
        for (j = 0; j < sizeof(authorSeperator) / sizeof(authorSeperator[0]); j++) {
            if (wchar == authorSeperator[j]) {
                if (j == sizeof(authorSeperator) / sizeof(authorSeperator[0]) - 1) {
                    wchar = ws.GetAt(i + 1);
                    if (wchar != authorSeperator[j]) {
                        break;
                    }
                }
                wsStr = ws.Mid(star, i - star);
                if (wsStr.GetLength() != 0) {
                    wsArray.Add(wsStr);
                }
                star = FilterSpace(ws, i + 1);
                i = star - 1;
                flag = TRUE;
                break;
            }
        }
        if (j >= sizeof(authorSeperator) / sizeof(authorSeperator[0])) {
            flag = FALSE;
        }
    }
    wsStr = ws.Mid(star, i - star);
    if (wsStr.GetLength() != 0 || ws.GetLength() == 0) {
        wsArray.Add(wsStr);
    }
}
void CPDF_KeyValueStringArray::GetKeyWordsArray(const CFX_WideString& wsValue, CFX_WideStringArray& wsArray)
{
    CFX_WideString ws = wsValue;
    CFX_WideString wsStr;
    FX_STRSIZE star = 0, sepa = 1;
    FX_INT32 i = 0, j = 0;
    FX_BOOL flag = TRUE;
    i = FilterSpace(ws, i);
    for(; i < ws.GetLength(); i++) {
        FX_WCHAR wchar = ws.GetAt(i);
        if (flag) {
            FX_INT32 ret = SeparateDoubleQuotation(i, ws, wsArray);
            if (ret == 0) {
                return;
            }
            if (ret == 1) {
                star = FilterSpace(ws, i + 1);
                i = star - 1;
                continue;
            }
        }
        for (j = 0; j < sizeof(keywordsSeparator) / sizeof(keywordsSeparator[0]); j++) {
            if (wchar == keywordsSeparator[j]) {
                if (wchar == 0x0D) {
                    if (ws.GetAt(i + 1) == 0x0A) {
                        sepa = 2;
                    } else {
                        continue;
                    }
                }
                wsStr = ws.Mid(star, i - star);
                if (wsStr.GetLength() != 0) {
                    wsArray.Add(wsStr);
                }
                star = FilterSpace(ws, i + sepa);
                i = star - 1;
                flag = TRUE;
                break;
            }
        }
        if (j >= sizeof(keywordsSeparator) / sizeof(keywordsSeparator[0])) {
            flag = FALSE;
        }
    }
    wsStr = ws.Mid(star, i - star);
    if (wsStr.GetLength() != 0) {
        wsArray.Add(wsStr);
    }
}
FX_BOOL CPDF_KeyValueStringArray::CompareKeywords(const CFX_WideStringArray& wsArray, const CFX_WideString& wsValue)
{
    CFX_WideStringArray wsValueArray;
    GetKeyWordsArray(wsValue, wsValueArray);
    if (wsArray.GetSize() == wsValueArray.GetSize()) {
        for (FX_INT32 i = 0; i < wsArray.GetSize(); i++) {
            CFX_WideString wsKey = *wsArray.GetDataPtr(i);
            FX_INT32 j = 0;
            for (; j < wsValueArray.GetSize(); j++) {
                if (wsKey == *wsValueArray.GetDataPtr(j)) {
                    break;
                }
            }
            if (j >= wsValueArray.GetSize()) {
                return FALSE;
            }
        }
        return TRUE;
    }
    return FALSE;
}
CFX_WideString CPDF_KeyValueStringArray::GetFirst(const CFX_WideString& keyValue)
{
    CFX_WideString wsValue = keyValue;
    FX_INT32 i = 0;
    i = FilterSpace(wsValue, i);
    if (i  >= wsValue.GetLength()) {
        return L"";
    }
    CFX_WideStringArray wsArray;
    FX_INT32 ret = SeparateDoubleQuotation(i, wsValue, wsArray);
    if (ret != -1) {
        return *wsArray.GetDataPtr(0);
    }
    wsValue = wsValue.Right(wsValue.GetLength() - i);
    FX_STRSIZE posiMin = -1;
    for (FX_INT32 j  = 0; j < sizeof(authorSeperator) / sizeof(authorSeperator[0]); j++) {
        FX_STRSIZE posi = wsValue.Find(authorSeperator[j]);
        if (posi != -1) {
            if (posiMin == -1 || posiMin > posi) {
                if(authorSeperator[j] == 0x20 && (wsValue.GetLength() - 1 > posi)) {
                    if(wsValue[posi + 1] == 0x20) {
                        posiMin = posi;
                    } else {
                        continue;
                    }
                } else {
                    posiMin = posi;
                }
            }
        }
    }
    if (posiMin != -1) {
        return wsValue.Left(posiMin);
    } else {
        return wsValue;
    }
}
class CustomKeyTransfor
{
public:
    static void CustomKeyToXMPKey(const CFX_WideString& key, CFX_WideString& XMPKey);
    static void XMPKeyToCustomKey(const CFX_WideString& XMPKey, CFX_WideString& key);
protected:
    static FX_BOOL IsCustomXMPKey(FX_WCHAR wch);
    static FX_BOOL IsNameChar(FX_WCHAR wch);
    static FX_BOOL IsNameStartChar(FX_WCHAR wch);
};
FX_BOOL CustomKeyTransfor::IsNameStartChar(FX_WCHAR wch)
{
    return wch >= 'A' && wch <= 'Z' || wch == '_' || wch >= 'a' && wch <= 'z' ||
           wch >= 0xC0 && wch <= 0xD6 || wch >= 0xD8 && wch <= 0xF6 || wch >= 0xF8 && wch <= 0x2FF ||
           wch >= 0x370 && wch <= 0x37D || wch >= 0x37F && wch <= 0x1FFF || wch >= 0x200C && wch <= 0x200D ||
           wch >= 0x2070 && wch <= 0x218F || wch >= 0x2C00 && wch <= 0x2FEF || wch >= 0x3001 && wch <= 0xD7FF ||
           wch >= 0xF900 && wch <= 0xFDCF || wch >= 0xFDF0 && wch <= 0xFFFD;
}
FX_BOOL CustomKeyTransfor::IsNameChar(FX_WCHAR wch)
{
    return wch == '-' || wch == '.' || wch >= '0' && wch <= '9' || wch == 0xB7 ||
           wch >= 0x0300 && wch <= 0x036F || wch >= 0x203F && wch <= 0x2040 || IsNameStartChar(wch);
}
FX_BOOL CustomKeyTransfor::IsCustomXMPKey(FX_WCHAR wch)
{
    return !((wch >= 0 && wch <= 0xFFFD) && !IsNameChar(wch));
}
void CustomKeyTransfor::CustomKeyToXMPKey(const CFX_WideString& key, CFX_WideString& XMPKey)
{
    XMPKey = key;
    for (FX_INT32 i = 0; i < XMPKey.GetLength(); i++) {
        FX_WCHAR ch = XMPKey.GetAt(i);
        if (!IsCustomXMPKey(ch)) {
            FX_WCHAR* unicode = FX_Alloc(FX_WCHAR, 5);
            unicode[0] = 0x2182;
            for (FX_INT32 j = 0; j < 4; j++) {
                FX_BYTE b = (ch >> ((3 - j) * 4)) & 0x0f;
                unicode[j + 1] = "0123456789ABCDEF"[b % 16];
            }
            XMPKey.Delete(i, 1);
            for (FX_INT32 k = 0; k < 5; k++) {
                XMPKey.Insert(i++, unicode[k]);
            }
            FX_Free(unicode);
            i--;
        }
    }
}
void CustomKeyTransfor::XMPKeyToCustomKey(const CFX_WideString& XMPKey, CFX_WideString& key)
{
    key = XMPKey;
    for (FX_INT32 i = 0; i < key.GetLength(); i++) {
        FX_WCHAR ch = key.GetAt(i);
        if (ch == 0x2182) {
            CFX_WideString temp = key.Mid(i + 1, 4);
            FX_INT32 unicode = 0;
            for (FX_INT32 j = 0; j < 4; j++) {
                FX_WCHAR unic = temp.GetAt(j);
                if (unic >= L'0' && unic <= '9') {
                    unicode = unicode * 16 + (unic - L'0');
                } else if (unic >= L'a' && unic <= L'b') {
                    unicode = unicode * 16 + (unic - L'a') + 10;
                } else if (unic >= L'A' && unic <= L'F') {
                    unicode = unicode * 16 + (unic - L'A') + 10;
                }
            }
            FX_WCHAR ch = unicode;
            key.Delete(i, 5);
            key.Insert(i, ch);
        }
    }
}
static FX_BOOL FXCRT_CheckDateTime(const FXCRT_DATETIMEZONE *dt)
{
    if(	!dt
            ||	0 == dt->month || dt->month > 12
            ||	0 == dt->day || dt->day > 31
            ||	dt->hour > 23
            ||	dt->minute > 59
            ||	dt->second > 60
            ||	dt->tzHour < -12 || dt->tzHour > 12
            ||	dt->tzMinute > 59) {
        return FALSE;
    } else {
        return TRUE;
    }
}
static FX_INT32 _GetTimeZoneInSeconds(FX_INT16 tzhour, FX_UINT16 tzminute)
{
    return (FX_INT32)tzhour * 3600 + (FX_INT32)tzminute * (tzhour >= 0 ? 60 : -60);
}
static FX_BOOL _IsLeapYear(FX_UINT16 year)
{
    return ((0 == year % 400) || ((0 == year % 4) && (year % 100 != 0)));
}
static FX_WORD _GetYearDays(FX_UINT16 year)
{
    return (_IsLeapYear(year) == TRUE ? 366 : 365);
}
static FX_BYTE _GetMonthDays(FX_UINT16 year, FX_UINT16 month)
{
    FX_BYTE	mDays;
    switch (month) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            mDays = 31;
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            mDays = 30;
            break;
        case 2:
            if (_IsLeapYear(year) == TRUE) {
                mDays = 29;
            } else {
                mDays = 28;
            }
            break;
        default:
            mDays = 0;
            break;
    }
    return mDays;
}
static void AddDays(FXCRT_DATETIMEZONE& dt, FX_INT32 days)
{
    if (0 == days) {
        return;
    }
    FX_UINT16 y = dt.year, yy;
    FX_UINT16 m = dt.month;
    FX_UINT16 d = dt.day;
    FX_INT32 mdays, ydays, ldays;
    ldays = days;
    if (ldays > 0) {
        yy = y;
        if (((FX_WORD)m * 100 + d) > 300) {
            yy ++;
        }
        ydays = _GetYearDays(yy);
        while (ldays >= ydays) {
            y ++;
            ldays -= ydays;
            yy ++;
            mdays = _GetMonthDays(y, m);
            if (d > mdays) {
                m ++;
                d -= mdays;
            }
            ydays = _GetYearDays(yy);
        }
        mdays = _GetMonthDays(y, m) - d + 1;
        while (ldays >= mdays) {
            ldays -= mdays;
            m ++;
            d = 1;
            mdays = _GetMonthDays(y, m);
        }
        d += ldays;
    } else {
        ldays *= -1;
        yy = y;
        if (((FX_WORD)m * 100 + d) < 300) {
            yy --;
        }
        ydays = _GetYearDays(yy);
        while (ldays >= ydays) {
            y --;
            ldays -= ydays;
            yy --;
            mdays = _GetMonthDays(y, m);
            if (d > mdays) {
                m ++;
                d -= mdays;
            }
            ydays = _GetYearDays(yy);
        }
        while (ldays >= d) {
            ldays -= d;
            m --;
            mdays = _GetMonthDays(y, m);
            d = mdays;
        }
        d -= ldays;
    }
    dt.year = y;
    dt.month = m;
    dt.day = d;
}
#define SECONDSOFONEDAY (24 * 3600)
static void AddSeconds(FXCRT_DATETIMEZONE& dt, FX_INT32 seconds)
{
    if (0 == seconds) {
        return;
    }
    int	n;
    int	days;
    n = dt.hour * 3600 + dt.minute * 60 + dt.second + seconds;
    if (n < 0) {
        days = (n - (SECONDSOFONEDAY - 1)) / SECONDSOFONEDAY;
        n -= days * SECONDSOFONEDAY;
    } else {
        days = n / SECONDSOFONEDAY;
        n %= SECONDSOFONEDAY;
    }
    dt.hour = (FX_BYTE)(n / SECONDSOFONEDAY);
    dt.hour %= 24;
    n %= 3600;
    dt.minute = (FX_BYTE)(n / 60);
    dt.second = (FX_BYTE)(n % 60);
    if (days != 0) {
        AddDays(dt, days);
    }
}
FX_INT32 FXCRT_CompareDateTime(const FXCRT_DATETIMEZONE* dateTime1, const FXCRT_DATETIMEZONE* dateTime2)
{
    if (!dateTime1 && !dateTime2) {
        return 0;
    }
    if (dateTime1 && !dateTime2) {
        return 1;
    }
    if (!dateTime1 && dateTime2) {
        return -1;
    }
    FXCRT_DATETIMEZONE dtTmp1 = *dateTime1, dtTmp2 = *dateTime2;
    AddSeconds(dtTmp1, -_GetTimeZoneInSeconds(dtTmp1.tzHour, dtTmp1.tzMinute));
    dtTmp1.tzHour = 0;
    dtTmp1.tzMinute = 0;
    AddSeconds(dtTmp2, -_GetTimeZoneInSeconds(dtTmp2.tzHour, dtTmp2.tzMinute));
    dtTmp2.tzHour = 0;
    dtTmp2.tzMinute = 0;
    FX_INT32 d1Date = (((int)dtTmp1.year) << 16) | (((int)dtTmp1.month) << 8) | (int)dtTmp1.day;
    FX_INT32 d1Time = (((int)dtTmp1.hour) << 16) | (((int)dtTmp1.minute) << 8) | (int)dtTmp1.second;
    FX_INT32 d2Date = (((int)dtTmp2.year) << 16) | (((int)dtTmp2.month) << 8) | (int)dtTmp2.day;
    FX_INT32 d2Time = (((int)dtTmp2.hour) << 16) | (((int)dtTmp2.minute) << 8) | (int)dtTmp2.second;
    if (d1Date > d2Date) {
        return 1;
    } else if (d1Date < d2Date) {
        return -1;
    } else {
        if (d1Time > d2Time) {
            return 1;
        } else if (d1Time < d2Time) {
            return -1;
        }
    }
    return 0;
}
CPDF_DateTime::CPDF_DateTime()
{
    FXSYS_memset(&dt, 0, sizeof(FXCRT_DATETIMEZONE));
}
CPDF_DateTime::CPDF_DateTime(const CFX_ByteString& pdfDateTimeStr)
{
    ParserPDFDateTimeString(pdfDateTimeStr);
}
CPDF_DateTime::CPDF_DateTime(const FXCRT_DATETIMEZONE& st)
    : dt(st)
{
}
#define FSCRT_CHAR_NotDigit(ch) (ch < '0' || ch > '9')
#define FSCRT_CHAR_TimeZoom(ch) (ch == '+' || ch == '-' || ch == 'Z')
FX_BOOL CPDF_DateTime::ParserPDFXMPDateTimeString(const CFX_WideString& dtStr)
{
    CFX_ByteString bsStr;
    bsStr.ConvertFrom(dtStr);
    int strLength = dtStr.GetLength();
    if (strLength < 4) {
        return FALSE;
    }
    int index = 0;
    FX_BOOL bContinue = TRUE;
    if (!ParserYear(bsStr, index, bContinue)) {
        return FALSE;
    }
    if (!bContinue) {
        return TRUE;
    }
    if (bsStr[index] == '-') {
        index++;
    }
    if (strLength < index + 2) {
        return FALSE;
    }
    if (!ParserMonth(bsStr, index, bContinue)) {
        return FALSE;
    }
    if (!bContinue) {
        return TRUE;
    }
    if (bsStr[index] == '-') {
        index++;
    }
    if (strLength < index + 2) {
        return FALSE;
    }
    if (!ParserDay(bsStr, index, bContinue)) {
        return FALSE;
    }
    if (!bContinue) {
        return TRUE;
    }
    if (bsStr[index] == 'T') {
        index++;
    }
    if (strLength < index + 2) {
        return FALSE;
    }
    if (!ParserHour(bsStr, index, bContinue)) {
        return FALSE;
    }
    if (!bContinue) {
        return TRUE;
    }
    if (bsStr[index] == ':') {
        index++;
    }
    if (strLength < index + 2) {
        return FALSE;
    }
    if (!ParserMinute(bsStr, index, bContinue)) {
        return FALSE;
    }
    if (!bContinue) {
        return TRUE;
    }
    if (bsStr[index] == ':') {
        index++;
        if (strLength < index + 2) {
            return FALSE;
        }
        if (!ParserSecond(bsStr, index, bContinue)) {
            return FALSE;
        }
        if (!bContinue) {
            return TRUE;
        }
    } else {
        dt.second = 0;
    }
    return ParserXMPTimeZone(bsStr, index);
}
FX_BOOL CPDF_DateTime::ParserPDFDateTimeString(const CFX_ByteString& dtStr)
{
    int strLength = dtStr.GetLength();
    if (strLength < 4) {
        return FALSE;
    }
    int index = 0;
    FX_BOOL bContinue = TRUE;
    bool prefix = (dtStr[0] == 'D' && dtStr[1] == ':');
    if (prefix) {
        index = 2;
        if (strLength < 6) {
            return FALSE;
        }
    }
    if (!ParserYear(dtStr, index, bContinue)) {
        return FALSE;
    }
    if (!bContinue) {
        return TRUE;
    }
    if (FSCRT_CHAR_TimeZoom(dtStr[index])) {
        return ParserTimeZone(dtStr, index);
    }
    if (strLength < index + 2) {
        return FALSE;
    }
    if (!ParserMonth(dtStr, index, bContinue)) {
        return FALSE;
    }
    if (!bContinue) {
        return TRUE;
    }
    if (FSCRT_CHAR_TimeZoom(dtStr[index])) {
        return ParserTimeZone(dtStr, index);
    }
    if (strLength < index + 2) {
        return FALSE;
    }
    if (!ParserDay(dtStr, index, bContinue)) {
        return FALSE;
    }
    if (!bContinue) {
        return TRUE;
    }
    if (FSCRT_CHAR_TimeZoom(dtStr[index])) {
        return ParserTimeZone(dtStr, index);
    }
    if (strLength < index + 2) {
        return FALSE;
    }
    if (!ParserHour(dtStr, index, bContinue)) {
        return FALSE;
    }
    if (!bContinue) {
        return TRUE;
    }
    if (FSCRT_CHAR_TimeZoom(dtStr[index])) {
        return ParserTimeZone(dtStr, index);
    }
    if (strLength < index + 2) {
        return FALSE;
    }
    if (!ParserMinute(dtStr, index, bContinue)) {
        return FALSE;
    }
    if (!bContinue) {
        return TRUE;
    }
    if (FSCRT_CHAR_TimeZoom(dtStr[index])) {
        return ParserTimeZone(dtStr, index);
    }
    if (strLength < index + 2) {
        return FALSE;
    }
    if (!ParserSecond(dtStr, index, bContinue)) {
        return FALSE;
    }
    if (!bContinue) {
        return TRUE;
    }
    return ParserTimeZone(dtStr, index);
}
static FX_BOOL ParserDateTimeChar(const CFX_ByteString& dtStr, FX_INT32 &index, FX_BOOL &bContinue, FX_UINT16 parseCount, FX_UINT16 &ret)
{
    int i = 0;
    int strLength = dtStr.GetLength();
    CFX_ByteString sub;
    sub = dtStr.Mid(index, parseCount);
    while (i < parseCount) {
        if (FSCRT_CHAR_NotDigit(sub[i])) {
            bContinue = FALSE;
            return FALSE;
        }
        i ++;
    }
    ret = FXSYS_atoi(sub);
    index += parseCount;
    if (strLength == index) {
        bContinue = FALSE;
    }
    return TRUE;
}
inline FX_BOOL CPDF_DateTime::ParserYear(const CFX_ByteString& dtStr, FX_INT32 &index, FX_BOOL &bContinue)
{
    return ParserDateTimeChar(dtStr, index, bContinue, 4, dt.year);
}
inline FX_BOOL CPDF_DateTime::ParserMonth(const CFX_ByteString& dtStr, FX_INT32 &index, FX_BOOL &bContinue)
{
    return ParserDateTimeChar(dtStr, index, bContinue, 2, dt.month);
}
inline FX_BOOL CPDF_DateTime::ParserDay(const CFX_ByteString& dtStr, FX_INT32 &index, FX_BOOL &bContinue)
{
    return ParserDateTimeChar(dtStr, index, bContinue, 2, dt.day);
}
inline FX_BOOL CPDF_DateTime::ParserHour(const CFX_ByteString& dtStr, FX_INT32 &index, FX_BOOL &bContinue)
{
    return ParserDateTimeChar(dtStr, index, bContinue, 2, dt.hour);
}
inline FX_BOOL CPDF_DateTime::ParserMinute(const CFX_ByteString& dtStr, FX_INT32 &index, FX_BOOL &bContinue)
{
    return ParserDateTimeChar(dtStr, index, bContinue, 2, dt.minute);
}
inline FX_BOOL CPDF_DateTime::ParserSecond(const CFX_ByteString& dtStr, FX_INT32 &index, FX_BOOL &bContinue)
{
    return ParserDateTimeChar(dtStr, index, bContinue, 2, dt.second);
}
inline FX_BOOL CPDF_DateTime::ParserTimeZone(const CFX_ByteString& dtStr, FX_INT32 &index)
{
    CFX_ByteString sub;
    sub = dtStr.Mid(index, dtStr.GetLength() - index);
    if (!FSCRT_CHAR_TimeZoom(sub[0])) {
        return FALSE;
    }
    if (sub[0] == 'Z') {
        return TRUE;
    }
    int subStrLength = sub.GetLength();
    if (subStrLength == 1) {
        return TRUE;
    }
    if (subStrLength != 7 && subStrLength != 5 && subStrLength != 3) {
        return FALSE;
    }
    index = 1;
    if (sub[index] == '\'') {
        index ++;
        FX_BOOL bContinue = TRUE;
        if (!ParserDateTimeChar(sub, index, bContinue, 2, *(FX_UINT16*)&dt.tzMinute)) {
            return FALSE;
        }
        if (sub[index] != '\'') {
            return FALSE;
        }
        index ++;
    } else {
        FX_BOOL bContinue = TRUE;
        if (!ParserDateTimeChar(sub, index, bContinue, 2, *(FX_UINT16*)&dt.tzHour)) {
            return FALSE;
        }
        if (sub[index] == '-') {
            dt.tzHour = -dt.tzHour;
        }
        if (!bContinue) {
            return TRUE;
        }
        if (sub[index] != '\'') {
            return FALSE;
        }
        index ++;
        if (!ParserDateTimeChar(sub, index, bContinue, 2, *(FX_UINT16*)&dt.tzMinute)) {
            return FALSE;
        }
        if (sub[index] != '\'') {
            return FALSE;
        }
        index ++;
    }
    return index == subStrLength;
}
inline FX_BOOL CPDF_DateTime::ParserXMPTimeZone(const CFX_ByteString& dtStr, FX_INT32 &index)
{
    CFX_ByteString sub;
    sub = dtStr.Mid(index, dtStr.GetLength() - index);
    if (!FSCRT_CHAR_TimeZoom(sub[0])) {
        return FALSE;
    }
    if (sub[0] == 'Z') {
        return TRUE;
    }
    int subStrLength = sub.GetLength();
    if (subStrLength == 1) {
        return TRUE;
    }
    if (subStrLength != 6 && subStrLength != 5 && subStrLength != 3) {
        return FALSE;
    }
    index = 1;
    if (sub[index] == ':') {
        index ++;
        FX_BOOL bContinue = TRUE;
        if (!ParserDateTimeChar(sub, index, bContinue, 2, *(FX_UINT16*)&dt.tzMinute)) {
            return FALSE;
        }
        index ++;
    } else {
        FX_BOOL bContinue = TRUE;
        if (!ParserDateTimeChar(sub, index, bContinue, 2, *(FX_UINT16*)&dt.tzHour)) {
            return FALSE;
        }
        if (sub[0] == '-') {
            dt.tzHour = -dt.tzHour;
        }
        if (!bContinue) {
            return TRUE;
        }
        if (sub[index] != ':') {
            return FALSE;
        }
        index++;
        if (!ParserDateTimeChar(sub, index, bContinue, 2, *(FX_UINT16*)&dt.tzMinute)) {
            return FALSE;
        }
    }
    return index == subStrLength;
}
CFX_ByteString CPDF_DateTime::ToPDFDateTimeString()
{
    CFX_ByteString dtStr;
    CFX_ByteString tempStr;
    tempStr.Format("D:%04d%02d%02d%02d%02d%02d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
    dtStr = CFX_ByteString(tempStr);
    if (dt.tzHour < 0) {
        dtStr += CFX_ByteString("-");
    } else {
        dtStr += CFX_ByteString("+");
    }
    tempStr = "";
    tempStr.Format("%02d'%02d'", FXSYS_abs(dt.tzHour), dt.tzMinute);
    dtStr += CFX_ByteString(tempStr);
    return dtStr;
}
CFX_WideString CPDF_DateTime::ToXMPDateTimeString()
{
    CFX_WideString dtStr;
    CFX_ByteString tempStr;
    tempStr.Format("%04d-%02d-%02dT%02d:%02d:%02d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
    dtStr.ConvertFrom(tempStr);
    if (dt.tzHour < 0) {
        dtStr += FX_WSTRC(L"-");
    } else {
        dtStr += FX_WSTRC(L"+");
    }
    tempStr = FX_BSTRC("");
    tempStr.Format("%02d:%02d", FXSYS_abs(dt.tzHour), dt.tzMinute);
    CFX_WideString dtStrTmp;
    dtStrTmp.ConvertFrom(tempStr);
    dtStr += dtStrTmp;
    return dtStr;
}
void CPDF_DateTime::SetCurrentSystemDateTime()
{
    FX_Time_GetSystemTime((FX_SYSTEMTIME*)&dt);
}
typedef struct _PDFDOC_METADATA {
    CPDF_Document *m_pDoc;
    CXML_Element *m_pXmlElmnt;
    CXML_Element *m_pElmntRdf;
} PDFDOC_METADATA, * PDFDOC_LPMETADATA;
CPDF_Metadata::CPDF_Metadata()
{
    m_pData = FX_Alloc(PDFDOC_METADATA, 1);
    FXSYS_memset32(m_pData, 0, sizeof(PDFDOC_METADATA));
    m_bRDFParsed = FALSE;
}
CPDF_Metadata::~CPDF_Metadata()
{
    FXSYS_assert(m_pData != NULL);
    CXML_Element *&p = ((PDFDOC_LPMETADATA)m_pData)->m_pXmlElmnt;
    if (p) {
        delete p;
    }
    FX_Free(m_pData);
    for (int i = 0; i < m_XMLValueArray.GetSize(); i++) {
        CPDF_MetadataRDFValue* pRDFValue = (CPDF_MetadataRDFValue*)(m_XMLValueArray[i]);
        if (pRDFValue) {
            delete pRDFValue;
        }
    }
    m_XMLValueArray.RemoveAll();
}
FX_BOOL CPDF_Metadata::LoadDoc(CPDF_Document *pDoc, FX_BOOL bKeepSpecialCode)
{
    FXSYS_assert(pDoc != NULL);
    ((PDFDOC_LPMETADATA)m_pData)->m_pDoc = pDoc;
    CPDF_Dictionary *pRoot = pDoc->GetRoot();
    CPDF_Stream *pStream = pRoot->GetStream(FX_BSTRC("Metadata"));
    if (!pStream) {
        return FALSE;
    }
    CPDF_StreamAcc acc;
    if(acc.LoadAllData(pStream, FALSE) == FALSE) {
        return FALSE;
    }
    FX_FILESIZE size = acc.GetSize();
    FX_LPCBYTE pBuf = acc.GetData();
    CXML_Element *&pXmlElmnt = ((PDFDOC_LPMETADATA)m_pData)->m_pXmlElmnt;
    pXmlElmnt = CXML_Element::Parse(pBuf, size, bKeepSpecialCode, NULL, bKeepSpecialCode);
    if (!pXmlElmnt) {
        return FALSE;
    }
    CXML_Element *&pElmntRdf = ((PDFDOC_LPMETADATA)m_pData)->m_pElmntRdf;
    if (pXmlElmnt->GetTagName() == FX_BSTRC("RDF")) {
        pElmntRdf = pXmlElmnt;
    } else {
        pElmntRdf = pXmlElmnt->GetElement(NULL, FX_BSTRC("RDF"));
    }
    return TRUE;
}
FX_BOOL	CPDF_Metadata::LoadStream(CPDF_Stream *pStream, FX_BOOL bKeepSpecialCode) const
{
    if(!pStream) {
        return FALSE;
    }
    CPDF_StreamAcc acc;
    if(acc.LoadAllData(pStream, FALSE) == FALSE) {
        return FALSE;
    }
    FX_FILESIZE size = acc.GetSize();
    FX_LPCBYTE pBuf = acc.GetData();
    CXML_Element *&pXmlElmnt = ((PDFDOC_LPMETADATA)m_pData)->m_pXmlElmnt;
    pXmlElmnt = CXML_Element::Parse(pBuf, size, bKeepSpecialCode, NULL, bKeepSpecialCode);
    if (!pXmlElmnt) {
        return FALSE;
    }
    CXML_Element *&pElmntRdf = ((PDFDOC_LPMETADATA)m_pData)->m_pElmntRdf;
    if (pXmlElmnt->GetTagName() == FX_BSTRC("RDF")) {
        pElmntRdf = pXmlElmnt;
    } else {
        pElmntRdf = pXmlElmnt->GetElement(NULL, FX_BSTRC("RDF"));
    }
    return TRUE;
}
FX_INT32 CPDF_Metadata::GetAllCustomKeys(CFX_WideStringArray& keys) const
{
    CPDF_Dictionary* pInfo = ((PDFDOC_LPMETADATA)m_pData)->m_pDoc->GetInfo();
    if (!pInfo) {
        return -1;
    }
    CFX_ByteString bsKey;
    FX_POSITION pos = pInfo->GetStartPos();
    while (pos) {
        pInfo->GetNextElement(pos, bsKey);
        CPDF_Object* pObj = pInfo->GetElement(bsKey);
        if (pObj) {
            FX_INT32 type = pObj->GetType();
            if(type != PDFOBJ_STRING) {
                continue;
            }
        }
        FX_INT32 i = 0;
        for (; i < sizeof(bsKeyType) / sizeof(bsKeyType[0]); i++) {
            if (CFX_ByteString(bsKeyType[i]) == bsKey || bsKey == "CreationDate--Text") {
                break;
            }
        }
        if (i >= sizeof(bsKeyType) / sizeof(bsKeyType[0])) {
            keys.Add(bsKey.UTF8Decode());
        }
    }
    return keys.GetSize();
}
FX_BOOL CPDF_Metadata::DeleteCustomKey(FX_WSTR wsItem)
{
    if (wsItem.GetLength() == 0) {
        return TRUE;
    }
    CFX_ByteString bsItem = CFX_WideString(wsItem).UTF8Encode();
    for (FX_INT32 i = 0; i < sizeof(bsKeyType) / sizeof(bsKeyType[0]); i++) {
        if (bsItem == CFX_ByteString(bsKeyType[i])) {
            return FALSE;
        }
    }
    CPDF_Dictionary* pInfo = ((PDFDOC_LPMETADATA)m_pData)->m_pDoc->GetInfo();
    if (pInfo) {
        pInfo->RemoveAt(bsItem);
    }
    CFX_WideString XMPKey;
    CustomKeyTransfor::CustomKeyToXMPKey(wsItem, XMPKey);
    bsItem = XMPKey.UTF8Encode();
    CXML_Element* pXMLRDF = GetRDF();
    if (pXMLRDF) {
        FX_BSTR space = "rdf";
        FX_BSTR tagDescription = "Description";
        FX_DWORD dwCount = pXMLRDF->CountElements(space, tagDescription);
        for (FX_DWORD i = 0; i < dwCount; i++) {
            CXML_Element* pXMLElement = pXMLRDF->GetElement(space, tagDescription, i);
            if (!pXMLElement || !pXMLElement->HasAttr(FX_BSTRC("xmlns:pdfx"))) {
                continue;
            }
            FX_DWORD dw = pXMLElement->CountChildren();
            for (FX_DWORD j = 0; j < dw; j++) {
                CXML_Element* pEle = pXMLElement->GetElement(j);
                if (!pEle) {
                    continue;
                }
                CFX_ByteString bsTag = pEle->GetTagName();
                if (bsTag == bsItem) {
                    pXMLElement->RemoveChild(j);
                    break;
                }
            }
            break;
        }
    }
    return TRUE;
}
FX_INT32 CPDF_Metadata::GetStandardMetadataContent(FX_BSTR key, CFX_WideString &keyValue, FX_INT32& bUseInfoOrXML) const
{
    FX_INT32 ret = CompareModDT();
    CFX_WideStringArray wsArray;
    if (ret == CPDF_METADATATYPE_XML) {
        FX_INT32 bUsePDFOrDC = CPDF_METADATA_KEYWORDS_ALL;
        ret = GetStringFromXML(key, wsArray, bUsePDFOrDC);
        if (ret == 0) {
            bUseInfoOrXML = CPDF_METADATATYPE_XML;
            FX_BOOL bKeypdf = bUsePDFOrDC == CPDF_METADATA_KEYWORDS_PDF ? FALSE :  TRUE;
            CPDF_KeyValueStringArray::WideStringArrayToWideString(key, wsArray, keyValue, bKeypdf);
            return ret;
        }
        if(key != FX_BSTRC("Title") && key != FX_BSTRC("Subject") && key != FX_BSTRC("Producer")) {
            bUseInfoOrXML = CPDF_METADATATYPE_XML;
            keyValue = L"";
            return ret;
        }
        if (ret == FX_ERR_CODE_METADATA_NOTFOUND_KEY || ret == FX_ERR_CODE_METADATA_NOTFOUND_KEYVALUE) {
            bUseInfoOrXML = CPDF_METADATATYPE_INFO;
            return GetStringFromInfo(key, keyValue);
        }
    } else {
        CFX_WideString infoWstr;
        ret = GetStringFromInfo(key, infoWstr);
        FX_INT32 bUsePDFOrDC = CPDF_METADATA_KEYWORDS_ALL;
        if (ret == FX_ERR_CODE_METADATA_NOTFOUND_KEY) {
            bUseInfoOrXML = CPDF_METADATATYPE_XML;
            ret = GetStringFromXML(key, wsArray, bUsePDFOrDC);
            FX_BOOL bKeypdf = bUsePDFOrDC == CPDF_METADATA_KEYWORDS_PDF ? FALSE : TRUE;
            CPDF_KeyValueStringArray::WideStringArrayToWideString(key, wsArray, keyValue, bKeypdf);
            return ret;
        }
        bUseInfoOrXML = CPDF_METADATATYPE_INFO;
        keyValue = infoWstr;
        if (key == FX_BSTRC("Author") || key == FX_BSTRC("Keywords")) {
            bUsePDFOrDC = CPDF_METADATA_KEYWORDS_DC;
            ret = GetStringFromXML(key, wsArray, bUsePDFOrDC);
            if (ret != 0) {
                wsArray.Add(infoWstr);
                CPDF_KeyValueStringArray::WideStringArrayToWideString(key, wsArray, keyValue);
                return 0;
            }
            if (key == FX_BSTRC("Author")) {
                wsArray[0] = infoWstr;
                CPDF_KeyValueStringArray::WideStringArrayToWideString(key, wsArray, keyValue);
                return ret;
            } else {
                FX_BOOL bRet = CPDF_KeyValueStringArray::CompareKeywords(wsArray, infoWstr);
                if (bRet) {
                    keyValue = infoWstr;
                } else {
                    CFX_WideStringArray wsArrayTemp;
                    wsArrayTemp.Add(infoWstr);
                    wsArrayTemp.Append(wsArray);
                    CPDF_KeyValueStringArray::WideStringArrayToWideString(key, wsArrayTemp, keyValue);
                }
            }
        }
    }
    return ret;
}
FX_INT32 CPDF_Metadata::ParseRDF()
{
    if (m_bRDFParsed) {
        return 0;
    }
    CXML_Element* pXMLRDF = GetRDF();
    if (!pXMLRDF) {
        return -1;
    }
    FX_BSTR space = "rdf";
    FX_BSTR tagDescription = "Description";
    int dwCount = pXMLRDF->CountElements(space, tagDescription);
    for (int i = 0; i < dwCount; i++) {
        CXML_Element* pDesElement = pXMLRDF->GetElement(space, tagDescription, i);
        if (!pDesElement) {
            continue;
        }
        FX_INT32 nAttrs = pDesElement->CountAttrs();
        FX_UNREFERENCED_PARAMETER(nAttrs);
        FX_INT32 nChildren = pDesElement->CountChildren();
        for (int j = 0; j < nChildren; j++) {
            CXML_Element* pXMLElement = pDesElement->GetElement(j);
            if (!pXMLElement) {
                continue;
            }
            CFX_ByteString space = pXMLElement->GetNamespace(TRUE);
            CFX_ByteString name = pXMLElement->GetTagName();
            if (space.IsEmpty() || name.IsEmpty()) {
                continue;
            }
            CFX_WideString wsValue = pXMLElement->GetContent(0);
            if (wsValue.IsEmpty()) {
                CFX_WideStringArray keyArray;
                GetXMPArrayValue(pXMLElement, keyArray);
                CPDF_KeyValueStringArray::WideStringArrayToWideString(name, keyArray, wsValue);
            }
            CPDF_MetadataRDFValue* pRDFValue = FX_NEW CPDF_MetadataRDFValue;
            pRDFValue->bsNameSpace = space;
            pRDFValue->bsName = name;
            pRDFValue->wsContent = wsValue;
            m_XMLValueArray.Add((void*)pRDFValue);
        }
    }
    m_bRDFParsed = TRUE;
    return 0;
}
FX_INT32 CPDF_Metadata::GetAllRdfXMLKeys(CFX_WideStringArray& keys)
{
    if (!m_bRDFParsed) {
        ParseRDF();
    }
    FX_INT32 nKeys = 0;
    for (int i = 0; i < m_XMLValueArray.GetSize(); i++) {
        CPDF_MetadataRDFValue* pRDFValue = (CPDF_MetadataRDFValue*)(m_XMLValueArray[i]);
        if (!pRDFValue) {
            continue;
        }
        CFX_ByteString bsKey = pRDFValue->bsNameSpace + ":" + pRDFValue->bsName;
        CFX_WideString wsKey = bsKey.UTF8Decode();
        keys.Add(wsKey);
        nKeys++;
    }
    return nKeys;
}
FX_INT32 CPDF_Metadata::GetRdfXMLString(FX_WSTR wsNamespace, FX_WSTR wsKey, CFX_WideString &wsStr)
{
    if (wsNamespace.IsEmpty() || wsKey.IsEmpty()) {
        return FX_ERR_CODE_METADATA_NOTFOUND_KEY;
    }
    CFX_ByteString bsNamespace = CFX_WideString(wsNamespace).UTF8Encode();
    CFX_ByteString bsKey = CFX_WideString(wsKey).UTF8Encode();
    if(!m_bRDFParsed) {
        ParseRDF();
    }
    for (int i = m_XMLValueArray.GetSize() - 1; i >= 0; i--) {
        CPDF_MetadataRDFValue* pRDFValue = (CPDF_MetadataRDFValue*)(m_XMLValueArray[i]);
        if (!pRDFValue) {
            continue;
        }
        if (pRDFValue->bsNameSpace.Equal(bsNamespace) && pRDFValue->bsName.Equal(bsKey)) {
            wsStr = pRDFValue->wsContent;
            return 0;
        }
    }
    return FX_ERR_CODE_METADATA_NOTFOUND_KEY;
}
FX_INT32 CPDF_Metadata::GetString(FX_WSTR wsItem, CFX_WideString &wsStr, FX_INT32& bUseInfoOrXML) const
{
    if (wsItem.GetLength() == 0) {
        return FX_ERR_CODE_METADATA_NOTFOUND_KEY;
    }
    CFX_ByteString bsItem = CFX_WideString(wsItem).UTF8Encode();
    if (bsItem == FX_BSTRC("pdfaid")) {
        bUseInfoOrXML = CPDF_METADATATYPE_XML;
        CFX_WideStringArray wsArray;
        FX_INT32 bUsePDFOrDC = CPDF_METADATA_KEYWORDS_DC;
        FX_INT32 ret = GetStringFromXML(bsItem, wsArray, bUsePDFOrDC);
        CPDF_KeyValueStringArray::WideStringArrayToWideString(bsItem, wsArray, wsStr);
        return ret;
    }
    if (bsItem != FX_BSTRC("Title") && bsItem != FX_BSTRC("Author")
            && bsItem != FX_BSTRC("Subject") && bsItem != FX_BSTRC("Creator")
            && bsItem != FX_BSTRC("Producer") && bsItem != FX_BSTRC("Trapped")
            && bsItem != FX_BSTRC("Keywords") && bsItem != FX_BSTRC("CreationDate")
            && bsItem != FX_BSTRC("ModDate")) {
        bUseInfoOrXML = CPDF_METADATATYPE_INFO;
        return GetStringFromInfo(bsItem, wsStr);
    }
    return GetStandardMetadataContent(bsItem, wsStr, bUseInfoOrXML);
}
FX_INT32 CPDF_Metadata::GetStringArray(FX_WSTR wsItem, CFX_WideStringArray &wsStrArray, FX_INT32& bUseInfoOrXML) const
{
    CFX_WideString wsWstr;
    FX_INT32 ret = GetString(wsItem, wsWstr, bUseInfoOrXML);
    if (0 != ret) {
        return ret;
    }
    if (wsItem == FX_WSTRC(L"Author")) {
        CPDF_KeyValueStringArray::GetAuthorArray(wsWstr, wsStrArray, TRUE);
    } else if (wsItem == FX_WSTRC(L"Keywords")) {
        CPDF_KeyValueStringArray::GetKeyWordsArray(wsWstr, wsStrArray);
    } else {
        wsStrArray.Add(wsWstr);
    }
    return ret;
}
CXML_Element* CPDF_Metadata::GetRoot() const
{
    return ((PDFDOC_LPMETADATA)m_pData)->m_pXmlElmnt;
}
CXML_Element* CPDF_Metadata::GetRDF() const
{
    return ((PDFDOC_LPMETADATA)m_pData)->m_pElmntRdf;
}
FX_BOOL	CPDF_Metadata::CreateNewMetadata()
{
    CPDF_Document* pDoc = ((PDFDOC_LPMETADATA)m_pData)->m_pDoc;
    if (!pDoc) {
        return FALSE;
    }
    CPDF_Dictionary *pRoot = pDoc->GetRoot();
    if (!pRoot) {
        return FALSE;
    }
    CPDF_Stream *pXMLStream = pRoot->GetStream(FX_BSTRC("Metadata"));
    if (pXMLStream) {
        return FALSE;
    }
    pXMLStream = FX_NEW CPDF_Stream(NULL, 0, NULL);
    CPDF_Dictionary* pXMLDict = CPDF_Dictionary::Create();
    pXMLDict->SetAtName("Type", "Metadata");
    pXMLDict->SetAtName("Subtype", "XML");
    pXMLStream->InitStream(NULL, 0, pXMLDict);
    FX_INT32 objnum = pDoc->AddIndirectObject(pXMLStream);
    pRoot->SetAtReference("Metadata", pDoc, objnum);
    CXML_Element metadataElem("x", "xmpmeta");
    metadataElem.SetAttrValue("xmlns:x", FX_WSTRC(L"adobe:ns:meta/"));
    metadataElem.SetAttrValue("x:xmptk", FX_WSTRC(L"Adobe XMP Core 5.2-c001 63.139439, 2010/09/27-13:37:26        "));
    CXML_Element* rdfElem = FX_NEW CXML_Element(FX_BSTRC("rdf"), FX_BSTRC("RDF"));
    rdfElem->SetAttrValue(FX_BSTRC("xmlns:rdf"), FX_WSTRC(L"http://www.w3.org/1999/02/22-rdf-syntax-ns#"));
    metadataElem.AddChildElement(rdfElem);
    CFX_ByteString bstrXMLStream = metadataElem.OutputStream();
    CFX_ByteString beginXML = FX_BSTR("<?xpacket begin=\"\xEF\xBB\xBF\" id=\"W5M0MpCehiHzreSzNTczkc9d\"?>\n");
    CFX_ByteString endXML = FX_BSTR("\n<?xpacket end=\"w\"?>");
    bstrXMLStream = beginXML + bstrXMLStream + endXML;
    pXMLStream->SetData(bstrXMLStream, bstrXMLStream.GetLength(), FALSE, FALSE);
    LoadDoc(pDoc);
    return TRUE;
}
FX_BOOL	CPDF_Metadata::SetDateTime(FX_WSTR wsItem, const FXCRT_DATETIMEZONE& dt)
{
    if(wsItem != FX_WSTRC(L"CreationDate") && wsItem != FX_WSTRC(L"ModDate")) {
        return FALSE;
    }
    if(!FXCRT_CheckDateTime(&dt)) {
        return FALSE;
    }
    CFX_ByteString bsItem = CFX_WideString(wsItem).UTF8Encode();
    CPDF_DateTime tdata(dt);
    CFX_ByteString dtStr = tdata.ToPDFDateTimeString();
    CFX_WideString wsDTStr = tdata.ToXMPDateTimeString();
    FX_BOOL bRet = SetMetadataStrArrayToInfo(bsItem, dtStr.UTF8Decode());
    if (!bRet) {
        return FALSE;
    }
    return SetXMPOrPDFOrPDFXMetadataStrArrayToXML(bsItem, wsDTStr);
}
FX_INT32 CPDF_Metadata::GetDateTime(FX_WSTR wsItem, FXCRT_DATETIMEZONE& dt, FX_INT32& bUseInfoOrXML) const
{
    if (wsItem != FX_WSTRC(L"CreationDate") && wsItem != FX_WSTRC(L"ModDate")) {
        return -1;
    }
    CFX_ByteString bsItem = CFX_WideString(wsItem).UTF8Encode();
    CFX_WideString infoDt;
    FX_INT32 retInfo = GetStringFromInfo(bsItem, infoDt);
    CFX_WideStringArray metadataDtArray;
    FX_INT32 retXML = GetXMPOrPDFOrPDFXMetadataStringFromXML(bsItem, metadataDtArray);
    if (retInfo == retXML && retInfo == FX_ERR_CODE_METADATA_NOTFOUND_KEY) {
        return FX_ERR_CODE_METADATA_NOTFOUND_KEY;
    }
    if (retInfo == retXML && retInfo != 0) {
        return FX_ERR_CODE_METADATA_NOTFOUND_KEYVALUE;
    }
    CFX_WideString metadataDt;
    if (metadataDtArray.GetSize() != 0) {
        metadataDt = *metadataDtArray.GetDataPtr(0);
    }
    CPDF_DateTime dtInfo;
    CPDF_DateTime dtMetadata;
    if (infoDt.GetLength() != 0) {
        dtInfo.ParserPDFDateTimeString(infoDt.UTF8Encode());
    }
    if (metadataDt.GetLength() != 0) {
        dtMetadata.ParserPDFXMPDateTimeString(metadataDt);
    }
    if (infoDt.GetLength() == 0) {
        bUseInfoOrXML = CPDF_METADATATYPE_XML;
    } else if (metadataDt.GetLength() == 0) {
        bUseInfoOrXML = CPDF_METADATATYPE_INFO;
    } else {
        bUseInfoOrXML = CompareModDT();
    }
    if (bUseInfoOrXML == CPDF_METADATATYPE_XML) {
        dt = dtMetadata.dt;
    } else {
        dt = dtInfo.dt;
    }
    return 0;
}
FX_BOOL	CPDF_Metadata::SyncUpdateInfoFromXML()
{
    FX_INT32 ret = -1;
    CFX_WideStringArray wsArray;
    CFX_WideString wsStr;
    for (FX_INT32 i = 0; i < sizeof(bsKeyType) / sizeof (bsKeyType[0]) - 1; i++) {
        wsArray.RemoveAll();
        if (CFX_ByteString(bsKeyType[i]) != "CreationDate") {
            FX_INT32 bUsePDFOrDC = CPDF_METADATA_KEYWORDS_PDF;
            ret = GetStringFromXML(bsKeyType[i], wsArray, bUsePDFOrDC);
        } else {
            ret = GetXMPOrPDFOrPDFXMetadataStringFromXML(bsKeyType[i], wsArray);
        }
        if (ret == FX_ERR_CODE_METADATA_NOTFOUND_KEY) {
            continue;
        }
        if ((CFX_ByteString(bsKeyType[i]) != "Keywords" && CFX_ByteString(bsKeyType[i]) != "Producer" && CFX_ByteString(bsKeyType[i]) != "CreationDate")) {
            CPDF_KeyValueStringArray::WideStringArrayToWideString(bsKeyType[i], wsArray, wsStr);
            ret = SetMetadataStrArrayToInfo(bsKeyType[i], wsStr);
            if (!ret) {
                return FALSE;
            }
        } else if (ret != FX_ERR_CODE_METADATA_NOTFOUND_KEYVALUE) {
            CPDF_KeyValueStringArray::WideStringArrayToWideString(bsKeyType[i], wsArray, wsStr, FALSE);
            if (CFX_ByteString(bsKeyType[i]) == "CreationDate") {
                CPDF_DateTime dt;
                dt.ParserPDFXMPDateTimeString(wsStr);
                wsStr = dt.ToPDFDateTimeString().UTF8Decode();
            }
            ret = SetMetadataStrArrayToInfo(bsKeyType[i], wsStr);
            if (!ret) {
                return FALSE;
            }
        }
    }
    SyncUpdateCustomMetadataInfoFromXML();
    return TRUE;
}
void CPDF_Metadata::SyncUpdateCustomMetadataInfoFromXML(FX_BOOL bInfo)
{
    CFX_WideStringArray wsArray;
    FX_INT32 ret = GetXMPOrPDFOrPDFXMetadataStringFromXML(FX_BSTRC("CreationDate"), wsArray);
    if (ret == 0) {
        CXML_Element* pXMLRDF = GetRDF();
        if (!pXMLRDF) {
            return;
        }
        FX_BSTR space = "rdf";
        FX_BSTR tagDescription = "Description";
        FX_DWORD dwCount = pXMLRDF->CountElements(space, tagDescription);
        for (FX_DWORD i = 0; i < dwCount; i++) {
            CXML_Element* pXMLElement = pXMLRDF->GetElement(space, tagDescription, i);
            if (!pXMLElement || !pXMLElement->HasAttr(FX_BSTRC("xmlns:pdfx"))) {
                continue;
            }
            FX_DWORD dw = pXMLElement->CountChildren();
            for (FX_DWORD j = 0; j < dw; j++) {
                CXML_Element* pEle = pXMLElement->GetElement(j);
                if (!pEle) {
                    continue;
                }
                CFX_ByteString bsTag = pEle->GetTagName();
                if (!pXMLElement->GetElement("pdfx", bsTag)) {
                    continue;
                }
                CFX_WideString key;
                CustomKeyTransfor::XMPKeyToCustomKey(bsTag.UTF8Decode(), key);
                bsTag = key.UTF8Encode();
                CFX_WideString wsValue = pEle->GetContent(0);
                if (bInfo) {
                    CFX_WideString keyValue;
                    FX_INT32 r = GetStringFromInfo(bsTag, keyValue);
                    if (r != FX_ERR_CODE_METADATA_NOTFOUND_KEY) {
                        continue;
                    }
                }
                SetMetadataStrArrayToInfo(bsTag, wsValue);
            }
            break;
        }
    }
}
void CPDF_Metadata::AddCreationDateText()
{
    CXML_Element* pXMLRDF = GetRDF();
    FX_BSTR space = "rdf";
    FX_BSTR tagDescription = "Description";
    FX_DWORD dwCount = pXMLRDF->CountElements(space, tagDescription);
    for (FX_DWORD i = 0; i < dwCount; i++) {
        CXML_Element* pXMLElement = pXMLRDF->GetElement(space, tagDescription, i);
        if (!pXMLElement || !pXMLElement->HasAttr(FX_BSTRC("xmlns:pdf"))) {
            continue;
        }
        CXML_Element* pXMPXMLElem = FX_NEW CXML_Element(FX_BSTRC("pdf"), FX_BSTRC("CreationDate--Text"));
        pXMPXMLElem->AddChildContent(FX_WSTRC(L""));
        pXMLElement->AddChildElement(pXMPXMLElem);
        break;
    }
    SetMetadataStrArrayToInfo(FX_BSTRC("CreationDate--Text"), FX_WSTRC(L""));
}
FX_BOOL	CPDF_Metadata::SyncUpdateXMLFromInfo()
{
    CFX_WideString wsStr;
    CFX_WideStringArray wsArray;
    for (FX_INT32 i = 0; i < sizeof(bsKeyType) / sizeof (bsKeyType[0]) - 1; i++) {
        wsArray.RemoveAll();
        FX_INT32 ret = GetStringFromInfo(bsKeyType[i], wsStr);
        if (ret == 0) {
            FX_BOOL bRet;
            if (CFX_ByteString(bsKeyType[i]) == "CreationDate") {
                CPDF_DateTime dt;
                dt.ParserPDFDateTimeString(wsStr.UTF8Encode());
                wsStr = dt.ToXMPDateTimeString();
                bRet = SetXMPOrPDFOrPDFXMetadataStrArrayToXML(FX_BSTRC("CreationDate"), wsStr);
            } else {
                bRet = SetMetadataStrArrayToXML(bsKeyType[i], wsStr, TRUE, TRUE);
            }
            if (!bRet) {
                return FALSE;
            }
            continue;
        }
        if (CFX_ByteString(bsKeyType[i]) == "CreationDate") {
            if (ret == FX_ERR_CODE_METADATA_NOTFOUND_KEYVALUE) {
                AddCreationDateText();
            }
            if (ret == FX_ERR_CODE_METADATA_NOTFOUND_KEY) {
                FX_INT32 r = GetXMPOrPDFOrPDFXMetadataStringFromXML(FX_BSTRC("CreationDate"), wsArray);
                if (r == FX_ERR_CODE_METADATA_NOTFOUND_KEY) {
                    FXCRT_DATETIMEZONE dt;
                    FXCRT_GetCurrentSystemTime(dt);
                    FX_BOOL bRet = SetDateTime(FX_WSTRC(L"CreationDate"), dt);
                    if (!bRet) {
                        return FALSE;
                    }
                } else {
                    wsStr = *wsArray.GetDataPtr(0);
                    CPDF_DateTime dtMetadata;
                    if (wsStr.GetLength() != 0) {
                        dtMetadata.ParserPDFXMPDateTimeString(wsStr);
                    }
                    FXCRT_DATETIMEZONE dt;
                    dt = dtMetadata.dt;
                    CPDF_DateTime tdata(dt);
                    CFX_ByteString dtStr = tdata.ToPDFDateTimeString();
                    FX_BOOL bRet = SetMetadataStrArrayToInfo(FX_BSTRC("CreationDate"), dtStr.UTF8Decode());
                    if (!bRet) {
                        return FALSE;
                    }
                }
            }
        } else {
            if (ret != FX_ERR_CODE_METADATA_NOTFOUND_KEY) {
                FX_BOOL bRet = SetMetadataStrArrayToXML(bsKeyType[i], wsStr, TRUE, TRUE);
                if (!bRet) {
                    return FALSE;
                }
            } else {
                FX_INT32 bUsePDFOrDC = CPDF_METADATA_KEYWORDS_PDF;
                ret = GetStringFromXML(bsKeyType[i], wsArray, bUsePDFOrDC);
                if (ret == FX_ERR_CODE_METADATA_NOTFOUND_KEY) {
                    continue;
                }
                if ((CFX_ByteString(bsKeyType[i]) != "Keywords" && CFX_ByteString(bsKeyType[i]) != "Producer" && CFX_ByteString(bsKeyType[i]) != "CreationDate") || wsArray.GetSize() != 0) {
                    CPDF_KeyValueStringArray::WideStringArrayToWideString(bsKeyType[i], wsArray, wsStr, FALSE);
                    ret = SetMetadataStrArrayToInfo(bsKeyType[i], wsStr);
                    if (!ret) {
                        return FALSE;
                    }
                }
            }
        }
    }
    SyncUpdateCustomMetadataXMLFromInfo();
    return TRUE;
}
void CPDF_Metadata::SyncUpdateCustomMetadataXMLFromInfo()
{
    CPDF_Dictionary* pInfo = ((PDFDOC_LPMETADATA)m_pData)->m_pDoc->GetInfo();
    if (!pInfo) {
        return;
    }
    CFX_ByteString bsKey;
    CFX_WideString keyValue;
    FX_POSITION pos = pInfo->GetStartPos();
    while (pos) {
        pInfo->GetNextElement(pos, bsKey);
        FX_INT32 i = 0;
        for (; i < sizeof(bsKeyType) / sizeof(bsKeyType[0]); i++) {
            if (CFX_ByteString(bsKeyType[i]) == bsKey || bsKey == "CreationDate--Text") {
                break;
            }
        }
        if (i >= sizeof(bsKeyType) / sizeof(bsKeyType[0])) {
            CPDF_Object* pObj = pInfo->GetElement(bsKey);
            if (pObj) {
                FX_INT32 type = pObj->GetType();
                if(type != PDFOBJ_STRING) {
                    continue;
                }
            }
            keyValue = PDF_DecodeText(pInfo->GetString(bsKey));
            CFX_WideString XMPKey;
            CustomKeyTransfor::CustomKeyToXMPKey(bsKey.UTF8Decode(), XMPKey);
            SetXMPOrPDFOrPDFXMetadataStrArrayToXML(XMPKey.UTF8Encode(), keyValue);
        }
    }
    SyncUpdateCustomMetadataInfoFromXML(TRUE);
}
FX_BOOL CPDF_Metadata::SyncUpdateMetadata()
{
    FX_INT32 bUseInfoOrXML = CompareModDT();
    FX_BOOL ret =  FALSE;
    if (bUseInfoOrXML == CPDF_METADATATYPE_INFO) {
        ret = SyncUpdateXMLFromInfo();
    } else {
        ret = SyncUpdateInfoFromXML();
    }
    return ret;
}
FX_BOOL	CPDF_Metadata::SyncUpdate()
{
    FX_BOOL ret = SyncUpdateMetadata();
    if (!ret) {
        return ret;
    }
    FXCRT_DATETIMEZONE dt;
    FXCRT_GetCurrentSystemTime(dt);
    return SetDateTime(FX_WSTRC(L"ModDate"), dt);
}
FX_BOOL CPDF_Metadata::SetDCMetadataStrArrayToXML(FX_BSTR key, const CFX_WideString& keyValue, FX_BOOL bAdd, FX_BOOL bAuthorFirst)
{
    FX_UNREFERENCED_PARAMETER(bAdd);
    FX_BOOL ret = FALSE;
    CFX_ByteString bsXMLSpace(FX_BSTRC("dc")), bsXMLKey(FX_BSTRC("")), bsFlag(FX_BSTRC("xmlns:dc"));
    CFX_WideString wsFlagStr(FX_WSTRC(L"http://purl.org/dc/elements/1.1/"));
    if (key == "Title") {
        bsXMLKey = FX_BSTRC("title");
    } else if(key == "Author") {
        bsXMLKey = FX_BSTRC("creator");
    } else if(key == "Subject") {
        bsXMLKey = FX_BSTRC("description");
    } else if(key == "Keywords") {
        bsXMLKey = FX_BSTRC("subject");
    }
    CFX_WideString ws = keyValue;
    CFX_WideStringArray wsValue;
    if (bsXMLKey == FX_BSTRC("creator")) {
        CPDF_KeyValueStringArray::GetAuthorArray(ws, wsValue);
    } else if (bsXMLKey == FX_BSTRC("subject")) {
        CPDF_KeyValueStringArray::GetKeyWordsArray(ws, wsValue);
    } else {
        wsValue.Add(ws);
    }
    CXML_Element* pXMLRoot = GetRoot();
    if (!pXMLRoot) {
        return FALSE;
    }
    CXML_Element* pXMLRDF = GetRDF();
    if (!pXMLRDF) {
        return FALSE;
    }
    FX_BSTR space = "rdf";
    FX_BSTR tagDescription = "Description";
    FX_DWORD dwCount = pXMLRDF->CountElements(space, tagDescription);
    FX_BOOL bHasKey = FALSE, bHasDes = FALSE;
    FX_INT32 iDesIndex = 0;
    for (FX_DWORD i = 0; i < dwCount; i++) {
        CXML_Element* pXMLElement = pXMLRDF->GetElement(space, tagDescription, i);
        if(pXMLElement && pXMLElement->HasAttr(bsFlag)) {
            bHasDes = TRUE;
            iDesIndex = i;
            if (pXMLElement->HasAttr(bsXMLSpace + ":" + bsXMLKey)) {
                pXMLElement->SetAttrValue(bsXMLSpace + ":" + bsXMLKey, keyValue);
                bHasKey = TRUE;
                break;
            }
            CXML_Element* pDCXMLElement = pXMLElement->GetElement(bsXMLSpace, bsXMLKey);
            if (pDCXMLElement) {
                CFX_ByteString tag[] = {FX_BSTRC("Alt"), FX_BSTRC("Seq"), FX_BSTRC("Bag")};
                FX_BOOL bSetDone = FALSE;
                for (int i = 0; i < sizeof(tag) / sizeof(tag[0]); i++) {
                    FX_DWORD dwCountArray = pDCXMLElement->CountElements("rdf", tag[i]);
                    if (dwCountArray != 0) {
                        CXML_Element* pAltXMLElement = pDCXMLElement->GetElement("rdf", tag[i]);
                        if (!pAltXMLElement) {
                            continue;
                        }
                        if ((key == "Author") && bAuthorFirst) {
                            pAltXMLElement->RemoveChild(0);
                        } else {
                            pAltXMLElement->RemoveChildren();
                        }
                        for (FX_INT32 i = 0; i < wsValue.GetSize(); i++) {
                            CXML_Element* pAltLiElement = FX_NEW CXML_Element("rdf", "li");
                            if (bsXMLKey == FX_BSTRC("title") || bsXMLKey == FX_BSTRC("description")) {
                                pAltLiElement->SetAttrValue("xml:lang", FX_WSTRC(L"x-default"));
                            }
                            pAltLiElement->AddChildContent(*wsValue.GetDataPtr(i));
                            pAltXMLElement->InsertChildElement(i, pAltLiElement);
                            if ((key == "Author") && bAuthorFirst) {
                                break;
                            }
                        }
                        bSetDone = TRUE;
                        break;
                    }
                }
                if (bSetDone == FALSE) {
                    ret = setNewDCToXML(NULL, pDCXMLElement, bsXMLSpace, bsXMLKey, wsValue);
                    if (!ret ) {
                        return ret;
                    }
                }
                bHasKey = TRUE;
                break;
            }
        }
    }
    if (FALSE == bHasKey) {
        CXML_Element* pDesXMLElement = NULL;
        if (bHasDes) {
            pDesXMLElement = pXMLRDF->GetElement(space, tagDescription, iDesIndex);
        } else {
            pDesXMLElement = pXMLRDF->GetElement(space, tagDescription, 0);
            if (pDesXMLElement) {
                pDesXMLElement->SetAttrValue(bsFlag, wsFlagStr);
            } else {
                pDesXMLElement = FX_NEW CXML_Element(space, tagDescription);
                pDesXMLElement->SetAttrValue("rdf:about", FX_WSTRC(L""));
                pDesXMLElement->SetAttrValue(bsFlag, wsFlagStr);
                pXMLRDF->AddChildElement(pDesXMLElement);
            }
        }
        ret = setNewDCToXML(pDesXMLElement, NULL, bsXMLSpace, bsXMLKey, wsValue);
        if (!ret) {
            return ret;
        }
    }
    CFX_ByteString bstrXMLStream = pXMLRoot->OutputStream();
    bstrXMLStream = FX_BSTR("<?xpacket begin=\"\xEF\xBB\xBF\" id=\"W5M0MpCehiHzreSzNTczkc9d\"?>\n") + bstrXMLStream
                    + FX_BSTR("\n<?xpacket end=\"w\"?>");
    CPDF_Dictionary *pRoot = ((PDFDOC_LPMETADATA)m_pData)->m_pDoc->GetRoot();
    CPDF_Stream *pXMLStream = pRoot->GetStream(FX_BSTRC("Metadata"));
    pXMLStream->SetData(bstrXMLStream, bstrXMLStream.GetLength(), FALSE, FALSE);
    return TRUE;
}
FX_BOOL CPDF_Metadata::SetMetadataStrArrayToXML(FX_BSTR key, const CFX_WideString& keyValue, FX_BOOL bAuthorFirst, FX_BOOL bKeywordsPdf)
{
    if(!GetRoot() || !GetRDF()) {
        if (!CreateNewMetadata()) {
            return FALSE;
        }
    }
    FX_BOOL ret = FALSE;
    if(key == "Title" || key == "Author" || key == "Subject") {
        ret = SetDCMetadataStrArrayToXML(key, keyValue, FALSE, bAuthorFirst);
    } else if (key == "pdfaid") {
        ret = SetPDFAMetadataStrArrayToXML(key, keyValue);
    } else if (key == "Keywords") {
        if (!bKeywordsPdf) {
            ret = SetDCMetadataStrArrayToXML(key, keyValue);
            if (!ret) {
                return ret;
            }
        }
        ret = SetXMPOrPDFOrPDFXMetadataStrArrayToXML(key, keyValue);
    } else {
        CFX_WideString XMPKey;
        CustomKeyTransfor::CustomKeyToXMPKey(CFX_ByteString(key).UTF8Decode(), XMPKey);
        ret = SetXMPOrPDFOrPDFXMetadataStrArrayToXML(XMPKey.UTF8Encode(), keyValue);
    }
    return ret;
}
FX_BOOL CPDF_Metadata::CreateDocInfoDict() const
{
    CPDF_Dictionary* pInfoDict = CPDF_Dictionary::Create();
    if (!pInfoDict) {
        return FALSE;
    }
    CPDF_Document* pDoc = ((PDFDOC_LPMETADATA)m_pData)->m_pDoc;
    pDoc->AddIndirectObject(pInfoDict);
    pDoc->SetInfoObjNum(pInfoDict->GetObjNum());
    CPDF_Parser* pParser = (CPDF_Parser*)pDoc->GetParser();
    if (pParser) {
        CPDF_Dictionary* pTrailer = pParser->GetTrailer();
        if (pTrailer) {
            pTrailer->SetAtReference(FX_BSTRC("Info"), pDoc, pInfoDict);
        }
    }
    return TRUE;
}
FX_BOOL CPDF_Metadata::SetMetadataStrArrayToInfo(FX_BSTR key, const CFX_WideString& keyValue)
{
    if (!((PDFDOC_LPMETADATA)m_pData)->m_pDoc->GetInfo()) {
        FX_BOOL ret = CreateDocInfoDict();
        if (!ret) {
            return FALSE;
        }
    }
    CFX_WideString wsValue = keyValue;
    if (key == FX_BSTRC("Author")) {
        wsValue = CPDF_KeyValueStringArray::GetFirst(keyValue);
    }
    CPDF_Dictionary* pInfoDict = ((PDFDOC_LPMETADATA)m_pData)->m_pDoc->GetInfo();
    pInfoDict->SetAtString(key, wsValue);
    return TRUE;
}
FX_BOOL CPDF_Metadata::SetString(FX_WSTR wsItem, const CFX_WideString& wsStr)
{
    if (wsItem.GetLength() == 0) {
        return FALSE;
    }
    CFX_ByteString bsItem = CFX_WideString(wsItem).UTF8Encode();
    FX_BOOL ret = TRUE;
    FX_INT32 i = 0;
    if (wsStr.GetLength() == 0) {
        for (; i < sizeof(bsKeyType) / sizeof(bsKeyType[0]); i++)
            if(bsItem == CFX_ByteString(bsKeyType[i])) {
                break;
            }
    }
    if (i >= sizeof(bsKeyType) / sizeof(bsKeyType[0])) {
        return FALSE;
    }
    if (bsItem != "pdfaid") {
        ret = SetMetadataStrArrayToInfo(bsItem, wsStr);
    }
    if (!ret) {
        return ret;
    }
    ret = SetMetadataStrArrayToXML(bsItem, wsStr);
    return ret;
}
FX_BOOL CPDF_Metadata::SetStringArray(FX_WSTR wsItem, const CFX_WideStringArray& wsStrArray)
{
    CFX_WideString wsStr;
    for (FX_INT32 i = 0; i < wsStrArray.GetSize(); i++) {
        wsStr += *wsStrArray.GetDataPtr(i);
        if (i != wsStrArray.GetSize() - 1) {
            wsStr += L";";
        }
    }
    return SetString(wsItem, wsStr);
}
FX_BOOL CPDF_Metadata::SetPDFAMetadataStrArrayToXML(FX_BSTR key, const CFX_WideString& keyValue, FX_BOOL bKeywordsNeedParse)
{
    FX_UNREFERENCED_PARAMETER(key);
    FX_UNREFERENCED_PARAMETER(bKeywordsNeedParse);
    CFX_ByteString bsXMLSpace(FX_BSTRC("pdfaid")), bsXMLKeyPart(FX_BSTRC("part")), bsXMLKeyConf(FX_BSTRC("conformance")), bsFlag(FX_BSTRC("xmlns:pdfaid"));
    CFX_WideString wsFlagStr(FX_WSTRC(L"http://www.aiim.org/pdfa/ns/id/"));
    CXML_Element* pXMLRoot = GetRoot();
    if (!pXMLRoot) {
        return FALSE;
    }
    CXML_Element* pXMLRDF = GetRDF();
    if (!pXMLRDF) {
        return FALSE;
    }
    FX_BSTR space = "rdf";
    FX_BSTR tagDescription = "Description";
    FX_DWORD dwCount = pXMLRDF->CountElements(space, tagDescription);
    FX_BOOL bHasDes = FALSE, bHasKey = FALSE;
    FX_INT32 iDesIndex = 0;
    for (FX_DWORD i = 0; i < dwCount; i++) {
        CXML_Element* pXMLElement = pXMLRDF->GetElement(space, tagDescription, i);
        if(pXMLElement && pXMLElement->HasAttr(bsFlag)) {
            bHasDes = TRUE;
            iDesIndex = i;
            if (pXMLElement->HasAttr(bsXMLSpace + ":" + bsXMLKeyPart)) {
                pXMLElement->SetAttrValue(bsXMLSpace + ":" + bsXMLKeyPart, keyValue.Mid(0, 1));
                pXMLElement->SetAttrValue(bsXMLSpace + ":" + bsXMLKeyConf, keyValue.Mid(1, 1));
                bHasKey = TRUE;
                break;
            }
            CXML_Element* pPDFAPartXMLElement = pXMLElement->GetElement(bsXMLSpace, bsXMLKeyPart);
            if (pPDFAPartXMLElement) {
                pPDFAPartXMLElement->RemoveChildren();
                pPDFAPartXMLElement->AddChildContent(keyValue.Mid(0, 1));
            } else {
                continue;
            }
            CXML_Element* pPDFAConfXMLElement = pXMLElement->GetElement(bsXMLSpace, bsXMLKeyConf);
            if (pPDFAConfXMLElement) {
                pPDFAConfXMLElement->RemoveChildren();
                pPDFAConfXMLElement->AddChildContent(keyValue.Mid(1, 1));
            } else {
                CXML_Element* pNewPDFConfXMLElement = FX_NEW CXML_Element(bsXMLSpace, bsXMLKeyConf);
                pNewPDFConfXMLElement->AddChildContent(keyValue.Mid(1, 1));
                pXMLElement->AddChildElement(pNewPDFConfXMLElement);
            }
            bHasKey = TRUE;
            break;
        }
    }
    if (!bHasKey) {
        CXML_Element* pDesXMLElement = NULL;
        if (bHasDes == TRUE) {
            pDesXMLElement = pXMLRDF->GetElement(space, tagDescription, iDesIndex);
        } else {
            pDesXMLElement = pXMLRDF->GetElement(space, tagDescription, 0);
            if (pDesXMLElement) {
                pDesXMLElement->SetAttrValue(bsFlag, wsFlagStr);
            } else {
                pDesXMLElement = FX_NEW CXML_Element(space, tagDescription);
                pDesXMLElement->SetAttrValue("rdf:about", FX_WSTRC(L""));
                pDesXMLElement->SetAttrValue(bsFlag, wsFlagStr);
                pXMLRDF->AddChildElement(pDesXMLElement);
            }
        }
        CXML_Element* pPDFPartXMLElem = FX_NEW CXML_Element(bsXMLSpace, bsXMLKeyPart);
        pPDFPartXMLElem->AddChildContent(keyValue.Mid(0, 1));
        CXML_Element* pPDFConfXMLElem = FX_NEW CXML_Element(bsXMLSpace, bsXMLKeyConf);
        pPDFConfXMLElem->AddChildContent(keyValue.Mid(1, 1));
        pDesXMLElement->AddChildElement(pPDFPartXMLElem);
        pDesXMLElement->AddChildElement(pPDFConfXMLElem);
    }
    CFX_ByteString bstrXMLStream =  pXMLRoot->OutputStream();
    bstrXMLStream = FX_BSTR("<?xpacket begin=\"\xEF\xBB\xBF\" id=\"W5M0MpCehiHzreSzNTczkc9d\"?>\n") + bstrXMLStream
                    + FX_BSTR("\n<?xpacket end=\"w\"?>");
    CPDF_Dictionary *pRoot = ((PDFDOC_LPMETADATA)m_pData)->m_pDoc->GetRoot();
    CPDF_Stream *pXMLStream = pRoot->GetStream(FX_BSTRC("Metadata"));
    pXMLStream->SetData(bstrXMLStream, bstrXMLStream.GetLength(), FALSE, FALSE);
    return TRUE;
}
void KeyMapToXML(FX_BSTR key, CFX_ByteString* bsXMLSpace, CFX_ByteString& bsXMLKey, CFX_WideString& wsFlagStr, FX_BOOL bPDF = FALSE)
{
    bsXMLKey = "";
    if (key == FX_BSTRC("Creator")) {
        bsXMLKey = FX_BSTRC("CreatorTool");
    } else if (key == "CreationDate") {
        bsXMLKey = FX_BSTR("CreateDate");
    } else if (key == "ModDate") {
        bsXMLKey = FX_BSTR("ModifyDate");
    } else if (key == "MetadataDate") {
        bsXMLKey = FX_BSTR("MetadataDate");
    } else if ((key == "Title") && !bPDF) {
        bsXMLKey = FX_BSTRC("Title");
    } else if((key == "Author") && !bPDF) {
        bsXMLKey = FX_BSTRC("Author");
    }
    if (bsXMLKey.GetLength() != 0) {
        bsXMLSpace[0] = "xmp";
        bsXMLSpace[1] = "xap";
        wsFlagStr = L"http://ns.adobe.com/xap/1.0/";
        return;
    }
    bsXMLKey = key;
    if ((key == "Producer") || (key == "Trapped") || (key == "Keywords") || (bPDF && (key == "Title" || key == "Author" || key == "Subject"))) {
        bsXMLSpace[0] = "pdf";
        wsFlagStr = L"http://ns.adobe.com/pdf/1.3/";
    } else {
        bsXMLSpace[0] = "pdfx";
        wsFlagStr = L"http://ns.adobe.com/pdfx/1.3/";
    }
}
FX_BOOL	CPDF_Metadata::SetXMPOrPDFOrPDFXMetadataStrArrayToXML(FX_BSTR key, const CFX_WideString& keyValue, FX_BOOL bKeywordsNeedParse)
{
    FX_UNREFERENCED_PARAMETER(key);
    FX_UNREFERENCED_PARAMETER(bKeywordsNeedParse);
    if(!GetRoot() || !GetRDF()) {
        if (!CreateNewMetadata()) {
            return FALSE;
        }
    }
    CFX_ByteString bsXMLSpace[] = {"", ""}, bsXMLKey;
    CFX_WideString wsFlagStr;
    KeyMapToXML(key, bsXMLSpace, bsXMLKey, wsFlagStr);
    CXML_Element* pXMLRoot = GetRoot();
    if (!pXMLRoot) {
        return FALSE;
    }
    CXML_Element* pXMLRDF = GetRDF();
    if (!pXMLRDF) {
        return FALSE;
    }
    FX_BSTR space = "rdf";
    FX_BSTR tagDescription = "Description";
    FX_DWORD dwCount = pXMLRDF->CountElements(space, tagDescription);
    FX_BOOL bHasDes = FALSE, bHasKey = FALSE;
    FX_INT32 iDesIndex = 0;
    for (FX_DWORD i = 0; i < dwCount; i++) {
        CXML_Element* pXMLElement = pXMLRDF->GetElement(space, tagDescription, i);
        if (!pXMLElement) {
            continue;
        }
        FX_INT32 j = 0;
        for (; j < sizeof(bsXMLSpace) / sizeof(bsXMLSpace[0]); j++) {
            if(pXMLElement && pXMLElement->HasAttr("xmlns:" + bsXMLSpace[j])) {
                bHasDes = TRUE;
                iDesIndex = i;
                if (pXMLElement->HasAttr(bsXMLSpace[j] + ":" + bsXMLKey)) {
                    bHasKey = TRUE;
                    pXMLElement->SetAttrValue(bsXMLSpace[j] + ":" + bsXMLKey, keyValue);
                    if (bsXMLKey.Equal("ModifyDate")) {
                        pXMLElement->SetAttrValue(bsXMLSpace[j] + ":MetadataDate", keyValue);
                    }
                    break;
                }
                CXML_Element* pSubXMLElement = pXMLElement->GetElement(bsXMLSpace[j], bsXMLKey);
                if (pSubXMLElement) {
                    pSubXMLElement->RemoveChildren();
                    pSubXMLElement->AddChildContent(keyValue);
                    if (bsXMLKey.Equal("ModifyDate")) {
                        CXML_Element* pMetadataXMLElement = pXMLElement->GetElement(bsXMLSpace[j],  "MetadataDate");
                        if (pMetadataXMLElement) {
                            pMetadataXMLElement->RemoveChildren();
                            pMetadataXMLElement->AddChildContent(keyValue);
                        } else {
                            CXML_Element* newMDXMLElem = FX_NEW CXML_Element(bsXMLSpace[j], "MetadataDate");
                            newMDXMLElem->AddChildContent(keyValue);
                            pXMLElement->AddChildElement(newMDXMLElem);
                        }
                    }
                    bHasKey = TRUE;
                    break;
                }
            }
        }
        if (j >= 0 && j < sizeof(bsXMLSpace) / sizeof(bsXMLSpace[0])) {
            break;
        }
    }
    if (bHasKey == FALSE) {
        CFX_ByteString bsSetXMLSpace = bsXMLSpace[0];
        CXML_Element* pDesXMLElement = NULL;
        if (bHasDes == TRUE) {
            pDesXMLElement = pXMLRDF->GetElement(space, tagDescription, iDesIndex);
            if (bsXMLSpace[1].GetLength() != 0 && pDesXMLElement && pDesXMLElement->HasAttr("xmlns:" + bsXMLSpace[1])) {
                bsSetXMLSpace = bsXMLSpace[1];
            }
        } else {
            pDesXMLElement = pXMLRDF->GetElement(space, tagDescription, 0);
            if (pDesXMLElement) {
                pDesXMLElement->SetAttrValue("xmlns:" + bsSetXMLSpace, wsFlagStr);
            } else {
                pDesXMLElement = FX_NEW CXML_Element(space, tagDescription);
                pDesXMLElement->SetAttrValue("rdf:about", FX_WSTRC(L""));
                pDesXMLElement->SetAttrValue("xmlns:" + bsSetXMLSpace, wsFlagStr);
                pXMLRDF->AddChildElement(pDesXMLElement);
            }
        }
        CXML_Element* pNewXMPXMLElem = FX_NEW CXML_Element(bsSetXMLSpace, bsXMLKey);
        pNewXMPXMLElem->AddChildContent(keyValue);
        pDesXMLElement->AddChildElement(pNewXMPXMLElem);
        if (bsXMLKey.Equal("ModifyDate")) {
            CXML_Element* newMetadataDateElement = FX_NEW CXML_Element(bsSetXMLSpace, "MetadataDate");
            newMetadataDateElement->AddChildContent(keyValue);
            pDesXMLElement->AddChildElement(newMetadataDateElement);
        }
    }
    CFX_ByteString bstrXMLStream = pXMLRoot->OutputStream();
    bstrXMLStream = FX_BSTR("<?xpacket begin=\"\xEF\xBB\xBF\" id=\"W5M0MpCehiHzreSzNTczkc9d\"?>\n") + bstrXMLStream
                    + FX_BSTR("\n<?xpacket end=\"w\"?>");
    CPDF_Dictionary *pRoot = ((PDFDOC_LPMETADATA)m_pData)->m_pDoc->GetRoot();
    CPDF_Stream *pXMLStream = pRoot->GetStream(FX_BSTRC("Metadata"));
    pXMLStream->SetData(bstrXMLStream, bstrXMLStream.GetLength(), FALSE, FALSE);
    return TRUE;
}
FX_INT32 CPDF_Metadata::CompareModDT() const
{
    CFX_WideString infoDt;
    GetStringFromInfo(FX_BSTRC("ModDate"), infoDt);
    CFX_WideStringArray metadataDtArray;
    GetXMPOrPDFOrPDFXMetadataStringFromXML(FX_BSTRC("MetadataDate"), metadataDtArray);
    if (!GetRDF()) {
        return CPDF_METADATATYPE_INFO;
    }
    if (infoDt.GetLength() == 0) {
        return CPDF_METADATATYPE_XML;
    }
    if (metadataDtArray.GetSize() == 0) {
        return CPDF_METADATATYPE_INFO;
    }
    CFX_WideString metadataDt = *metadataDtArray.GetDataPtr(0);
    CPDF_DateTime dtInfo;
    CPDF_DateTime dtMetadata;
    dtInfo.ParserPDFDateTimeString(infoDt.UTF8Encode());
    dtMetadata.ParserPDFXMPDateTimeString(metadataDt);
    FX_INT32 r = FXCRT_CompareDateTime(&dtMetadata.dt, &dtInfo.dt);
    if (r >= 0) {
        return CPDF_METADATATYPE_XML;
    } else {
        return CPDF_METADATATYPE_INFO;
    }
}
FX_INT32 CPDF_Metadata::GetStringFromInfo(FX_BSTR key, CFX_WideString& keyValue) const
{
    CPDF_Dictionary* pInfo = ((PDFDOC_LPMETADATA)m_pData)->m_pDoc->GetInfo();
    if (!pInfo) {
        return FX_ERR_CODE_METADATA_NOTFOUND_KEY;
    }
    CPDF_Object* pObj = pInfo->GetElement(key);
    if (pObj) {
        FX_INT32 type = pObj->GetType();
        if (type != PDFOBJ_STRING) {
            return FX_ERR_CODE_METADATA_NOTFOUND_KEY;
        }
    }
    if (!pInfo->KeyExist(key)) {
        return FX_ERR_CODE_METADATA_NOTFOUND_KEY;
    }
    keyValue = PDF_DecodeText(pInfo->GetString(key));
    if (keyValue.GetLength() == 0) {
        return FX_ERR_CODE_METADATA_NOTFOUND_KEYVALUE;
    }
    return 0;
}
FX_INT32 CPDF_Metadata::GetStringFromXML(FX_BSTR key, CFX_WideStringArray& keyArray, FX_INT32& keywordsUsePDFOrDC) const
{
    FX_INT32 ret = -1;
    if(key == "Title" || key == "Author" || key == "Subject") {
        ret = GetDCMetadataStringFromXML(key, keyArray);
    } else if (key == "pdfaid") {
        ret = GetPDFAMetadataStringFromXML(key, keyArray);
    } else if (key == "Keywords") {
        if (keywordsUsePDFOrDC == CPDF_METADATA_KEYWORDS_ALL) {
            CFX_WideStringArray dcValue;
            FX_INT32 rDoc = GetDCMetadataStringFromXML(key, dcValue);
            CFX_WideStringArray pdfValue;
            FX_INT32 rPDF = GetXMPOrPDFOrPDFXMetadataStringFromXML(key, pdfValue);
            if (dcValue.GetSize() == 0) {
                keyArray.Copy(pdfValue);
            } else if (pdfValue.GetSize() == 0) {
                keyArray.Copy(dcValue);
            } else {
                FX_BOOL bRet = CPDF_KeyValueStringArray::CompareKeywords(dcValue, *pdfValue.GetDataPtr(0));
                if (!bRet) {
                    pdfValue.Append(dcValue);
                } else {
                    keywordsUsePDFOrDC = CPDF_METADATA_KEYWORDS_PDF;
                }
                keyArray.Copy(pdfValue);
            }
            if (rDoc == FX_ERR_CODE_METADATA_NOTFOUND_KEY && rPDF == FX_ERR_CODE_METADATA_NOTFOUND_KEY) {
                ret = FX_ERR_CODE_METADATA_NOTFOUND_KEY;
            } else if (rDoc == 0 || rPDF == 0) {
                ret = 0;
            } else {
                ret = FX_ERR_CODE_METADATA_NOTFOUND_KEYVALUE;
            }
        } else if (keywordsUsePDFOrDC == CPDF_METADATA_KEYWORDS_DC) {
            ret = GetDCMetadataStringFromXML(key, keyArray);
        } else if (keywordsUsePDFOrDC == CPDF_METADATA_KEYWORDS_PDF) {
            ret = GetXMPOrPDFOrPDFXMetadataStringFromXML(key, keyArray);
        }
    } else {
        ret = GetXMPOrPDFOrPDFXMetadataStringFromXML(key, keyArray);
    }
    return ret;
}
FX_INT32 CPDF_Metadata::GetDCMetadataStringFromXML(FX_BSTR key, CFX_WideStringArray& keyArray) const
{
    FX_INT32 ret = 0;
    CXML_Element* pXMLRDF = GetRDF();
    if (!pXMLRDF) {
        return FX_ERR_CODE_METADATA_NOTFOUND_KEY;
    }
    CFX_ByteString bsXMLKey;
    if (key == "Title") {
        bsXMLKey = FX_BSTRC("title");
    } else if(key == "Author") {
        bsXMLKey = FX_BSTRC("creator");
    } else if(key == "Subject") {
        bsXMLKey = FX_BSTRC("description");
    } else if(key == "Keywords") {
        bsXMLKey = FX_BSTRC("subject");
    }
    FX_BOOL bHasKey = FALSE;
    CFX_ByteString bsXMLSpace(FX_BSTRC("dc"));
    FX_BSTR space = "rdf";
    FX_BSTR tagDescription = "Description";
    FX_DWORD dwCount = pXMLRDF->CountElements(space, tagDescription);
    for (FX_DWORD i = 0; i < dwCount; i++) {
        CXML_Element* pDesElement = pXMLRDF->GetElement(space, tagDescription, i);
        if (pDesElement) {
            CFX_WideString wsStr;
            if(pDesElement->HasAttr(bsXMLSpace + ":" + bsXMLKey)) {
                bHasKey = TRUE;
                wsStr = pDesElement->GetAttrValue(bsXMLSpace, bsXMLKey);
                keyArray.Add(wsStr);
                break;
            } else {
                CXML_Element* pXMLElement = pDesElement->GetElement(bsXMLSpace, bsXMLKey);
                if (!pXMLElement) {
                    continue;
                }
                bHasKey = TRUE;
                CFX_ByteString strArrayKey;
                CFX_ByteString strArrayKeyArr[] = {"Seq", "Bag", "Alt"};
                for (FX_INT32 q = 0; q < sizeof(strArrayKeyArr) / sizeof(strArrayKeyArr[0]); q++) {
                    if (0 != pXMLElement->CountElements(FX_BSTRC("rdf"), strArrayKeyArr[q])) {
                        strArrayKey = strArrayKeyArr[q];
                        break;
                    }
                }
                if (strArrayKey.IsEmpty()) {
                    wsStr = pXMLElement->GetContent(0);
                    keyArray.Add(wsStr);
                    break;
                }
                CXML_Element* pSubXMLElement = pXMLElement->GetElement(FX_BSTRC("rdf"), strArrayKey);
                if (!pSubXMLElement) {
                    break;
                }
                FX_DWORD dwCountArray = pSubXMLElement->CountElements(FX_BSTRC("rdf"), FX_BSTRC("li"));
                if (strArrayKey.Equal("Alt") && dwCountArray > 0) {
                    dwCountArray = 1;
                }
                FX_DWORD j = 0;
                for (; j < dwCountArray; j++) {
                    CXML_Element* pLiXMLElement = pSubXMLElement->GetElement(FX_BSTRC("rdf"), FX_BSTRC("li"), j);
                    if (!pLiXMLElement) {
                        break;
                    }
                    wsStr = pLiXMLElement->GetContent(0);
                    keyArray.Add(wsStr);
                    if (bsXMLKey == FX_BSTRC("subject") && pLiXMLElement->CountAttrs() == 1) {
                        wsStr = pLiXMLElement->GetAttrValue(FX_BSTRC("rdf"), FX_BSTRC("li"));
                        keyArray.Add(wsStr);
                    }
                }
                if (j >= dwCountArray) {
                    break;
                }
            }
        }
    }
    if (bHasKey == FALSE) {
        ret = FX_ERR_CODE_METADATA_NOTFOUND_KEY;
    } else if (keyArray.GetSize() == 0) {
        ret = FX_ERR_CODE_METADATA_NOTFOUND_KEYVALUE;
    } else {
        for (FX_INT32 i = 0; i < keyArray.GetSize(); i++)
            if (keyArray.GetDataPtr(i)->GetLength() != 0) {
                return 0;
            }
        ret = FX_ERR_CODE_METADATA_NOTFOUND_KEYVALUE;
    }
    if (key != "Keywords" && (ret == FX_ERR_CODE_METADATA_NOTFOUND_KEY || ret == FX_ERR_CODE_METADATA_NOTFOUND_KEYVALUE)) {
        keyArray.RemoveAll();
        if (GetXMPOrPDFOrPDFXMetadataStringFromXML(key, keyArray, TRUE)) {
            return ret;
        }
        return 0;
    }
    return ret;
}
FX_INT32 CPDF_Metadata::GetPDFAMetadataStringFromXML(FX_BSTR key, CFX_WideStringArray& keyArray) const
{
    FX_UNREFERENCED_PARAMETER(key);
    CXML_Element* pXMLRDF = GetRDF();
    if (!pXMLRDF) {
        return FX_ERR_CODE_METADATA_NOTFOUND_KEY;
    }
    CFX_WideString wsXMLValue, wsXMLValuePart, wsXMLValueConf(FX_WSTRC(L""));
    CFX_ByteString bsXMLSpace(FX_BSTRC("pdfaid")), bsXMLKeyPart(FX_BSTRC("part")), bsXMLKeyConf(FX_BSTRC("conformance"));
    FX_BSTR space = "rdf";
    FX_BSTR tagDescription = "Description";
    FX_DWORD dwCount = pXMLRDF->CountElements(space, tagDescription);
    FX_BOOL bHasKey = FALSE;
    for (FX_DWORD i = 0; i < dwCount; i++) {
        CXML_Element* pDesElement = pXMLRDF->GetElement(space, tagDescription, i);
        if (pDesElement) {
            if (pDesElement->HasAttr(bsXMLSpace + ":" + bsXMLKeyPart) && pDesElement->HasAttr(bsXMLSpace + ":" + bsXMLKeyConf)) {
                bHasKey = TRUE;
                wsXMLValuePart = pDesElement->GetAttrValue(bsXMLSpace, bsXMLKeyPart);
                wsXMLValueConf = pDesElement->GetAttrValue(bsXMLSpace, bsXMLKeyConf);
                wsXMLValue = wsXMLValueConf + wsXMLValuePart;
                break;
            } else {
                CXML_Element* pPartXMLElement = pDesElement->GetElement(bsXMLSpace, bsXMLKeyPart);
                if (!pPartXMLElement) {
                    continue;
                }
                wsXMLValuePart = pPartXMLElement->GetContent(0);
                CXML_Element* pConfXMLElement = pDesElement->GetElement(bsXMLSpace, bsXMLKeyConf);
                if (!pConfXMLElement) {
                    continue;
                }
                bHasKey = TRUE;
                wsXMLValueConf = pConfXMLElement->GetContent(0);
                wsXMLValue = wsXMLValuePart + wsXMLValueConf;
                break;
            }
        }
    }
    keyArray.Add(wsXMLValue);
    if (bHasKey == FALSE) {
        return FX_ERR_CODE_METADATA_NOTFOUND_KEY;
    }
    if (keyArray.GetDataPtr(0)->GetLength() == 0) {
        return FX_ERR_CODE_METADATA_NOTFOUND_KEYVALUE;
    }
    return 0;
}
void GetTitleFromXMP(CXML_Element* pXMLElement, CFX_WideStringArray& keyArray)
{
    CFX_ByteString strArrayKey;
    CFX_ByteString strArrayKeyArr[] = {"Seq", "Bag", "Alt"};
    for (FX_INT32 q = 0; q < sizeof(strArrayKeyArr) / sizeof(strArrayKeyArr[0]); q++) {
        if (0 != pXMLElement->CountElements(FX_BSTRC("rdf"), strArrayKeyArr[q])) {
            strArrayKey = strArrayKeyArr[q];
            break;
        }
    }
    if (strArrayKey.IsEmpty()) {
        return;
    }
    CXML_Element* pSubXMLElement = pXMLElement->GetElement(FX_BSTRC("rdf"), strArrayKey);
    if (!pSubXMLElement) {
        return;
    }
    FX_DWORD dwCountArray = pSubXMLElement->CountElements(FX_BSTRC("rdf"), FX_BSTRC("li"));
    if (dwCountArray == 0) {
        return;
    }
    CXML_Element* pLiXMLElement = pSubXMLElement->GetElement(FX_BSTRC("rdf"), FX_BSTRC("li"), 0);
    if (!pLiXMLElement) {
        return;
    }
    CFX_WideString wsStr = pLiXMLElement->GetContent(0);
    keyArray.Add(wsStr);
}
FX_INT32 CPDF_Metadata::GetXMPOrPDFOrPDFXMetadataStringFromXML(FX_BSTR key, CFX_WideStringArray& keyArray, FX_BOOL bPDF) const
{
    CXML_Element* pXMLRDF = GetRDF();
    if (!pXMLRDF) {
        return FX_ERR_CODE_METADATA_NOTFOUND_KEY;
    }
    CFX_ByteString bsXMLSpace[2] = {"", ""}, bsXMLKey;
    CFX_WideString wsFlagStr;
    KeyMapToXML(key, bsXMLSpace, bsXMLKey, wsFlagStr, bPDF);
    FX_BSTR space = "rdf";
    FX_BSTR tagDescription = "Description";
    FX_DWORD dwCount = pXMLRDF->CountElements(space, tagDescription);
    FX_BOOL bHasKey = FALSE;
    for (FX_DWORD i = 0; i < dwCount; i++) {
        CXML_Element* pDesElement = pXMLRDF->GetElement(space, tagDescription, i);
        if (pDesElement) {
            CFX_WideString wsStr;
            FX_INT32 j = 0;
            for (; j < sizeof(bsXMLSpace) / sizeof (bsXMLSpace[0]); j++) {
                if (bsXMLSpace[j].GetLength() == 0) {
                    continue;
                }
                if (pDesElement->HasAttr(bsXMLSpace[j] + ":" + bsXMLKey)) {
                    bHasKey = TRUE;
                    wsStr = pDesElement->GetAttrValue(bsXMLSpace[j], bsXMLKey);
                    keyArray.Add(wsStr);
                    break;
                } else {
                    CXML_Element* pXMLElement = pDesElement->GetElement(bsXMLSpace[j], bsXMLKey);
                    if (!pXMLElement) {
                        continue;
                    }
                    bHasKey = TRUE;
                    if ((bsXMLKey == "Title") && (bsXMLSpace[0] == "xmp"))  {
                        keyArray.RemoveAll();
                        GetTitleFromXMP(pXMLElement, keyArray);
                        break;
                    }
                    wsStr = pXMLElement->GetContent(0);
                    keyArray.Add(wsStr);
                    break;
                }
            }
            if (j >= 0 && j < sizeof(bsXMLSpace) / sizeof (bsXMLSpace[0])) {
                break;
            }
        }
    }
    if (bHasKey == FALSE) {
        if (bsXMLSpace[0] == "pdf" && (bsXMLKey == "Author" || bsXMLKey == "Title")) {
            keyArray.RemoveAll();
            return GetXMPOrPDFOrPDFXMetadataStringFromXML(bsXMLKey, keyArray, FALSE);
        }
        return FX_ERR_CODE_METADATA_NOTFOUND_KEY;
    }
    if (keyArray.GetSize() == 0 || (keyArray.GetDataPtr(0) && keyArray.GetDataPtr(0)->GetLength() == 0)) {
        return FX_ERR_CODE_METADATA_NOTFOUND_KEYVALUE;
    }
    return 0;
}
static FX_BOOL setNewDCToXML(CXML_Element* pXMLElement, CXML_Element* pDCXMLElement, CFX_ByteString bsXMLSpace, CFX_ByteString bsXMLKey, CFX_WideStringArray& wsValue)
{
    FX_BOOL bNewDC = FALSE;
    if (!pDCXMLElement) {
        bNewDC = TRUE;
        pDCXMLElement = FX_NEW CXML_Element(bsXMLSpace, bsXMLKey);
    }
    if (bsXMLKey.Equal(FX_BSTRC("title")) || bsXMLKey.Equal(FX_BSTRC("description"))) {
        CXML_Element* pNewAltElem = FX_NEW CXML_Element("rdf", "Alt");
        CXML_Element* pNewAltliElem = FX_NEW CXML_Element("rdf", "li");
        pNewAltliElem->SetAttrValue("xml:lang", FX_WSTRC(L"x-default"));
        pNewAltliElem->AddChildContent(*wsValue.GetDataPtr(0));
        pNewAltElem->AddChildElement(pNewAltliElem);
        pDCXMLElement->AddChildElement(pNewAltElem);
    } else {
        CXML_Element* pNewBagElem = FX_NEW CXML_Element("rdf", "Bag");
        for (FX_INT32 i = 0; i < wsValue.GetSize(); i++) {
            CXML_Element* pNewBagLiElem = FX_NEW CXML_Element("rdf", "li");
            pNewBagLiElem->AddChildContent(*wsValue.GetDataPtr(i));
            pNewBagElem->AddChildElement(pNewBagLiElem);
        }
        pDCXMLElement->AddChildElement(pNewBagElem);
    }
    if (bNewDC && pXMLElement) {
        pXMLElement->AddChildElement(pDCXMLElement);
    }
    return TRUE;
}
