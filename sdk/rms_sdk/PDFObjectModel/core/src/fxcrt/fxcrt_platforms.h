#ifndef _FXCRT_PLATFORMS_
#define _FXCRT_PLATFORMS_
#include "extension.h"
#if _FX_OS_ == _FX_ANDROID_
void	FXCRT_GetFileModeString(FX_DWORD dwModes, CFX_ByteString &bsMode);
void	FXCRT_GetFileModeString(FX_DWORD dwModes, CFX_WideString &wsMode);
class CFXCRT_FileAccess_CRT : public IFXCRT_FileAccess, public CFX_Object
{
public:
    CFXCRT_FileAccess_CRT();
    virtual ~CFXCRT_FileAccess_CRT();
    virtual FX_BOOL		Open(FX_BSTR fileName, FX_DWORD dwMode);
    virtual FX_BOOL		Open(FX_WSTR fileName, FX_DWORD dwMode);
    virtual void		Close();
    virtual void		Release();
    virtual FX_FILESIZE	GetSize() const;
    virtual FX_FILESIZE	GetPosition() const;
    virtual FX_FILESIZE	SetPosition(FX_FILESIZE pos);
    virtual size_t		Read(void* pBuffer, size_t szBuffer);
    virtual size_t		Write(const void* pBuffer, size_t szBuffer);
    virtual size_t		ReadPos(void* pBuffer, size_t szBuffer, FX_FILESIZE pos);
    virtual size_t		WritePos(const void* pBuffer, size_t szBuffer, FX_FILESIZE pos);
    virtual FX_BOOL		Flush();
    virtual FX_BOOL		Truncate(FX_FILESIZE szFile);
protected:
    FXSYS_FILE*	m_hFile;
};
#endif
#endif
