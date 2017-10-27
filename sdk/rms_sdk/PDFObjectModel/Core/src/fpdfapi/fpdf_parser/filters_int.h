/*
 * ======================================================================
 * Copyright (c) Foxit Software, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 *======================================================================
 */

class CPDF_DecryptFilter : public CFX_DataFilter
{
public:
    CPDF_DecryptFilter(CPDF_CryptoHandler* pCryptoHandler, FX_DWORD objnum, FX_DWORD gennum, FX_UINT32 newInBufferSize);
    virtual ~CPDF_DecryptFilter();
    virtual	void	v_FilterIn(FX_LPCBYTE src_buf, size_t src_size, CFX_BinaryBuf& dest_buf);
    virtual void	v_FilterFinish(CFX_BinaryBuf& dest_buf);
    CPDF_CryptoHandler*	crypto_handler_;
    FX_LPVOID		m_pContext;
    FX_DWORD		m_ObjNum, m_GenNum;
};
class CPDF_FlateFilter : public CFX_DataFilter
{
public:
    CPDF_FlateFilter();
    virtual ~CPDF_FlateFilter();
    virtual	void	v_FilterIn(FX_LPCBYTE src_buf, size_t src_size, CFX_BinaryBuf& dest_buf);
    virtual void	v_FilterFinish(CFX_BinaryBuf& dest_buf) {}
    void*			m_pContext;
    FX_BYTE			m_DestBuffer[FPDF_FILTER_BUFFER_SIZE];
};
class CPDF_LzwFilter : public CFX_DataFilter
{
public:
    CPDF_LzwFilter(FX_BOOL bEarlyChange);
    virtual ~CPDF_LzwFilter() {}
    virtual	void	v_FilterIn(FX_LPCBYTE src_buf, size_t src_size, CFX_BinaryBuf& dest_buf);
    virtual void	v_FilterFinish(CFX_BinaryBuf& dest_buf) {}
    FX_BOOL			m_bEarlyChange;
    FX_DWORD		m_CodeArray[5021];
    FX_DWORD		m_nCodes;
    FX_DWORD		m_CodeLen;
    FX_DWORD		m_OldCode;
    FX_BYTE			m_LastChar;
    FX_DWORD		m_nLeftBits, m_LeftBits;
    FX_BYTE			m_DecodeStack[4000];
    FX_DWORD		m_StackLen;
    void			AddCode(FX_DWORD prefix_code, FX_BYTE append_char);
    void			DecodeString(FX_DWORD code);
};
class CPDF_PredictorFilter : public CFX_DataFilter
{
public:
    CPDF_PredictorFilter(int predictor, int colors, int bpc, int cols);
    virtual ~CPDF_PredictorFilter();
    virtual	void	v_FilterIn(FX_LPCBYTE src_buf, size_t src_size, CFX_BinaryBuf& dest_buf);
    virtual void	v_FilterFinish(CFX_BinaryBuf& dest_buf) {}
    FX_BOOL			m_bTiff;
    FX_DWORD		m_Pitch, m_Bpp;
    FX_LPBYTE		m_pRefLine, m_pCurLine;
    FX_DWORD		m_iLine, m_LineInSize;
};
class CPDF_AsciiHexFilter : public CFX_DataFilter
{
public:
    CPDF_AsciiHexFilter();
    virtual ~CPDF_AsciiHexFilter() {}
    virtual	void	v_FilterIn(FX_LPCBYTE src_buf, size_t src_size, CFX_BinaryBuf& dest_buf);
    virtual void	v_FilterFinish(CFX_BinaryBuf& dest_buf) {}
    int				m_State;
    int				m_FirstDigit;
};
class CPDF_Ascii85Filter : public CFX_DataFilter
{
public:
    CPDF_Ascii85Filter();
    virtual ~CPDF_Ascii85Filter() {}
    virtual	void	v_FilterIn(FX_LPCBYTE src_buf, size_t src_size, CFX_BinaryBuf& dest_buf);
    virtual void	v_FilterFinish(CFX_BinaryBuf& dest_buf) {}
    int				m_State;
    int				m_CharCount;
    FX_DWORD		m_CurDWord;
};
class CPDF_RunLenFilter : public CFX_DataFilter
{
public:
    CPDF_RunLenFilter();
    virtual ~CPDF_RunLenFilter() {}
    virtual	void	v_FilterIn(FX_LPCBYTE src_buf, size_t src_size, CFX_BinaryBuf& dest_buf);
    virtual void	v_FilterFinish(CFX_BinaryBuf& dest_buf) {}
    virtual void	v_ResetStatistics()
    {
        m_TotalDestSize = 0;
    }
    int				m_State;
    FX_DWORD		m_Count;
private:
    FX_DWORD		m_SrcOffset;
    FX_DWORD		m_TotalDestSize;
};
