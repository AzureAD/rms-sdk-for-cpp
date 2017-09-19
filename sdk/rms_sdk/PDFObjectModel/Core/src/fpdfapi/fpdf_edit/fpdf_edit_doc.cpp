/*
 * ======================================================================
 * Copyright (c) Foxit Software, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 *======================================================================
 */

#include "../../../include/fpdfapi/fpdf_parser.h"
#include "../../../include/fpdfapi/fpdf_module.h"
#include <limits.h>
CPDF_Document::CPDF_Document() : CPDF_IndirectObjects(NULL)
{
    m_pRootDict = NULL;
    m_pInfoDict = NULL;
    m_bLinearized = FALSE;
    m_bMetaDataFlate  = FALSE;
    m_FileVersion = 0;
}
#if (_FXM_PLATFORM_  == _FXM_PLATFORM_APPLE_)
#define BUFSIZE 1024
#endif
void CPDF_IndirectObjects::DeleteIndirectObject(FX_DWORD objnum)
{
    FX_LPVOID value;
    if (m_IndirectObjs.Lookup((FX_LPVOID)(FX_UINTPTR)objnum, value)) {
        ((CPDF_Object*)value)->m_ObjNum = -1;
    }
    if (m_pParser) {
        m_pParser->DeleteIndirectObject(objnum);
    }
}
