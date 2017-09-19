/*
 * ======================================================================
 * Copyright (c) Foxit Software, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 *======================================================================
 */

#include "../../include/fxcrt/fx_basic.h"
#include "plex.h"
CFX_Plex* CFX_Plex::Create(CFX_Plex*& pHead, FX_DWORD nMax, FX_DWORD cbElement)
{
    CFX_Plex* p = (CFX_Plex*)FX_Alloc(FX_BYTE, sizeof(CFX_Plex) + nMax * cbElement);
    if (!p) {
        return NULL;
    }
    p->pNext = pHead;
    pHead = p;
    return p;
}
void CFX_Plex::FreeDataChain()
{
    CFX_Plex* p = this;
    while (p != NULL) {
        FX_BYTE* bytes = (FX_BYTE*)p;
        CFX_Plex* pNext = p->pNext;
        FX_Free(bytes);
        p = pNext;
    }
}
