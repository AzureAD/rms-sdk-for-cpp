#include <QString>
#include <QtTest>
#include <string>
#include <QTStreamImpl.h>
#include <PFileFileFormat/pfile_file_format.h>
#include <CryptoAPI.h>
#include <sstream>
#include <fstream>

using rmscrypto::api::IStream;
using std::static_pointer_cast;
using std::make_shared;
using mip::file::PFileFileFormat;

class pfile_tests : public QObject
{
  Q_OBJECT

public:
  pfile_tests();

private Q_SLOTS:
  void GetProperties_FileWithTag_ReturnCorrectTag();
  void GetProperties_FileWithoutTag_ReturnZeroTags();
};

pfile_tests::pfile_tests() {
}

void pfile_tests::GetProperties_FileWithTag_ReturnCorrectTag() {

//  QTemporaryDir tempDir;
//  tempDir.autoRemove();

//  foreach(const QString &fileName, QDir(":files/taged_pfiles").entryList()) {

//    auto fileNameString = fileName.toStdString();
//    auto extension = fileNameString.substr(fileNameString.find_last_of("."));
//    auto tempFile = tempDir.path() + "/temp" + QString::fromStdString(extension);

//    QVERIFY2(QFile::copy(":files/taged_pfiles/"+fileName, tempFile), "Failed to copy to temp file");

//    auto ifs = std::make_shared<std::ifstream>(tempFile.toStdString(), std::ios_base::binary);
//    auto stream = rmscrypto::api::CreateStreamFromStdStream(std::static_pointer_cast<std::istream>(ifs));
//    PFileFileFormat pFileFileFormat(stream, extension);
//    auto tags = pFileFileFormat.GetProperties();

//    QVERIFY2(tags.size() == 1, "Tags count shoud be 1");
//    mip::Tag tag = tags[0];
//    QVERIFY2(tag.GetLabelName() == "General", "Label name is different than expected");
//  }
}

void pfile_tests::GetProperties_FileWithoutTag_ReturnZeroTags() {

//  QTemporaryDir tempDir;
//  tempDir.autoRemove();

//  foreach(const QString &fileName, QDir(":files/pfiles").entryList()) {

//    auto fileNameString = fileName.toStdString();
//    auto extension = fileNameString.substr(fileNameString.find_last_of("."));
//    auto tempFile = tempDir.path() + "/temp" + QString::fromStdString(extension);

//    QVERIFY2(QFile::copy(":files/pfiles/"+fileName, tempFile), "Failed to copy to temp file");

//    auto ifs = std::make_shared<std::ifstream>(tempFile.toStdString(), std::ios_base::binary);
//    auto stream = rmscrypto::api::CreateStreamFromStdStream(std::static_pointer_cast<std::istream>(ifs));
//    PFileFileFormat pFileFileFormat(stream, extension);
//    auto tags = pFileFileFormat.GetProperties();

//    QVERIFY2(tags.size() == 0, "Tags count shoud be 0");
//  }
}

QTEST_APPLESS_MAIN(pfile_tests)

#include "pfile_tests.moc"
