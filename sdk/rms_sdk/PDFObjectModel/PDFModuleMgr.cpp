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

static std::unique_ptr<PDFModuleMgr> g_pdf_module_mgr = nullptr;

bool PDFModuleMgr::Initialize()
{
    if(g_pdf_module_mgr == nullptr)
    {
        g_pdf_module_mgr.reset(new PDFModuleMgrImpl());
    }

    return true;
}

PDFModuleMgrImpl::PDFModuleMgrImpl()
{
    pdf_codec_module_ = CCodec_ModuleMgr::Create();

    CPDF_ModuleMgr::Create();
    CPDF_ModuleMgr::Get()->SetCodecModule(pdf_codec_module_);
    pdf_module_manager_ = CPDF_ModuleMgr::Get();

    custom_security_handler_ = nullptr ;

}

PDFModuleMgrImpl::~PDFModuleMgrImpl()
{
    custom_security_handler_ = nullptr;

    if (pdf_module_manager_ != nullptr)
    {
        pdf_module_manager_->Destroy();
        pdf_module_manager_ = nullptr;
    }

    if (pdf_codec_module_ != nullptr)
    {
        pdf_codec_module_->Destroy();
        pdf_codec_module_ = nullptr;
    }
}

static CPDF_SecurityHandler* CreateCustomerSecurityHandler(void* param)
{
    return (CustomSecurityHandler*)param;
}

void PDFModuleMgrImpl::RegisterSecurityHandler(const std::string& filter_name, std::shared_ptr<PDFSecurityHandler> security_hander)
{
     if(g_pdf_module_mgr != nullptr)
     {
         PDFModuleMgrImpl* pdf_module_mgr = (PDFModuleMgrImpl*)g_pdf_module_mgr.get();
         //core takes over custom_security_handler_
         pdf_module_mgr->custom_security_handler_ = new CustomSecurityHandler(security_hander);
         pdf_module_mgr->pdf_module_manager_->RegisterSecurityHandler(filter_name.c_str(),
                                                                      CreateCustomerSecurityHandler,
                                                                      pdf_module_mgr->custom_security_handler_);

     }
}

} // namespace pdfobjectmodel
} // namespace rmscore
