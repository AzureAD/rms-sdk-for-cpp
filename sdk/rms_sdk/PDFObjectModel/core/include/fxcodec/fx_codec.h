#ifndef _FX_CODEC_H_
#define _FX_CODEC_H_
#ifndef _FXCRT_EXTENSION_
#include "../fxcrt/fx_ext.h"
#endif
class ICodec_FlateModule;
class CCodec_ModuleMgr : public CFX_Object
{
public:

    static CCodec_ModuleMgr*	Create();

    void				Destroy();
protected:
    CCodec_ModuleMgr();
    ~CCodec_ModuleMgr();
public:
    ICodec_FlateModule*	GetFlateModule()
    {
        return m_pFlateModule;
    }
protected:
    ICodec_FlateModule*	m_pFlateModule;
};
class ICodec_ScanlineDecoder : public CFX_Object
{
public:

    virtual ~ICodec_ScanlineDecoder() {}

    virtual FX_DWORD	GetSrcOffset() = 0;

    virtual void		DownScale(int dest_width, int dest_height) = 0;

    virtual FX_LPBYTE	GetScanline(int line) = 0;

    virtual FX_BOOL		SkipToScanline(int line, IFX_Pause* pPause) = 0;

    virtual int			GetWidth() = 0;

    virtual int			GetHeight() = 0;

    virtual int			CountComps() = 0;

    virtual int			GetBPC() = 0;

    virtual FX_BOOL		IsColorTransformed() = 0;

    virtual void		ClearImageData() = 0;
};
class ICodec_FlateModule : public CFX_Object
{
public:

    virtual ~ICodec_FlateModule() {}
    virtual ICodec_ScanlineDecoder*	CreateDecoder(FX_LPCBYTE src_buf, FX_DWORD src_size, int width, int height,
            int nComps, int bpc, int predictor, int Colors, int BitsPerComponent, int Columns, int pitch = 0) = 0;
    virtual FX_DWORD	FlateOrLZWDecode(FX_BOOL bLZW, const FX_BYTE* src_buf, FX_DWORD src_size, FX_BOOL bEarlyChange,
                                         int predictor, int Colors, int BitsPerComponent, int Columns,
                                         FX_DWORD estimated_size, FX_LPBYTE& dest_buf, FX_DWORD& dest_size) = 0;
    virtual FX_BOOL		Encode(const FX_BYTE* src_buf, FX_DWORD src_size,
                               int predictor, int Colors, int BitsPerComponent, int Columns,
                               FX_LPBYTE& dest_buf, FX_DWORD& dest_size) = 0;
    virtual FX_BOOL		Encode(FX_LPCBYTE src_buf, FX_DWORD src_size, FX_LPBYTE& dest_buf, FX_DWORD& dest_size) = 0;
};
#endif
