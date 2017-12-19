#include "../../include/fxcrt/fx_basic.h"
void CFX_UTF8Decoder::Clear()
{
    m_Buffer.Clear();
    m_PendingBytes = 0;
}
void CFX_UTF8Decoder::AppendChar(FX_DWORD ch)
{
    m_Buffer.AppendChar((FX_WCHAR)ch);
}
void CFX_UTF8Decoder::Input(FX_BYTE byte)
{
    if (byte < 0x80) {
        m_PendingBytes = 0;
        m_Buffer.AppendChar(byte);
    } else if (byte < 0xc0) {
        if (m_PendingBytes == 0) {
            return;
        }
        m_PendingBytes --;
        m_PendingChar |= (byte & 0x3f) << (m_PendingBytes * 6);
        if (m_PendingBytes == 0) {
            if(0 == m_PendingChar) {
                AppendChar(0xFFFD);
            } else {
                AppendChar(m_PendingChar);
            }
        }
    } else if (byte < 0xe0) {
        m_PendingBytes = 1;
        m_PendingChar = (byte & 0x1f) << 6;
    } else if (byte < 0xf0) {
        m_PendingBytes = 2;
        m_PendingChar = (byte & 0x0f) << 12;
    } else if (byte < 0xf8) {
        m_PendingBytes = 3;
        m_PendingChar = (byte & 0x07) << 18;
    } else if (byte < 0xfc) {
        m_PendingBytes = 4;
        m_PendingChar = (byte & 0x03) << 24;
    } else if (byte < 0xfe) {
        m_PendingBytes = 5;
        m_PendingChar = (byte & 0x01) << 30;
    }
}
void CFX_UTF8Encoder::Input(FX_WCHAR unicode)
{
    if ((FX_DWORD)unicode < 0x80) {
        m_Buffer.AppendChar(unicode);
    } else {
        if ((FX_DWORD)unicode >= 0x80000000) {
            return;
        }
        int nbytes = 0;
        if ((FX_DWORD)unicode < 0x800) {
            nbytes = 2;
        } else if ((FX_DWORD)unicode < 0x10000) {
            nbytes = 3;
        } else if ((FX_DWORD)unicode < 0x200000) {
            nbytes = 4;
        } else if ((FX_DWORD)unicode < 0x4000000) {
            nbytes = 5;
        } else {
            nbytes = 6;
        }
        static FX_BYTE prefix[] = {0xc0, 0xe0, 0xf0, 0xf8, 0xfc};
        int order = 1 << ((nbytes - 1) * 6);
        int code = unicode;
        m_Buffer.AppendChar(prefix[nbytes - 2] | (code / order));
        for (int i = 0; i < nbytes - 1; i ++) {
            code = code % order;
            order >>= 6;
            m_Buffer.AppendChar(0x80 | (code / order));
        }
    }
}
CFX_ByteString FX_UTF8Encode(FX_LPCWSTR pwsStr, FX_STRSIZE len)
{
    FXSYS_assert(pwsStr != NULL);
    if (len < 0) {
        len = (FX_STRSIZE)FXSYS_wcslen(pwsStr);
    }
    CFX_UTF8Encoder encoder;
    while (len -- > 0) {
        encoder.Input(*pwsStr ++);
    }
    return encoder.GetResult();
}
