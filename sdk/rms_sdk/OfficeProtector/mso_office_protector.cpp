/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "mso_office_protector.h"
#include <sstream>
#include <vector>
#include "CryptoAPI.h"
#include "RMSExceptions.h"
#include "CommonTypes.h"
#include "ProtectionPolicy.h"
#include "utils.h"
#include "Logger.h"
#include "data_spaces.h"
#include "office_utils.h"
#include "stream_constants.h"

using namespace rmscore::platform::logger;
using namespace rmscore::common;

namespace{
const char kDrmContent[] = "\11DRMContent";
} // namespace

namespace rmscore {
namespace officeprotector {

MsoOfficeProtector::MsoOfficeProtector(
    const std::string& fileName,
    std::shared_ptr<std::istream> inputStream)
  : mFileName(fileName),
    mInputStream(inputStream) {
  utils::CheckStream(mInputStream.get(), "Invalid input stream");
  gsf_init();
}

MsoOfficeProtector::~MsoOfficeProtector() {
}

void MsoOfficeProtector::ProtectWithTemplate(
    const UserContext& userContext,
    const ProtectWithTemplateOptions& options,
    std::shared_ptr<std::ostream> outputStream,
    std::shared_ptr<std::atomic<bool>> cancelState) {
  Logger::Hidden("+MsoOfficeProtector::ProtectWithTemplate");
  if (!outputStream->good()) {
    Logger::Error("Output stream invalid");
    throw exceptions::RMSStreamException("Output stream invalid");
  }

  // Check if the file is protected first. This is done by checking presence of license in the file.
  if (IsProtected()) {
    Logger::Error("File is already protected");
    throw exceptions::RMSOfficeFileException(
        "File is already protected",
        exceptions::RMSOfficeFileException::Reason::AlreadyProtected);
  }

  auto inputFileSize = utils::ValidateAndGetFileSize(
      mInputStream.get(),
      utils::MAX_FILE_SIZE_FOR_ENCRYPT);
  auto userPolicyCreationOptions = officeutils::ConvertToUserPolicyCreationOptionsForOffice(
      options.allowAuditedExtraction,
      options.cryptoOptions);
  // Create User Policy
  mUserPolicy = modernapi::UserPolicy::CreateFromTemplateDescriptor(
      options.templateDescriptor,
      userContext.userId,
      userContext.authenticationCallback,
      userPolicyCreationOptions,
      options.signedAppData,
      cancelState);
  // Creating and using temporary files to store unencrypted content and encrypted content.
  // This is done since LibGsf doesn't support C++ iostreams. In older office format, the unencrypted
  // data is also in Coumpound file format. We also need a temporary file since the encrypted content
  // is a separate CFB storage altogether. Hence, we need 3 temporary files - for input, output and
  // for storing drm content as a separate compound file.
  std::string inputTempFileName = utils::CreateTemporaryFileName(mFileName);
  std::string outputTempFileName = utils::CreateTemporaryFileName("output");
  std::string drmTempFileName = utils::CreateTemporaryFileName("drm");
  // The custom deleters will delete the temporary files once the unique ptrs to the names go out of scope.
  std::unique_ptr<utils::TempFileName, utils::TempFile_deleter> inputTempFile(&inputTempFileName);
  std::unique_ptr<utils::TempFileName, utils::TempFile_deleter> outputTempFile(&outputTempFileName);
  std::unique_ptr<utils::TempFileName, utils::TempFile_deleter> drmTempFile(&drmTempFileName);
  try {
    ProtectInternal(inputTempFileName, outputTempFileName, drmTempFileName, inputFileSize);
    utils::CopyFromFileToOstream(outputTempFileName, outputStream.get());
  } catch (std::exception&) {
    Logger::Hidden("-MsoOfficeProtector::ProtectWithTemplate");
    throw;
  }
  Logger::Hidden("-MsoOfficeProtector::ProtectWithTemplate");
}

void MsoOfficeProtector::ProtectWithCustomRights(
    const UserContext& userContext,
    const ProtectWithCustomRightsOptions& options,
    std::shared_ptr<std::ostream> outputStream,
    std::shared_ptr<std::atomic<bool>> cancelState) {
  Logger::Hidden("+MsoOfficeProtector::ProtectWithCustomRights");
  if (!outputStream->good()) {
    Logger::Error("Output stream invalid");
    throw exceptions::RMSStreamException("Output stream invalid");
  }

  // Check if the file is protected first. This is done by checking presence of license in the file.
  if (IsProtected()) {
    Logger::Error("File is already protected");
    throw exceptions::RMSOfficeFileException(
        "File is already protected",
        exceptions::RMSOfficeFileException::Reason::AlreadyProtected);
  }
  auto inputFileSize = utils::ValidateAndGetFileSize(
      mInputStream.get(),
      utils::MAX_FILE_SIZE_FOR_ENCRYPT);
  auto userPolicyCreationOptions = officeutils::ConvertToUserPolicyCreationOptionsForOffice(
      options.allowAuditedExtraction,
      options.cryptoOptions);
  // Create User Policy
  mUserPolicy = modernapi::UserPolicy::Create(
      const_cast<modernapi::PolicyDescriptor&>(options.policyDescriptor),
      userContext.userId,
      userContext.authenticationCallback,
      userPolicyCreationOptions,
      cancelState);
  // Creating and using temporary files to store unencrypted content and encrypted content.
  // This is done since LibGsf doesn't support C++ iostreams. In older office format, the unencrypted
  // data is also in Coumpound file format. We also need a temporary file since the encrypted content
  // is a separate CFB storage altogether. Hence, we need 3 temporary files - for input, output and
  // for storing drm content as a separate compound file.
  std::string inputTempFileName = utils::CreateTemporaryFileName(mFileName);
  std::string outputTempFileName = utils::CreateTemporaryFileName("output");
  std::string drmTempFileName = utils::CreateTemporaryFileName("drm");
  // The custom deleters will delete the temporary files once the unique ptrs to the names go out of scope.
  std::unique_ptr<utils::TempFileName, utils::TempFile_deleter> inputTempFile(&inputTempFileName);
  std::unique_ptr<utils::TempFileName, utils::TempFile_deleter> outputTempFile(&outputTempFileName);
  std::unique_ptr<utils::TempFileName, utils::TempFile_deleter> drmTempFile(&drmTempFileName);
  try {
    ProtectInternal(inputTempFileName, outputTempFileName, drmTempFileName, inputFileSize);
    utils::CopyFromFileToOstream(outputTempFileName, outputStream.get());
  } catch (std::exception&) {
    Logger::Hidden("-MsoOfficeProtector::ProtectWithCustomRights");
    throw;
  }
  Logger::Hidden("-MsoOfficeProtector::ProtectWithCustomRights");
}

UnprotectResult MsoOfficeProtector::Unprotect(
    const UserContext& userContext,
    const UnprotectOptions& options,
    std::shared_ptr<std::ostream> outputStream,
    std::shared_ptr<std::atomic<bool>> cancelState) {
  Logger::Hidden("+MsoOfficeProtector::UnProtect");
  if (!outputStream->good()) {
    Logger::Error("Output stream invalid");
    throw exceptions::RMSStreamException("Output stream invalid");
  }
  auto inputFileSize = utils::ValidateAndGetFileSize(
      mInputStream.get(),
      utils::MAX_FILE_SIZE_FOR_DECRYPT);
  auto result = UnprotectResult::NORIGHTS;
  // Creating and using temporary file to store encrypted content and license. This is done since
  // LibGsf doesn't support C++ iostreams. We need 3 temporary files - for input, output and for
  // storing drm content as a separate compound file.
  std::string inputTempFileName = utils::CreateTemporaryFileName(mFileName);
  std::string outputTempFileName = utils::CreateTemporaryFileName("output");
  std::string drmTempFileName = utils::CreateTemporaryFileName("drm");
  // The custom deleters will delete the temporary files once the unique ptrs to the names go out of scope.
  std::unique_ptr<utils::TempFileName, utils::TempFile_deleter> inputTempFile(&inputTempFileName);
  std::unique_ptr<utils::TempFileName, utils::TempFile_deleter> outputTempFile(&outputTempFileName);
  std::unique_ptr<utils::TempFileName, utils::TempFile_deleter> drmTempFile(&drmTempFileName);
  try {
    result = UnprotectInternal(
        userContext,
        options,
        inputTempFileName,
        outputTempFileName,
        drmTempFileName,
        inputFileSize,
        cancelState);
    utils::CopyFromFileToOstream(outputTempFileName, outputStream.get());
  } catch (std::exception&) {
    Logger::Hidden("-MetroOfficeProtector::UnProtect");
    throw;
  }
  Logger::Hidden("-MetroOfficeProtector::UnProtect");
  return result;
}

bool MsoOfficeProtector::IsProtected() const {
  Logger::Hidden("+MsoOfficeProtector::IsProtected");
  auto inputFileSize = utils::ValidateAndGetFileSize(
      mInputStream.get(),
      utils::MAX_FILE_SIZE_FOR_DECRYPT);
  std::string inputTempFileName = utils::CreateTemporaryFileName(mFileName);
  std::unique_ptr<utils::TempFileName, utils::TempFile_deleter> inputTempFile(&inputTempFileName);
  bool isProtected = officeutils::IsProtectedInternal(
      mInputStream.get(),
      inputTempFileName,
      inputFileSize,
      false);
  Logger::Hidden("-MsoOfficeProtector::IsProtected");
  return isProtected;
}

void MsoOfficeProtector::ProtectInternal(
    const std::string& inputTempFileName,
    const std::string& outputTempFileName,
    const std::string& drmTempFileName,
    uint64_t inputFileSize) {
  if (mUserPolicy.get() == nullptr) {
    Logger::Error("User Policy creation failed");
    throw exceptions::RMSOfficeFileException(
        "User Policy creation failed.",
        exceptions::RMSOfficeFileException::Reason::BadArguments);
  }
  // Copy from input stream to a temporary file. This file is opened as a Compound File through
  // LibGsf.
  utils::CopyFromIstreamToFile(mInputStream.get(), inputTempFileName, inputFileSize);
  std::unique_ptr<GsfInfile, officeprotector::GsfInfile_deleter> inputStg;  
  try {
    std::unique_ptr<GsfInput, officeprotector::GsfInput_deleter> gsfInputStdIO(
        gsf_input_stdio_new(inputTempFileName.c_str(), nullptr));
    officeutils::CheckGsfInput(gsfInputStdIO.get());
    inputStg.reset(gsf_infile_msole_new(gsfInputStdIO.get(), nullptr));
    officeutils::CheckGsfInfile(inputStg.get());
  } catch (std::exception&) {
    Logger::Hidden("Failed to open file as a valid Compound File");
    throw;
  }
  // For storages, number of children are >0 and for streams, number of children are -1
  auto num_children = gsf_infile_num_children(inputStg.get());
  if (num_children < 0) {
    Logger::Hidden("Empty storage. Nothing to protect");
    throw exceptions::RMSOfficeFileException(
        "The file is invalid",
        exceptions::RMSOfficeFileException::Reason::NotOfficeFile);
  }
  // The file is partially protected. The streams that don't need to be protected are copied as it
  // is to the output Compound File. The ones that need to be protected are copied to another
  // temporary Compound File. Then this file is opened as binary, protected and the protected
  // content is copied to the output Compound File.
  std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> gsfOutputStdIO(
      gsf_output_stdio_new(outputTempFileName.c_str(), nullptr));
  officeutils::CheckGsfOutput(gsfOutputStdIO.get());
  std::unique_ptr<GsfOutfile, officeprotector::GsfOutfile_deleter> outputStg(
      gsf_outfile_msole_new(gsfOutputStdIO.get()));
  {
    officeutils::CheckGsfOutfile(outputStg.get());
    // Create the temporary Compound File which will hold the streams that need to be protected.
    std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> gsfDrmStdIO(
        gsf_output_stdio_new(drmTempFileName.c_str(), nullptr));
    officeutils::CheckGsfOutput(gsfDrmStdIO.get());

    std::unique_ptr<GsfOutfile, officeprotector::GsfOutfile_deleter> drmStg(
        gsf_outfile_msole_new(gsfDrmStdIO.get()));
    officeutils::CheckGsfOutfile(drmStg.get());

    // This is a list of the streams/storages that don't need to be protected.
    auto storageElementsList = GetStorageElementsList();
    auto identifierMap = GetIdentifierMap();
    for (int i = 0; i < num_children; i++)
    {
      std::unique_ptr<GsfInput, officeprotector::GsfInput_deleter> inputChild(
          gsf_infile_child_by_index(inputStg.get(), i));
      std::string childName = gsf_input_name(inputChild.get());
      auto num_children_child = gsf_infile_num_children(GSF_INFILE(inputChild.get()));
      // Some template streams are copied to the protected file. This is done to display an
      // appropriate message in non enlightened applications.
      if (identifierMap.find(childName) != identifierMap.end()) {
        CopyTemplate(outputStg.get(), identifierMap[childName]);
      }

      if (std::find(storageElementsList.begin(), storageElementsList.end(), childName) !=
          storageElementsList.end()) {
        // Copy to the output Compound File as it is
        std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> outputChild(
            gsf_outfile_new_child(outputStg.get(), childName.c_str(), num_children_child > 0));
        auto result = CopyStorage(GSF_INFILE(inputChild.get()), GSF_OUTFILE(outputChild.get()));
      } else {
        // Copy to the temporary Compound File.
        std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> drmChild(
            gsf_outfile_new_child(drmStg.get(), childName.c_str(), num_children_child > 0));
        auto result = CopyStorage(GSF_INFILE(inputChild.get()), GSF_OUTFILE(drmChild.get()));
      }
    }
  }
  {
    // Write Dataspaces
    auto dataSpaces = std::make_shared<officeprotector::DataSpaces>(
        false,
        mUserPolicy->DoesUseDeprecatedAlgorithms());
    auto publishingLicense = mUserPolicy->SerializedPolicy();
    dataSpaces->WriteDataSpaces(outputStg.get(), publishingLicense);
  }
  {
    // Create a stream which will hold the encrypted content
    std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> drmEncryptedStream(
        gsf_outfile_new_child(outputStg.get(), kDrmContent, false));
    // Open the temporary Compound File, which has streams/storages to be protected, as binary.
    std::unique_ptr<FILE, utils::FILE_closer> drmTempFileStream(fopen(drmTempFileName.c_str(), "r+b"));
    auto drmContentSize = utils::ValidateAndGetFileSize(
        drmTempFileStream.get(),
        utils::MAX_FILE_SIZE_FOR_ENCRYPT);
    officeutils::WriteStreamHeader(drmEncryptedStream.get(), drmContentSize);
    EncryptStream(drmTempFileStream.get(), drmEncryptedStream.get(), drmContentSize);
  }
}

UnprotectResult MsoOfficeProtector::UnprotectInternal(
    const UserContext& userContext,
    const UnprotectOptions& options,
    const std::string& inputTempFileName,
    const std::string& outputTempFileName,
    const std::string& drmTempFileName,
    uint64_t inputFileSize,
    std::shared_ptr<std::atomic<bool>> cancelState) {
  // Copy from input stream to a temporary file. This file is opened as a Compound File through
  // LibGsf.
  utils::CopyFromIstreamToFile(mInputStream.get(), inputTempFileName, inputFileSize);
  std::unique_ptr<GsfInfile, officeprotector::GsfInfile_deleter> inputStg; 
  try {
    std::unique_ptr<GsfInput, officeprotector::GsfInput_deleter> gsfInputStdIO(
        gsf_input_stdio_new(inputTempFileName.c_str(), nullptr));
    officeutils::CheckGsfInput(gsfInputStdIO.get());
    inputStg.reset(gsf_infile_msole_new(gsfInputStdIO.get(), nullptr));
    officeutils::CheckGsfInfile(inputStg.get());
  } catch (std::exception&) {
    Logger::Hidden("Failed to open file as a valid Compound File");
    throw;
  }
  ByteArray publishingLicense;
  try {
    // Read the license from the protected file. If this fails, either the file is not protected
    // or has been corrupted.
    auto dataSpaces = std::make_shared<officeprotector::DataSpaces>(false, true);
    dataSpaces->ReadDataSpaces(inputStg.get(), publishingLicense);
  } catch (std::exception&) {
    Logger::Hidden("Failed to read dataspaces");
    throw;
  }
  modernapi::PolicyAcquisitionOptions policyAcquisitionOptions = options.offlineOnly?
      modernapi::PolicyAcquisitionOptions::POL_OfflineOnly :
      modernapi::PolicyAcquisitionOptions::POL_None;
  auto cacheMask = modernapi::RESPONSE_CACHE_NOCACHE;
  if (options.useCache) {
    cacheMask = static_cast<modernapi::ResponseCacheFlags>(
        modernapi::RESPONSE_CACHE_INMEMORY|
        modernapi::RESPONSE_CACHE_ONDISK |
        modernapi::RESPONSE_CACHE_CRYPTED);
  }
  // Acquire User Policy
  auto policyRequest = modernapi::UserPolicy::Acquire(
      publishingLicense,
      userContext.userId,
      userContext.authenticationCallback,
      &userContext.consentCallback,
      policyAcquisitionOptions,
      cacheMask,
      cancelState);
  if (policyRequest->Status != modernapi::GetUserPolicyResultStatus::Success) {
    Logger::Error("UserPolicy::Acquire unsuccessful", policyRequest->Status);
    throw exceptions::RMSOfficeFileException(
        "The file has been corrupted",
        exceptions::RMSOfficeFileException::Reason::BadArguments);
  }
  mUserPolicy = policyRequest->Policy;
  if (mUserPolicy.get() == nullptr) {
    Logger::Error("User Policy acquisition failed");
    throw exceptions::RMSOfficeFileException(
        "User Policy acquisition failed.",
        exceptions::RMSOfficeFileException::Reason::BadArguments);
  }

  if (!mUserPolicy->DoesUseDeprecatedAlgorithms()) {
    throw exceptions::RMSLogicException(
        exceptions::RMSException::ErrorTypes::NotSupported,
        "CBC Decryption with Office files is not yet supported");
  }
  // The file is partially protected. The storages/streams that aren't protected are copied as it is
  // to the output Compound File. The ones that are protected are a part of the '\11DRMContent'
  // stream. This stream is decrypted and copied to another temporary Compound File. Then this file
  // is opened as a Compound File, and all storages/streams are copied to the output Compound File.
  std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> gsfOutputStdIO(
      gsf_output_stdio_new(outputTempFileName.c_str(), nullptr));
  officeutils::CheckGsfOutput(gsfOutputStdIO.get());
  std::unique_ptr<GsfOutfile, officeprotector::GsfOutfile_deleter> outputStg(
      gsf_outfile_msole_new(gsfOutputStdIO.get()));
  {
    officeutils::CheckGsfOutfile(outputStg.get());
    std::unique_ptr<FILE, utils::FILE_closer> drmTempFileStream(fopen(drmTempFileName.c_str(), "w+b"));
    std::unique_ptr<GsfInput, officeprotector::GsfInput_deleter> drmEncryptedStream(
        gsf_infile_child_by_name(inputStg.get(), kDrmContent));
    uint64_t originalFileSize = 0;
    officeutils::ReadStreamHeader(drmEncryptedStream.get(), originalFileSize);
    // Decrypt the encrypted stream and copy it to the temporary Compound File.
    DecryptStream(drmTempFileStream.get(), drmEncryptedStream.get(), originalFileSize);
    auto num_children = gsf_infile_num_children(inputStg.get());
    // This is a list of the streams/storages that aren't protected and will be copied as it is.
    auto storageElementsList = GetStorageElementsList();
    for (int i = 0; i < num_children; i++) {
      std::unique_ptr<GsfInput, officeprotector::GsfInput_deleter> inputChild(
          gsf_infile_child_by_index(inputStg.get(), i));
      std::string childName = gsf_input_name(inputChild.get());
      if (std::find(storageElementsList.begin(), storageElementsList.end(), childName) !=
          storageElementsList.end()) {
        auto num_children_child = gsf_infile_num_children(GSF_INFILE(inputChild.get()));
        std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> outputChild(
            gsf_outfile_new_child(outputStg.get(), childName.c_str(),
                                    num_children_child > 0));
        auto result = CopyStorage(GSF_INFILE(inputChild.get()), GSF_OUTFILE(outputChild.get()));
      }
    }
  }
  // Open the temporary file as a Compound File and copy all storages/streams to the output
  // Compound File
  std::unique_ptr<GsfInput, officeprotector::GsfInput_deleter> gsfDrmStdIO(
      gsf_input_stdio_new(drmTempFileName.c_str(), nullptr));
  officeutils::CheckGsfInput(gsfDrmStdIO.get());
  std::unique_ptr<GsfInfile, officeprotector::GsfInfile_deleter> drmStg(
      gsf_infile_msole_new(gsfDrmStdIO.get(), nullptr));
  officeutils::CheckGsfInfile(drmStg.get());
  CopyStorage(drmStg.get(), outputStg.get());
  return (UnprotectResult)policyRequest->Status;
}

void MsoOfficeProtector::EncryptStream(
    FILE* drmStream,
    GsfOutput* drmEncryptedStream,
    uint64_t inputFileSize) {
  officeutils::CheckGsfOutput(drmEncryptedStream);
  auto cryptoProvider = mUserPolicy->GetImpl()->GetCryptoProvider();
  mBlockSize = cryptoProvider->GetBlockSize();
  std::vector<uint8_t> buffer(utils::BUF_SIZE_BYTES);
  uint64_t readPosition  = 0;
  bool isECB = mUserPolicy->DoesUseDeprecatedAlgorithms();
  // Total size represents the size of the encrypted content. in case of ECB, padding is added in
  // the end to round the size up to the block size using the bitmask.
  uint64_t totalSize = isECB? ((inputFileSize + mBlockSize - 1) & ~(mBlockSize - 1)) :
                              inputFileSize;
  while (totalSize - readPosition > 0) {
    uint64_t offsetRead  = readPosition;
    uint64_t toProcess   = std::min(utils::BUF_SIZE_BYTES, totalSize - readPosition);
    readPosition  += toProcess;

    // Using String Streams since the underlying protection APIs use streams instead of buffers.
    // Hence we create string streams and create protected stream from it.
    // TODO: modify this logic once we have buffer based APIs.
    auto sstream = std::make_shared<std::stringstream>();
    std::shared_ptr<std::iostream> iosstream = sstream;
    auto sharedStringStream = rmscrypto::api::CreateStreamFromStdStream(iosstream);
    auto pStream = officeutils::CreateProtectedStream(sharedStringStream, 0, cryptoProvider);

    fseek(drmStream, offsetRead, SEEK_SET);
    fread(&buffer[0], toProcess, 1, drmStream);
    pStream->WriteAsync(buffer.data(), toProcess, 0, std::launch::deferred).get();
    pStream->Flush();
    // get the encrypted content as a string from the string stream.
    std::string encryptedData = sstream->str();
    auto encryptedDataLen = encryptedData.length();
    officeutils::GsfWrite(
        drmEncryptedStream,
        encryptedDataLen,
        reinterpret_cast<const uint8_t*>(encryptedData.data()));
  }
}

void MsoOfficeProtector::DecryptStream(
    FILE* drmStream,
    GsfInput* drmEncryptedStream,
    uint64_t originalFileSize) {
  officeutils::CheckGsfInput(drmEncryptedStream);
  auto cryptoProvider = mUserPolicy->GetImpl()->GetCryptoProvider();
  mBlockSize = cryptoProvider->GetBlockSize();
  std::vector<uint8_t> buffer(utils::BUF_SIZE_BYTES);
  uint64_t readPosition  = 0;
  uint64_t writePosition = 0;
  // Total size is the size of the encrypted content. The size of the unencrypted content is
  // written in the stream header.
  uint64_t totalSize = (uint64_t)gsf_input_size(drmEncryptedStream) - sizeof(uint64_t);
  while (totalSize - readPosition > 0) {
    uint64_t offsetWrite = writePosition;
    uint64_t toProcess   = std::min(utils::BUF_SIZE_BYTES, totalSize - readPosition);
    uint64_t originalRemaining = std::min(utils::BUF_SIZE_BYTES, originalFileSize - readPosition);
    readPosition  += toProcess;
    writePosition += toProcess;

    officeutils::GsfRead(drmEncryptedStream, toProcess, &buffer[0]);
    std::string encryptedData(reinterpret_cast<const char *>(buffer.data()), toProcess);
    // Using String Streams since the underlying protection APIs use streams instead of buffers.
    // Hence we create string stream with encrypted content and create protected stream from it
    // to read.
    // TODO: modify this logic once we have buffer based APIs.
    auto sstream = std::make_shared<std::stringstream>();
    sstream->str(encryptedData);
    std::shared_ptr<std::iostream> iosstream = sstream;
    auto sharedStringStream = rmscrypto::api::CreateStreamFromStdStream(iosstream);
    auto pStream = officeutils::CreateProtectedStream(
        sharedStringStream,
        encryptedData.length(),
        cryptoProvider);
    pStream->ReadAsync(&buffer[0], toProcess, 0, std::launch::deferred).get();

    fseek(drmStream, offsetWrite, SEEK_SET);
    // Write to standard output stream.
    fwrite(reinterpret_cast<const char *>(buffer.data()), originalRemaining, 1, drmStream );
  }
  fflush(drmStream);
}

bool MsoOfficeProtector::CopyStorage(GsfInfile* src, GsfOutfile* dest) {
  officeutils::CheckGsfInfile(src);
  officeutils::CheckGsfOutfile(dest);
  // For storages, number of children are >0 and for streams, number of children are -1
  auto childCount = gsf_infile_num_children(src);
  if (childCount == -1) {
    // It's a stream
    return CopyStream(GSF_INPUT(src), GSF_OUTPUT(dest));
  }
  bool result = true;
  for (int i = 0; i < childCount; i++) {
    std::unique_ptr<GsfInput, officeprotector::GsfInput_deleter> srcChild(
        gsf_infile_child_by_index(src, i));
    officeutils::CheckGsfInput(srcChild.get());
    std::string childName = gsf_input_name(srcChild.get());
    if (gsf_infile_num_children(GSF_INFILE(srcChild.get())) < 0) {
      // Child is a stream
      std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> destChild(
          gsf_outfile_new_child(dest, childName.c_str(), false));
      officeutils::CheckGsfOutput(destChild.get());
      result = result & CopyStream(srcChild.get(), destChild.get());
    } else {
      // Child is a storage
      std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> destChild(
          gsf_outfile_new_child(dest, childName.c_str(), true));
      officeutils::CheckGsfOutput(destChild.get());
      result = result & CopyStorage(GSF_INFILE(srcChild.get()), GSF_OUTFILE(destChild.get()));
    }
  }
  return result;
}

bool MsoOfficeProtector::CopyStream(GsfInput* src, GsfOutput* dest) {
  officeutils::CheckGsfInput(src);
  officeutils::CheckGsfOutput(dest);
  officeutils::GsfInputSeek(src, 0, G_SEEK_SET);
  officeutils::GsfOutputSeek(dest, 0, G_SEEK_SET);
  return gsf_input_copy(src, dest);
}

void MsoOfficeProtector::CopyTemplate(GsfOutfile* dest, uint32_t identifier) {
  officeutils::CheckGsfOutfile(dest);
  switch (identifier) {
    case 0: {
    // Word File
      {
        std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> tableStream(
            gsf_outfile_new_child(dest, "1Table", false));
        officeutils::CheckGsfOutput(tableStream.get());
        officeutils::GsfWrite(tableStream.get(), sizeof(kTable), kTable);
      }
      {
        std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> wordStream(
            gsf_outfile_new_child(dest, "WordDocument", false));
        officeutils::CheckGsfOutput(wordStream.get());
        officeutils::GsfWrite(wordStream.get(), sizeof(kWordDocument), kWordDocument);
      }
      break;
    }
    case 1: {
    // PowerPoint File
      {
        std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> userStream(
            gsf_outfile_new_child(dest, "Current User", false));
        officeutils::CheckGsfOutput(userStream.get());
        officeutils::GsfWrite(userStream.get(), sizeof(kCurrentUser), kCurrentUser);
      }
      {
        std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> pptStream(
            gsf_outfile_new_child(dest, "PowerPoint Document", false));
        officeutils::CheckGsfOutput(pptStream.get());
        officeutils::GsfWrite(pptStream.get(), sizeof(kPowerPointDocument), kPowerPointDocument);
      }
      break;
    }
    case 2: {
    // Excel File
      {
        std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> excelStream(
            gsf_outfile_new_child(dest, "Workbook", false));
        officeutils::CheckGsfOutput(excelStream.get());
        officeutils::GsfWrite(excelStream.get(), sizeof(kWorkbook), kWorkbook);
      }
      break;
    }
    default: {
      break;
    }
  }
}

const std::vector<std::string>& MsoOfficeProtector::GetStorageElementsList() {
  static std::vector<std::string> storageElementsList = {"_signatures", "\1CompObj", "Macros",
                                                         "_VBA_PROJECT_CUR", "\5SummaryInformation",
                                                         "\5DocumentSummaryInformation",
                                                         "MsoDataStore"};
  return storageElementsList;
}

const std::map<std::string, uint32_t>& MsoOfficeProtector::GetIdentifierMap() {
  static std::map<std::string, uint32_t> identifierMap = {{"WordDocument", 0},
                                                          {"PowerPoint Document", 1},
                                                          {"Workbook", 2}};
  return identifierMap;
}

} // namespace officeprotector
} // namespace rmscore
