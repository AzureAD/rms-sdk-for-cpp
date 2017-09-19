/*
 * ======================================================================
 * Copyright (c) Foxit Software, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 *======================================================================
 */

#ifndef _FPDF_DOC_H_
#define _FPDF_DOC_H_
#ifndef _FPDF_PARSER_
#include "../fpdfapi/fpdf_parser.h"
#endif
class CPDF_NameTree;
class CPDF_FileSpec;
class CPDF_NameTree : public CFX_Object
{
public:

    CPDF_NameTree(CPDF_Dictionary* pParent, FX_BSTR sCategory);

    int					SetValue(CPDF_Document* pDoc, const CFX_ByteString& csName, CPDF_Object* pValue);

    int					GetCount() const;


    CPDF_Dictionary*	GetRoot() const
    {
        return m_pRoot;
    }

protected:
    CPDF_Dictionary*		m_pParent;

    CPDF_Dictionary*		m_pRoot;
    CFX_ByteString			m_bsCategory;
};
class CPDF_FileSpec : public CFX_Object
{
public:

    CPDF_FileSpec();

    CPDF_FileSpec(CPDF_Object *pObj)
    {
        m_pObj = pObj;
    }

    operator CPDF_Object*() const
    {
        return m_pObj;
    }

    FX_BOOL			IsURL() const;

    FX_BOOL			GetFileName(CFX_WideString &wsFileName) const;

    CPDF_Stream*	GetFileStream() const;

    void			SetFileName(FX_WSTR wsFileName, FX_BOOL bURL = FALSE);

    void			SetEmbeddedFile(CPDF_IndirectObjects *pDocument, IFX_FileRead*pFile, FX_WSTR file_path);
protected:

    CPDF_Object		*m_pObj;
};
#endif
