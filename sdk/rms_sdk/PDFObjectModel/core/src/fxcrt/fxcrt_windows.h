#ifndef _FXCRT_WINDOWS_
#define _FXCRT_WINDOWS_
#include "extension.h"
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
class CFXCRT_FileAccess_Win64 : public IFXCRT_FileAccess, public CFX_Object
{
public:
    CFXCRT_FileAccess_Win64();
    virtual ~CFXCRT_FileAccess_Win64();
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
    FX_LPVOID	m_hFile;
};
#endif
#endif
