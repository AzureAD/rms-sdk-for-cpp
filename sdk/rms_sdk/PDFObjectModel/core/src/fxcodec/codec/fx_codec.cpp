#include "../../../include/fxcodec/fx_codec.h"
#include "codec_int.h"
CCodec_ModuleMgr::CCodec_ModuleMgr()
{
    m_pFlateModule = FX_NEW CCodec_FlateModule;
}
CCodec_ModuleMgr::~CCodec_ModuleMgr()
{
    delete m_pFlateModule;
}
CCodec_ScanlineDecoder::CCodec_ScanlineDecoder()
{
    m_OrigHeight = 0;
    m_OrigWidth = 0;
    m_DownScale = 0;
    m_OutputWidth = 0;
    m_OutputHeight = 0;
    m_nComps = 0;
    m_bpc = 0;
    m_Pitch = 0;
    m_bColorTransformed = FALSE;
    m_NextLine = -1;
    m_pDataCache = NULL;
    m_pLastScanline = NULL;
}
CCodec_ScanlineDecoder::~CCodec_ScanlineDecoder()
{
    if (m_pDataCache) {
        FX_Free(m_pDataCache);
    }
}
FX_LPBYTE CCodec_ScanlineDecoder::GetScanline(int line)
{
    if (m_pDataCache && line < m_pDataCache->m_nCachedLines) {
        return &m_pDataCache->m_Data + line * m_Pitch;
    }
    if (m_NextLine == line + 1) {
        return m_pLastScanline;
    }
    if (m_NextLine < 0 || m_NextLine > line) {
        if (!v_Rewind()) {
            return NULL;
        }
        m_NextLine = 0;
    }
    while (m_NextLine < line) {
        ReadNextLine();
        m_NextLine ++;
    }
    m_pLastScanline = ReadNextLine();
    m_NextLine ++;
    return m_pLastScanline;
}
FX_BOOL CCodec_ScanlineDecoder::SkipToScanline(int line, IFX_Pause* pPause)
{
    if (m_pDataCache && line < m_pDataCache->m_nCachedLines) {
        return FALSE;
    }
    if (m_NextLine == line || m_NextLine == line + 1) {
        return FALSE;
    }
    if (m_NextLine < 0 || m_NextLine > line) {
        v_Rewind();
        m_NextLine = 0;
    }
    m_pLastScanline = NULL;
    while (m_NextLine < line) {
        m_pLastScanline = ReadNextLine();
        m_NextLine ++;
        if (pPause && pPause->NeedToPauseNow()) {
            return TRUE;
        }
    }
    return FALSE;
}
FX_LPBYTE CCodec_ScanlineDecoder::ReadNextLine()
{
    FX_LPBYTE pLine = v_GetNextLine();
    if (pLine == NULL) {
        return NULL;
    }
    if (m_pDataCache && m_NextLine == m_pDataCache->m_nCachedLines) {
        FXSYS_memcpy32(&m_pDataCache->m_Data + m_NextLine * m_Pitch, pLine, m_Pitch);
        m_pDataCache->m_nCachedLines ++;
    }
    return pLine;
}
void CCodec_ScanlineDecoder::DownScale(int dest_width, int dest_height)
{
    if (dest_width < 0) {
        dest_width = -dest_width;
    }
    if (dest_height < 0) {
        dest_height = -dest_height;
    }
    v_DownScale(dest_width, dest_height);
    if (m_pDataCache) {
        if (m_pDataCache->m_Height == m_OutputHeight && m_pDataCache->m_Width == m_OutputWidth) {
            return;
        }
        FX_Free(m_pDataCache);
        m_pDataCache = NULL;
    }
    m_pDataCache = (CCodec_ImageDataCache*)FX_AllocNL(FX_BYTE, sizeof(CCodec_ImageDataCache) + m_Pitch * m_OutputHeight);
    if (m_pDataCache == NULL) {
        return;
    }
    m_pDataCache->m_Height = m_OutputHeight;
    m_pDataCache->m_Width = m_OutputWidth;
    m_pDataCache->m_nCachedLines = 0;
}
CCodec_ModuleMgr* CCodec_ModuleMgr::Create()
{
    return FX_NEW CCodec_ModuleMgr;
}
void CCodec_ModuleMgr::Destroy()
{
    delete this;
}
