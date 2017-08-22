#include "file_format_factory.h"
#include <algorithm>
#include <vector>
#include <RMSExceptions.h>
#include "file_format.h"
#include "XMPFileFormat/xmp_file_format.h"
#include "CompoundFileFormat/compound_file_format.h"
#include "OPCFileFormat/opc_file_format.h"
#include "PDFFileFormat/pdf_file_format.h"
#include "PFileFileFormat/pfile_file_format.h"
#include "DefaultFormat/default_format.h"

using std::vector;
using mip::file::FileFormat;
using mip::file::CompoundFileFormat;
using mip::file::DefaultFormat;
using mip::file::OPCFileFormat;
using mip::file::PDFFileFormat;
using mip::file::PFileFileFormat;
using mip::file::XMPFileFormat;

namespace {
static const vector<string> kXMPExtentions {".jpg", ".jpeg", ".jpe", ".jif", ".jfif", ".jfi",".png", ".tif", ".tiff", ".gif", ".dng", ".psd"};
static const vector<string> kCompoundFileExtentions {".doc", ".doc", ".xls", "xlt", ".ppt", ".pps", ".pot", ".vsd", ".vdw", ".vst", ".pub", ".sldprt", ".slddrw", ".sldasm", ".mpp", ".mpt"};
static const vector<string> kOPCFileExtentions {".docx", ".docm", ".dotm", ".dotx", ".xlsx", ".xltx", ".xltm", ".xlsm", ".xlsb", ".pptx", ".ppsx", ".pptm", ".ppsm", ".potx", ".potm",  ".vsdx", ".vsdm", ".vssx", ".vssm", ".vstx", ".vstm", ".xps", ".oxps", ".dwfx"};
static const vector<string> kPDFExtentions {".pdf"};
static const vector<string> kPfileExtentions {".pfile", ".ppdf", ".ptxt", ".pxml", ".pjpg", ".pjpeg", ".ppng", ".ptif", ".ptiff", ".pbmp", ".pgif", ".pjpe", ".pjfif", ".pjt"};

bool in_array(const string &value, const vector<string> &array) {
    return std::find(array.begin(), array.end(), value) != array.end();
}
}  // namespace

namespace mip {
namespace file {

FileFormatFactory::FileFormatFactory()
{
}

std::shared_ptr<IFileFormat> FileFormatFactory::Create(std::shared_ptr<IStream> fileStream, const string& extension) {

  if (extension.empty())
  {
    // TODO: add log
    throw rmscore::exceptions::RMSInvalidArgumentException("The extention is empty");
  }

  std::string finalExtention = extension;
  if(extension.find_last_of('.') == string::npos)
  {
    // TODO: add log
    finalExtention.insert(finalExtention.begin(), '.');
  }

  if (in_array(finalExtention, kXMPExtentions))
  {
    return std::make_shared<XMPFileFormat>(fileStream, finalExtention);
  }
  else if (in_array(finalExtention, kCompoundFileExtentions))
  {
    return std::make_shared<CompoundFileFormat>(fileStream, finalExtention);
  }
  else if (in_array(finalExtention, kOPCFileExtentions))
  {
    return std::make_shared<OPCFileFormat>(fileStream, finalExtention);
  }
  else if (in_array(finalExtention, kPDFExtentions))
  {
    return std::make_shared<PDFFileFormat>(fileStream, finalExtention);
  }
  else if (in_array(finalExtention, kPfileExtentions))
  {
    return std::make_shared<PFileFileFormat>(fileStream, finalExtention);
  }

  return std::make_shared<DefaultFormat>(fileStream, finalExtention);
}

} // namespace file
} // namespace mip
