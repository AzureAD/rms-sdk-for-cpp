#ifndef _FPDF_DOC_DATETIME_H_
#define _FPDF_DOC_DATETIME_H_
class CPDF_DateTime
{
public:
    CPDF_DateTime();
    CPDF_DateTime(const CFX_ByteString& pdfDateTimeStr);
    CPDF_DateTime(const FXCRT_DATETIMEZONE& st);
    FX_BOOL			ParserPDFDateTimeString(const CFX_ByteString& dtStr);
    FX_BOOL			ParserPDFXMPDateTimeString(const CFX_WideString& dtStr);
    CFX_ByteString	ToPDFDateTimeString();
    CFX_WideString  ToXMPDateTimeString();
    void			SetCurrentSystemDateTime();
    FXCRT_DATETIMEZONE dt;
protected:
    inline FX_BOOL ParserYear(const CFX_ByteString& dtStr, FX_INT32 &index, FX_BOOL &bContinue);
    inline FX_BOOL ParserMonth(const CFX_ByteString& dtStr, FX_INT32 &index, FX_BOOL &bContinue);
    inline FX_BOOL ParserDay(const CFX_ByteString& dtStr, FX_INT32 &index, FX_BOOL &bContinue);
    inline FX_BOOL ParserHour(const CFX_ByteString& dtStr, FX_INT32 &index, FX_BOOL &bContinue);
    inline FX_BOOL ParserMinute(const CFX_ByteString& dtStr, FX_INT32 &index, FX_BOOL &bContinue);
    inline FX_BOOL ParserSecond(const CFX_ByteString& dtStr, FX_INT32 &index, FX_BOOL &bContinue);
    inline FX_BOOL ParserTimeZone(const CFX_ByteString& dtStr, FX_INT32 &index);
    inline FX_BOOL ParserXMPTimeZone(const CFX_ByteString& dtStr, FX_INT32 &index);
};
#endif
