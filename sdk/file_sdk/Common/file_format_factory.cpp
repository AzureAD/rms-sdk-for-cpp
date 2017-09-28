#include "file_format_factory.h"
#include <algorithm>
#include <vector>
#include <string>

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
static const vector<string> kCompoundFileExtentions {".doc", ".xls", ".xlt", ".ppt", ".pps", ".pot", ".vsd", ".vdw", ".vst", ".pub", ".sldprt", ".slddrw", ".sldasm", ".mpp", ".mpt"};
static const vector<string> kOPCFileExtentions {".docx", ".docm", ".dotm", ".dotx", ".xlsx", ".xltx", ".xltm", ".xlsm", ".xlsb", ".pptx", ".ppsx", ".pptm", ".ppsm", ".potx", ".potm",  ".vsdx", ".vsdm", ".vssx", ".vssm", ".vstx", ".vstm", ".xps", ".oxps", ".dwfx"};
static const vector<string> kPDFExtentions {".pdf"};
static const vector<string> kPfileExtentions {".pfile", ".ppdf", ".ptxt", ".pxml", ".pjpg", ".pjpeg", ".ppng", ".ptif", ".ptiff", ".pbmp", ".pgif", ".pjpe", ".pjfif", ".pjt"};
}  // namespace

namespace mip {
namespace file {

FileFormatFactory::FileFormatFactory()
{
}

// static
bool FileFormatFactory::in_array(const std::string &value, const vector<std::string> &array) {
    return std::find(array.begin(), array.end(), value) != array.end();
}

// static
std::shared_ptr<IFileFormat> FileFormatFactory::Create(std::shared_ptr<IStream> stream, const string& extension) {
  if (!stream){
    // TODO: add log
    throw std::invalid_argument("stream is null");
  }

  if (extension.empty()) {
    // TODO: add log
    throw std::invalid_argument("The extention is empty");
  }

  std::string finalExtention = extension;
  std::transform(finalExtention.begin(), finalExtention.end(), finalExtention.begin(), ::tolower);
  auto pos = extension.find_last_of('.');
  if(pos == string::npos) {
    // TODO: add log
    finalExtention.insert(finalExtention.begin(), '.');
  }
  else if (pos != 0) { // if the extention has more then 1 dot
    finalExtention = finalExtention.substr(pos);
  }

  if (in_array(finalExtention, kXMPExtentions)) {
    return std::make_shared<XMPFileFormat>(stream, finalExtention);
  }
  else if (in_array(finalExtention, kCompoundFileExtentions)) {
    return std::make_shared<CompoundFileFormat>(stream, finalExtention);
  }
  else if (in_array(finalExtention, kOPCFileExtentions)) {
    return std::make_shared<OPCFileFormat>(stream, finalExtention);
  }
  else if (in_array(finalExtention, kPDFExtentions)) {
    return std::make_shared<PDFFileFormat>(stream, finalExtention);
  }
  else if (in_array(finalExtention, kPfileExtentions)) {
    return std::make_shared<PFileFileFormat>(stream, finalExtention);
  }

  return std::make_shared<DefaultFormat>(stream, finalExtention);
}

} // namespace file
} // namespace mip
