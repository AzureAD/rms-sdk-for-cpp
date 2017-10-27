/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "PDFModuleMgr.h"

namespace rmscore {
namespace pdfobjectmodel {

static std::unique_ptr<PDFModuleMgr> g_pdfModuleMgr = nullptr;

bool PDFModuleMgr::Initialize()
{
    if(g_pdfModuleMgr == nullptr)
    {
        g_pdfModuleMgr.reset(new PDFModuleMgrImpl());
    }

    return true;
}

PDFModuleMgrImpl::PDFModuleMgrImpl()
{
    m_pCodecModule = CCodec_ModuleMgr::Create();

    CPDF_ModuleMgr::Create();
    CPDF_ModuleMgr::Get()->SetCodecModule(m_pCodecModule);
    m_pModuleMgr = CPDF_ModuleMgr::Get();

    m_pCusSecHandler = nullptr ;

}

PDFModuleMgrImpl::~PDFModuleMgrImpl()
{
    m_pCusSecHandler = nullptr;

    if (m_pModuleMgr != nullptr)
    {
        m_pModuleMgr->Destroy();
        m_pModuleMgr = nullptr;
    }

    if (m_pCodecModule != nullptr)
    {
        m_pCodecModule->Destroy();
        m_pCodecModule = nullptr;
    }
}

static CPDF_SecurityHandler* CreateCustomerSecurityHandler(void* param)
{
    return (CustomSecurityHandler*)param;
}

void PDFModuleMgrImpl::RegisterSecurityHandler(const std::string& filter_name, std::shared_ptr<PDFSecurityHandler> security_hander)
{
     if(g_pdfModuleMgr != nullptr)
     {
         PDFModuleMgrImpl* pMgr = (PDFModuleMgrImpl*)g_pdfModuleMgr.get();
         //core takes over m_pCusSecHandler
         pMgr->m_pCusSecHandler = new CustomSecurityHandler(security_hander);
         pMgr->m_pModuleMgr->RegisterSecurityHandler(filter_name.c_str(), CreateCustomerSecurityHandler, pMgr->m_pCusSecHandler);

     }
}

} // namespace pdfobjectmodel
} // namespace rmscore
