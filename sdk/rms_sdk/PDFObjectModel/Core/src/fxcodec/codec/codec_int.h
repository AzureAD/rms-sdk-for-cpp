/*
 * ======================================================================
 * Copyright (c) Foxit Software, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 *======================================================================
 */

#include <limits.h>
struct CCodec_ImageDataCache {
    int			m_Width, m_Height;
    int			m_nCachedLines;
    FX_BYTE		m_Data;
};
class CCodec_ScanlineDecoder : public ICodec_ScanlineDecoder
{
public:

    CCodec_ScanlineDecoder();

    virtual ~CCodec_ScanlineDecoder();

    virtual FX_DWORD	GetSrcOffset()
    {
        return -1;
    }

    virtual void		DownScale(int dest_width, int dest_height);

    FX_LPBYTE			GetScanline(int line);

    FX_BOOL				SkipToScanline(int line, IFX_Pause* pPause);

    int					GetWidth()
    {
        return m_OutputWidth;
    }

    int					GetHeight()
    {
        return m_OutputHeight;
    }

    int					CountComps()
    {
        return m_nComps;
    }

    int					GetBPC()
    {
        return m_bpc;
    }

    FX_BOOL				IsColorTransformed()
    {
        return m_bColorTransformed;
    }

    void				ClearImageData()
    {
        if (m_pDataCache) {
            FX_Free(m_pDataCache);
        }
        m_pDataCache = NULL;
    }
protected:

    int					m_OrigWidth;

    int					m_OrigHeight;

    int					m_DownScale;

    int					m_OutputWidth;

    int					m_OutputHeight;

    int					m_nComps;

    int					m_bpc;

    int					m_Pitch;

    FX_BOOL				m_bColorTransformed;

    FX_LPBYTE			ReadNextLine();

    virtual FX_BOOL		v_Rewind() = 0;

    virtual FX_LPBYTE	v_GetNextLine() = 0;

    virtual void		v_DownScale(int dest_width, int dest_height) = 0;

    int					m_NextLine;

    FX_LPBYTE			m_pLastScanline;

    CCodec_ImageDataCache*	m_pDataCache;
};
class CCodec_FlateModule : public ICodec_FlateModule
{
public:
    virtual ICodec_ScanlineDecoder*	CreateDecoder(FX_LPCBYTE src_buf, FX_DWORD src_size, int width, int height,
            int nComps, int bpc, int predictor, int Colors, int BitsPerComponent, int Columns, int pitch = 0);
    virtual FX_DWORD FlateOrLZWDecode(FX_BOOL bLZW, const FX_BYTE* src_buf, FX_DWORD src_size, FX_BOOL bEarlyChange,
                                      int predictor, int Colors, int BitsPerComponent, int Columns,
                                      FX_DWORD estimated_size, FX_LPBYTE& dest_buf, FX_DWORD& dest_size);
    virtual FX_BOOL Encode(const FX_BYTE* src_buf, FX_DWORD src_size,
                           int predictor, int Colors, int BitsPerComponent, int Columns,
                           FX_LPBYTE& dest_buf, FX_DWORD& dest_size);
    virtual FX_BOOL		Encode(FX_LPCBYTE src_buf, FX_DWORD src_size, FX_LPBYTE& dest_buf, FX_DWORD& dest_size);
};
