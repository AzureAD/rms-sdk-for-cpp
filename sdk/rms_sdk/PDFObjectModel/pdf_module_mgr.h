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
#include "core.inc"
#include "pdf_creator.h"

namespace rmscore {
namespace pdfobjectmodel {

/**
 * @brief The implementaion class of interface class PDFModuleMgr defined in pdf object model layer.
 * The implementation is to initialize the Foxit core.
 * Please refer to comments of PDFModuleMgr.
 */
class PDFModuleMgrImpl : public PDFModuleMgr {
 public:
  explicit PDFModuleMgrImpl();

  static void RegisterSecurityHandler(const std::string& filter_name,
                                      std::shared_ptr<PDFSecurityHandler> security_hander);
  virtual  ~PDFModuleMgrImpl();

  void SetSharedSecurityHandler(std::shared_ptr<PDFSecurityHandler> shared_security_hander);
  std::shared_ptr<PDFSecurityHandler> GetSharedSecurityHandler();
 private:
  CPDF_ModuleMgr* pdf_module_manager_;
  CCodec_ModuleMgr* pdf_codec_module_;

  std::shared_ptr<PDFSecurityHandler> shared_security_hander_;
};

} // namespace pdfobjectmodel
} // namespace rmscore

#endif // RMSSDK_PDFOBJECTMODEL_PDFMODULEMGR_H_

