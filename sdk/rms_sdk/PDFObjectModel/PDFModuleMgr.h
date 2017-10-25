/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMSSDK_PDFOBJECTMODEL_PDFMODULEMGR_H_
#define RMSSDK_PDFOBJECTMODEL_PDFMODULEMGR_H_

#include "PDFObjectModel.h"
#include "Core.inc"
#include "PDFCreator.h"

namespace rmscore {
namespace pdfobjectmodel {

/**
 * @brief The implementaion class of interface class PDFModuleMgr defined in pdf object model layer.
 * The implementation is to initialize the Foxit core.
 * Please refer to comments of PDFModuleMgr.
 */
class PDFModuleMgrImpl : public PDFModuleMgr
{
public:
    PDFModuleMgrImpl();

    static void RegisterSecurityHandler(const std::string& filterName, std::shared_ptr<PDFSecurityHandler> securityHander);

    virtual  ~PDFModuleMgrImpl();

public:
    CPDF_ModuleMgr* m_pModuleMgr;
    CCodec_ModuleMgr* m_pCodecModule;
    CustomSecurityHandler* m_pCusSecHandler;
};

} // namespace pdfobjectmodel
} // namespace rmscore

#endif // RMSSDK_PDFOBJECTMODEL_PDFMODULEMGR_H_

