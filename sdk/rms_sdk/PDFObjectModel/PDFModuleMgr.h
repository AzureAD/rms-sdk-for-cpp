/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_PDF_MODULE_MGR_H
#define RMS_SDK_PDF_MODULE_MGR_H

#include "PDFObjectModel.h"
#include "CoreInc.h"
#include "PDFCreator.h"

namespace rmscore {
namespace pdfobjectmodel {

class PDFModuleMgrImpl : public PDFModuleMgr
{
public:
    PDFModuleMgrImpl();

    static void RegisterSecurityHandler(const std::string& filterName, PDFSecurityHandler* securityHander);

    virtual  ~PDFModuleMgrImpl();

public:
    CPDF_ModuleMgr* m_pModuleMgr;
    CCodec_ModuleMgr* m_pCodecModule;
    CustomSecurityHandler* m_pCusSecHandler;
};

} // namespace pdfobjectmodel
} // namespace rmscore

#endif // RMS_SDK_PDF_MODULE_MGR_H

