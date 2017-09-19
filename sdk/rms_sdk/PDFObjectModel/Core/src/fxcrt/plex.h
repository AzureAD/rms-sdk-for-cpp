/*
 * ======================================================================
 * Copyright (c) Foxit Software, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 *======================================================================
 */

struct CFX_Plex {
    CFX_Plex* pNext;
    void* data()
    {
        return this + 1;
    }
    static CFX_Plex* Create(CFX_Plex*& head, FX_DWORD nMax, FX_DWORD cbElement);
    void FreeDataChain();
};
