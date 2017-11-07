/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_FILE_API_PROTECTOR_H
#define RMS_SDK_FILE_API_PROTECTOR_H

#include <memory>
#include <string>
#include "file_api_structures.h"
#include "api_export.h"

namespace rmscore {
namespace fileapi {

class DLL_PUBLIC_RMS Protector {
public:
  /*!
   * \brief Returns a pointer to an instance of a protector according to the file type.
   * \param fileName The name of the input file
   * \param inputStream The pointer to the input file stream
   * \param outputFileName The expected output file name returned by the method.
   * \return
   */
  static std::unique_ptr<Protector> Create(
      const std::string& fileName,
      std::shared_ptr<std::istream> inputStream,
      std::string& outputFileName);

  /*!
   * \brief ProtectWithTemplate
   * Creates User Policy using the template decriptor in 'options' using the 'userContext'.
   * Encrypts the 'inputStream' provided in 'Create' method and writes the encrypted content
   * to the 'outputStream'.
   * \param userContext Contains user id, authentication and consent callbacks.
   * \param options Contains options for protecting the stream with a template.
   * \param outputStream The stream to which the encrypted content has to be written.
   * \param cancelState
   */
  virtual void ProtectWithTemplate(
      const UserContext& userContext,
      const ProtectWithTemplateOptions& options,
      std::shared_ptr<std::ostream> outputStream,
      std::shared_ptr<std::atomic<bool>> cancelState = nullptr) = 0;

  /*!
   * \brief ProtectWithCustomRights
   * Creates User Policy using the policy decriptor in 'options' using the 'userContext'.
   * Encrypts the 'inputStream' provided in 'Create' method and writes the encrypted content
   * to the 'outputStream'.
   * \param userContext Contains user id, authentication and consent callbacks.
   * \param options Contains options for protecting the stream with custom rights using a policy descriptor.
   * \param outputStream The stream to which the encrypted content has to be written.
   * \param cancelState
   */
  virtual void ProtectWithCustomRights(
      const UserContext& userContext,
      const ProtectWithCustomRightsOptions& options,
      std::shared_ptr<std::ostream> outputStream,
      std::shared_ptr<std::atomic<bool>> cancelState = nullptr) = 0;

  /*!
   * \brief Unprotect
   * Extracts the license from the file and acquires user policy using the 'userContext'
   * Decrypts the 'inputStream' provided in 'Create' method and writes the decryped content
   * to the 'outputStream'.
   * \param userContext Contains user id, authentication and consent callbacks.
   * \param options Contains options about being offline and allowing to cache policy to disk or memory.
   * \param outputStream The stream to which the decrypted content has to be written.
   * \param cancelState
   * \return
   */
  virtual UnprotectResult Unprotect(
      const UserContext& userContext,
      const UnprotectOptions& options,
      std::shared_ptr<std::ostream> outputStream,
      std::shared_ptr<std::atomic<bool>> cancelState = nullptr) = 0;

  /*!
   * \brief Checks whether the 'inputStream' provided in 'Create' method is protected or not.
   * This is done by checking the presence of the publishing license in the stream.
   * \return
   */
  virtual bool IsProtected() const = 0;

  virtual ~Protector() { }

protected:
    Protector() {
    }
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_PROTECTOR_H

