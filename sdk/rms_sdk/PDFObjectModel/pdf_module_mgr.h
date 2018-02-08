/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMSSDK_PDFOBJECTMODEL_PDFMODULEMGR_H_
#define RMSSDK_PDFOBJECTMODEL_PDFMODULEMGR_H_

#include "pdf_object_model.h"
#include "core.h"
#include "pdf_creator.h"

namespace rmscore {
namespace pdfobjectmodel {

/**
 * @brief The implementation class of interface class PDFModuleMgr defined in pdf object model layer.
 * The implementation is to initialize the Foxit core.
 * Please refer to comments of PDFModuleMgr.
 */
class PDFModuleMgrImpl : public PDFModuleMgr {
 public:
  static PDFModuleMgrImpl& Instance() {
    static PDFModuleMgrImpl instance_;
    return instance_;
  }

  static void RegisterSecurityHandler(const std::string& filter_name,
                                      std::shared_ptr<PDFSecurityHandler> security_handler);

  static void UnRegisterSecurityHandler(const std::string& filter_name);

  void SetSharedSecurityHandler(std::shared_ptr<PDFSecurityHandler> shared_security_handler);
  std::shared_ptr<PDFSecurityHandler> GetSharedSecurityHandler();
 private:
  explicit PDFModuleMgrImpl();
  ~PDFModuleMgrImpl();
  PDFModuleMgrImpl(const PDFModuleMgrImpl&) = delete;
  PDFModuleMgrImpl& operator=(const PDFModuleMgrImpl&) = delete;
  PDFModuleMgrImpl(PDFModuleMgrImpl&&) = delete;
  PDFModuleMgrImpl& operator=(PDFModuleMgrImpl&&) = delete;

  CPDF_ModuleMgr* pdf_module_manager_;
  CCodec_ModuleMgr* pdf_codec_module_;

  std::shared_ptr<PDFSecurityHandler> shared_security_handler_;
};

} // namespace pdfobjectmodel
} // namespace rmscore

#endif // RMSSDK_PDFOBJECTMODEL_PDFMODULEMGR_H_

