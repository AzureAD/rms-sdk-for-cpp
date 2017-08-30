/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_OFFICE_PROTECTOR_DATASPACES_H
#define RMS_SDK_OFFICE_PROTECTOR_DATASPACES_H

#include <memory>
#include <string>
#include <gsf/gsf.h>
#include "../Common/CommonTypes.h"

using namespace rmscore::common;

namespace rmscore {
namespace officeprotector {

// DataSpaces contains the transform process. It contains the version, license etc
// For more information, go to https://msdn.microsoft.com/en-us/library/aa767782(v=vs.85).aspx#data_spaces
class DataSpaces {
public:
  DataSpaces(bool isMetro, bool doesUseDeprecatedAlgorithm = true);
  ~DataSpaces();
  void WriteDataSpaces(GsfOutfile* stg, const ByteArray& publishingLicense);
  void ReadDataSpaces(GsfInfile* stg, ByteArray& publishingLicense);

private:
  /*!
   * \brief Writes major and minor version for reading, writing and updating.
   * For more information, go to https://msdn.microsoft.com/en-us/library/aa767782(v=vs.85).aspx#structure_version
   * \param stm
   * \param content
   */
  void WriteVersion(GsfOutput* stm, const std::string& content);

  /*!
   * \brief Reads and verifies the major and minor version for reading, writing and updating.
   * For more information, go to https://msdn.microsoft.com/en-us/library/aa767782(v=vs.85).aspx#structure_version
   * \param stm
   * \param contentExpected
   */
  void ReadAndVerifyVersion(GsfInput* stm, const std::string& contentExpected);
  /*!
   * \brief The data space map associates transformed content with the data space that describes the
   * series of transforms applied to that data. For more information, go to
   * https://msdn.microsoft.com/en-us/library/aa767782(v=vs.85).aspx#structure_dataspacemap
   * \param stm
   */
  void WriteDataSpaceMap(GsfOutput* stm);

  /*!
   * \brief Writes information about the number of transforms applied and the names of the transforms.
   * For more information, go to https://msdn.microsoft.com/en-us/library/aa767782(v=vs.85).aspx#structure_dataspaceinfo
   * \param stm
   */
  void WriteDrmDataSpace(GsfOutput *stm);

  /*!
   * \brief Writes the feature identifier and the class GUID for the transform. For more information, go to
   * https://msdn.microsoft.com/en-us/library/aa767782(v=vs.85).aspx#structure_transforminfo
   * \param stm
   * \param txClassName
   * \param featureName
   */
  void WriteTxInfo(GsfOutput* stm, const std::string& txClassName, const std::string& featureName);

  /*!
   * \brief Reads and verifies the feature identifier and the class GUID for the transform.
   * For more information, go to https://msdn.microsoft.com/en-us/library/aa767782(v=vs.85).aspx#structure_transforminfo
   * \param stm
   * \param txClassName
   * \param featureName
   */
  void ReadTxInfo(GsfInput* stm, const std::string& txClassName, const std::string& featureName);

  /*!
   * \brief Calls WriteTxInfo() to write the transform details and then writes the 'publishingLicense'
   * after converting it from UTF16 to UTF8 encoding.
   * \param stm
   * \param publishingLicense
   */
  void WritePrimary(GsfOutput* stm,  const ByteArray& publishingLicense);

  /*!
   * \brief Calls ReadTxInfo() to read and verify the transform details. Reads the publishing
   * license from the stream, converts it to UTF16 and assigns it to the vector 'publishingLicense'.
   * \param stm
   * \param publishingLicense
   */
  void ReadPrimary(GsfInput* stm, ByteArray& publishingLicense);
  bool mIsMetro = true;
  bool mDoesUseDeprecatedAlgorithm=true;
};

struct GsfOutput_deleter {
  void operator () (GsfOutput* obj) const {
    if (!gsf_output_is_closed(obj)) {
      gsf_output_close(obj);
    }
    g_object_unref(G_OBJECT(obj));
  }
};

struct GsfOutfile_deleter {
  void operator () (GsfOutfile* obj) const {
    if (!gsf_output_is_closed(GSF_OUTPUT(obj))) {
      gsf_output_close(GSF_OUTPUT(obj));
    }
    g_object_unref(G_OBJECT(obj));
  }
};

struct GsfInput_deleter {
  void operator () (GsfInput* obj) const {
    g_object_unref(G_OBJECT(obj));
  }
};

struct GsfInfile_deleter {
  void operator () (GsfInfile* obj) const {
    g_object_unref(G_OBJECT(obj));
  }
};

} // namespace officeprotector
} // namespace rmscore

#endif // RMS_SDK_OFFICE_PROTECTOR_DATASPACES_H

