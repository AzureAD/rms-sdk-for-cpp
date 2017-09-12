/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "protector_selector.h"
#include "RMSExceptions.h"
#include "Logger.h"

using namespace rmscore::platform::logger;

namespace rmscore {
namespace fileapi {

ProtectorSelector::ProtectorSelector(const std::string& fileName)
    : mCurrentFileExtension(""),
      mPType(ProtectorType::PFILE) {
  Compute(fileName);
}

ProtectorType ProtectorSelector::GetProtectorType() {
  return mPType;
}

std::string ProtectorSelector::GetCurrentFileExtension() {
  return mCurrentFileExtension;
}

std::string ProtectorSelector::GetOutputFileName() {
  return mNewFileName;
}

std::map<std::string, std::pair<ProtectorType, std::string>> ProtectorSelector::Init() {
  // Map of file extension to a pair of appropriate protector type and new file extension.
  static std::map<std::string, std::pair<ProtectorType, std::string>> protectorExtensionsMap = {
    {".docm", std::make_pair(ProtectorType::OPC, ".docm")},
    {".docx", std::make_pair(ProtectorType::OPC, ".docx")},
    {".dotm", std::make_pair(ProtectorType::OPC, ".dotm")},
    {".dotx", std::make_pair(ProtectorType::OPC, ".dotx")},
    {".xlam", std::make_pair(ProtectorType::OPC, ".xlam")},
    {".xlsb", std::make_pair(ProtectorType::OPC, ".xlsb")},
    {".xlsm", std::make_pair(ProtectorType::OPC, ".xlsm")},
    {".xlsx", std::make_pair(ProtectorType::OPC, ".xlsx")},
    {".xltx", std::make_pair(ProtectorType::OPC, ".xltx")},
    {".xps", std::make_pair(ProtectorType::OPC, ".xps")},
    {".potm", std::make_pair(ProtectorType::OPC, ".potm")},
    {".potx", std::make_pair(ProtectorType::OPC, ".potx")},
    {".ppsx", std::make_pair(ProtectorType::OPC, ".ppsx")},
    {".ppsm", std::make_pair(ProtectorType::OPC, ".ppsm")},
    {".pptm", std::make_pair(ProtectorType::OPC, ".pptm")},
    {".pptx", std::make_pair(ProtectorType::OPC, ".pptx")},
    {".thmx", std::make_pair(ProtectorType::OPC, ".thmx")},
    {".vsdx", std::make_pair(ProtectorType::OPC, ".vsdx")},
    {".vsdm", std::make_pair(ProtectorType::OPC, ".vsdm")},
    {".vssx", std::make_pair(ProtectorType::OPC, ".vssx")},
    {".vssm", std::make_pair(ProtectorType::OPC, ".vssm")},
    {".vstx", std::make_pair(ProtectorType::OPC, ".vstx")},
    {".vstm", std::make_pair(ProtectorType::OPC, ".vstm")},
    {".jt", std::make_pair(ProtectorType::PSTAR, ".pjt")},
    {".txt", std::make_pair(ProtectorType::PSTAR, ".ptxt")},
    {".xml", std::make_pair(ProtectorType::PSTAR, ".pxml")},
    {".jpg", std::make_pair(ProtectorType::PSTAR, ".pjpg")},
    {".jpeg", std::make_pair(ProtectorType::PSTAR, ".pjpeg")},
    {".png", std::make_pair(ProtectorType::PSTAR, ".ppng")},
    {".tif", std::make_pair(ProtectorType::PSTAR, ".ptif")},
    {".tiff", std::make_pair(ProtectorType::PSTAR, ".ptiff")},
    {".bmp", std::make_pair(ProtectorType::PSTAR, ".pbmp")},
    {".gif", std::make_pair(ProtectorType::PSTAR, ".pgif")},
    {".jpe", std::make_pair(ProtectorType::PSTAR, ".pjpe")},
    {".jfif", std::make_pair(ProtectorType::PSTAR, ".pjfif")},
    {".jif", std::make_pair(ProtectorType::PSTAR, ".pjif")},
    {".pdf", std::make_pair(ProtectorType::PSTAR, ".ppdf")},
    {".doc", std::make_pair(ProtectorType::MSO, ".doc")},
    {".dot", std::make_pair(ProtectorType::MSO, ".dot")},
    {".xla", std::make_pair(ProtectorType::MSO, ".xla")},
    {".xls", std::make_pair(ProtectorType::MSO, ".xls")},
    {".xlt", std::make_pair(ProtectorType::MSO, ".xlt")},
    {".pps", std::make_pair(ProtectorType::MSO, ".pps")},
    {".ppt", std::make_pair(ProtectorType::MSO, ".ppt")},
    {".pot", std::make_pair(ProtectorType::MSO, ".pot")}};

  return protectorExtensionsMap;
}

const std::map<std::string, std::pair<ProtectorType, std::string>>&
ProtectorSelector::GetProtectorExtensionsMap() {
  static auto protectorExtensionsMap = Init();
  return protectorExtensionsMap;
}

void ProtectorSelector::Compute(const std::string& fileName) {
  if (fileName.empty()) {
    throw exceptions::RMSInvalidArgumentException("The filename is empty");
  }
  auto pos = fileName.find_last_of('.');
  if (pos == std::string::npos || pos == fileName.length()-1) {
    // Default to Pfile protection
    // TODO: Add tests to check this case
    mNewFileName = fileName + ".pfile";
    return;
  }
  std::string ext = fileName.substr(pos);
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
  mCurrentFileExtension = ext;
  if (ext == ".pfile") {
    mNewFileName = fileName.substr(0, pos);
    return;
  }
  auto& protectorExtensionsMap = GetProtectorExtensionsMap();
  auto it = protectorExtensionsMap.find(ext);
  if (it != protectorExtensionsMap.end()) {
    // Key present
    mPType = it->second.first;
  }

  if (mPType != ProtectorType::PFILE && mPType != ProtectorType::PSTAR) {
    //Native protector
    mNewFileName = fileName;
    return;
  }
  auto ppos = ext.find_first_of('p');
  if (mPType == ProtectorType::PSTAR) {
    // PStar extension protection
    mNewFileName = fileName.substr(0, pos) + it->second.second;
    return;
  } else if (ppos != std::string::npos) {
    // Possible PStar extension unprotection
    auto pStarExtension = "." + ext.substr(ppos + 1);
    it = protectorExtensionsMap.find(pStarExtension);
    if (it != protectorExtensionsMap.end() && it->second.first == ProtectorType::PSTAR) {
      // PStar extension unprotection
      mPType = ProtectorType::PSTAR;
      mNewFileName = fileName.substr(0, pos) + it->first;
      return;
    }
  }
  // Pfile protection
  mNewFileName = fileName + ".pfile";
}

} // namespace fileapi
} // namespace rmscore
