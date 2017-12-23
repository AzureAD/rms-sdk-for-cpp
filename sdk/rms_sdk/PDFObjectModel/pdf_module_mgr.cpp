/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "pdf_module_mgr.h"

namespace rmscore {
namespace pdfobjectmodel {

bool PDFModuleMgr::Initialize() {
  PDFModuleMgrImpl::Instance();
  return true;
}

PDFModuleMgrImpl::PDFModuleMgrImpl() {
  pdf_codec_module_ = CCodec_ModuleMgr::Create();

  CPDF_ModuleMgr::Create();
  CPDF_ModuleMgr::Get()->SetCodecModule(pdf_codec_module_);
  pdf_module_manager_ = CPDF_ModuleMgr::Get();

  shared_security_hander_ = nullptr;
}

PDFModuleMgrImpl::~PDFModuleMgrImpl() {

  if (pdf_module_manager_ != nullptr) {
    pdf_module_manager_->Destroy();
    pdf_module_manager_ = nullptr;
  }

  if (pdf_codec_module_ != nullptr) {
    pdf_codec_module_->Destroy();
    pdf_codec_module_ = nullptr;
  }
}

void PDFModuleMgrImpl::SetSharedSecurityHandler(std::shared_ptr<PDFSecurityHandler> shared_security_hander) {
  if (shared_security_hander_) {
    shared_security_hander_.reset();
    shared_security_hander_ = nullptr;
  }
  shared_security_hander_ = shared_security_hander;
}
std::shared_ptr<PDFSecurityHandler> PDFModuleMgrImpl::GetSharedSecurityHandler() {
  return shared_security_hander_;
}

static CPDF_SecurityHandler* CreateCustomerSecurityHandler(void* param) {
  FX_UNREFERENCED_PARAMETER(param);
  PDFModuleMgrImpl* pdf_module_mgr = &PDFModuleMgrImpl::Instance();
  std::shared_ptr<PDFSecurityHandler> security_hander = pdf_module_mgr->GetSharedSecurityHandler();
  //core takes over custom_security_handler
  CustomSecurityHandler* custom_security_handler = new CustomSecurityHandler(security_hander);
  return custom_security_handler;
}

void PDFModuleMgrImpl::RegisterSecurityHandler(const std::string& filter_name, std::shared_ptr<PDFSecurityHandler> security_hander) {
  PDFModuleMgrImpl* pdf_module_mgr = &PDFModuleMgrImpl::Instance();
    
  pdf_module_mgr->SetSharedSecurityHandler(security_hander);
  pdf_module_mgr->pdf_module_manager_->RegisterSecurityHandler(
      filter_name.c_str(),
      CreateCustomerSecurityHandler,
      nullptr);
}

void PDFModuleMgrImpl::UnRegisterSecurityHandler(const std::string& filter_name) {
  PDFModuleMgrImpl* pdf_module_mgr = &PDFModuleMgrImpl::Instance();
  pdf_module_mgr->SetSharedSecurityHandler(nullptr);
  pdf_module_mgr->pdf_module_manager_->RegisterSecurityHandler(
      filter_name.c_str(),
      nullptr,
      nullptr);
}

} // namespace pdfobjectmodel
} // namespace rmscore
