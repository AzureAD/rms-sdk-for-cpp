#include "depend.h"

std::string unittests::dependency::GetCurrentInputFile() {
  QString FilePaths = qApp->applicationDirPath();
  int k = FilePaths.lastIndexOf('/');
  FilePaths = FilePaths.mid(0,k);
  k = FilePaths.lastIndexOf('/');
  FilePaths = FilePaths.mid(0,k);
  std::string FilePath1 = FilePaths.toStdString();
  FilePath1 = FilePath1 + "/sdk/rms_sdk/UnitTests/fileapi_ut/pdf_protector_ut/";
  return FilePath1;
}
