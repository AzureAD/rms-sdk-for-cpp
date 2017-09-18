/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_FILE_API_PROTECTORSELECTOR_H
#define RMS_SDK_FILE_API_PROTECTORSELECTOR_H

#include <map>
#include <string>

namespace rmscore {
namespace fileapi {

enum class ProtectorType {
  PFILE = 0,
  PSTAR = 1,
  OPC = 2,
  MSO = 3,
  PDF = 4,
};

class ProtectorSelector {
public:
  ProtectorSelector(const std::string& fileName);
  ProtectorType GetProtectorType();
  std::string GetCurrentFileExtension();
  std::string GetOutputFileName();

private:
  static std::map<std::string, std::pair<ProtectorType, std::string>> Init();
  static const std::map<std::string, std::pair<ProtectorType, std::string>>& GetProtectorExtensionsMap();
  void Compute(const std::string& fileName);
  std::string mCurrentFileExtension;
  std::string mNewFileName;
  ProtectorType mPType;
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_PROTECTORSELECTOR_H
