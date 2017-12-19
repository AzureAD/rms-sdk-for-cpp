#ifndef _FXCRT_LINUX_
#define _FXCRT_LINUX_
#include "extension.h"
#if _FXM_PLATFORM_ == _FXM_PLATFORM_LINUX_ || _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_ || _FXM_PLATFORM_ == _FXM_PLATFORM_ANDROID_
class CFXCRT_FileAccess_Posix : public IFXCRT_FileAccess, public CFX_Object
{
public:
    CFXCRT_FileAccess_Posix();
    virtual ~CFXCRT_FileAccess_Posix();
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
    FX_INT32	m_nFD;
};
#endif
#endif
