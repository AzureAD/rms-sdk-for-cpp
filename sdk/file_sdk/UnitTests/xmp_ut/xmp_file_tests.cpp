#include <QString>
#include <string>
#include <QtTest>
#include <regex>
#include <QTStreamImpl.h>
#include <XMPFileFormat/xmp_file_format.h>
#include <CryptoAPI.h>
#include <sstream>
#include <fstream>

using rmscrypto::api::IStream;
using std::static_pointer_cast;
using std::make_shared;
using mip::file::XMPFileFormat;
using mip::Tag;

class Xmp_Tests : public QObject
{
  Q_OBJECT

public:
  Xmp_Tests();

private Q_SLOTS:
  void GetTags_FileWithAutomaticTag_ReturnCorrectTag();
  void GetTags_FileWithManualTag_ReturnCorrectTag();
  void SetTags_FileWithNoTags_TagsSetSuccessfully();
  void SetTags_FileWithExistingTags_TagsSetSuccessfully();
};

Xmp_Tests::Xmp_Tests() {
}

bool VerifyTags(Tag tag1, Tag tag2) {
  if (tag1.GetExtendedProperties().size() != tag2.GetExtendedProperties().size())
    return false;

  for (size_t i = 0; i < tag1.GetExtendedProperties().size(); i++) {
    if (tag1.GetExtendedProperties()[i].key != tag2.GetExtendedProperties()[i].key ||
        tag1.GetExtendedProperties()[i].value != tag2.GetExtendedProperties()[i].value ||
        tag1.GetExtendedProperties()[i].vendor != tag2.GetExtendedProperties()[i].vendor) {
      return false;
    }
  }

  return
      tag1.GetLabelId() == tag2.GetLabelId() &&
      tag1.GetLabelName() == tag2.GetLabelName() &&
      tag1.GetLabelParentId() == tag2.GetLabelParentId() &&
      tag1.GetEnabled() == tag2.GetEnabled() &&
      tag1.GetOwner() == tag2.GetOwner() &&
      tag1.GetSiteId() == tag2.GetSiteId();
}

void VerifySetTags(string folder) {
  vector<mip::ExtendedProperty> properties;
  mip::ExtendedProperty property1;
  property1.key = "TestKey";
  property1.vendor = "TestVendor";
  property1.value = "Test";
  properties.push_back(property1);

  mip::ExtendedProperty property2;
  property2.key = "TestKey";
  property2.vendor = "TestVendor";
  property2.value = "Test";
  properties.push_back(property2);

  mip::Tag parent( "5dd9004f-e5fd-429e-9243-d10516ef5a01", "Parent", "", "shbaruch@microsoft.com", true, "", "Microsoft Azure Information Protection", mip::Method::AUTOMATIC, "72f988bf-86f1-41af-91ab-2d7cd011db47", properties);
  mip::Tag child( "a3af17be-e32b-4bda-a3d6-574cfa83d9ff", "Child", "5dd9004f-e5fd-429e-9243-d10516ef5a01", "shbaruch@microsoft.com", true, "", "Microsoft Azure Information Protection", mip::Method::AUTOMATIC, "72f988bf-86f1-41af-91ab-2d7cd011db47", properties);

  vector<mip::Tag> tags;
  tags.push_back(parent);
  tags.push_back(child);

  QTemporaryDir tempDir;
  tempDir.autoRemove();

  foreach( const QString &fileName, QDir(QString::fromStdString(folder)).entryList() )
  {
    auto fileNameString = fileName.toStdString();
    auto extension = fileNameString.substr(fileNameString.find_last_of("."));
    auto tempFile = tempDir.path() + "/temp" +  QString::fromStdString(extension);
    if (QFile::copy(QString::fromStdString(folder) + "/" + fileName, tempFile)) {

      auto ofs = std::make_shared<std::ofstream>(tempFile.toStdString(), std::ios::binary);
      auto stream = rmscrypto::api::CreateStreamFromStdStream(
            std::static_pointer_cast<std::ostream>(ofs));

      XMPFileFormat xmpFileFormat(stream, extension);
      xmpFileFormat.SetTags(tags);

      auto outputFileName = regex_replace(tempFile.toStdString(), std::regex(extension), "out" + extension);
      auto outfile = std::make_shared<std::ofstream>(outputFileName, std::ios::binary);
      auto outStream = rmscrypto::api::CreateStreamFromStdStream(
            std::static_pointer_cast<std::ostream>(outfile));

      string newExtention;
      xmpFileFormat.Commit(outStream, newExtention);
      outStream->Seek(0);
      XMPFileFormat xmpOut(stream, newExtention);
      auto tags = xmpOut.GetTags();
      QVERIFY2(tags.size() == 2, "Tags count shoud be 2");
      QVERIFY2(VerifyTags(tags[0], parent), "Tag1 is different than expected");
      QVERIFY2(VerifyTags(tags[1], child), "Tag2 is different than expected");
    }
  }
}

void Xmp_Tests::GetTags_FileWithManualTag_ReturnCorrectTag() {
  mip::Tag general( "f42aa342-8706-4288-bd11-ebb85995028c", "", "", "vakuras@microsoft.com", true, "", mip::Method::MANUAL, "https://rmsibizaapidf.trafficmanager.net/api/72f988bf-86f1-41af-91ab-2d7cd011db47");

  QTemporaryDir tempDir;
  tempDir.autoRemove();
  foreach( const QString &fileName, QDir(":xmp_labeled_manual").entryList() )
  {
    auto fileNameString = fileName.toStdString();
    auto extension = fileNameString.substr(fileNameString.find_last_of("."));
    auto tempFile = tempDir.path() + "/temp" + QString::fromStdString(extension);
    if (QFile::copy(":xmp_labeled_manual/"+fileName, tempFile)) {

      auto ifs = std::make_shared<std::ifstream>(tempFile.toStdString(), std::ios_base::binary);
      auto stream = rmscrypto::api::CreateStreamFromStdStream(std::static_pointer_cast<std::istream>(ifs));

      XMPFileFormat xmpFileFormat(stream, extension);
      auto tags = xmpFileFormat.GetTags();
      QVERIFY2(tags.size() == 1, "Tags count shoud be 1");
      mip::Tag tag = tags[0];
      QVERIFY2(VerifyTags(tag, general), "Tag is different than expected");
    }
    else
      QFAIL("Failed to copy file");
  }
}

void Xmp_Tests::GetTags_FileWithAutomaticTag_ReturnCorrectTag() {
  vector<mip::ExtendedProperty> properties;
  mip::ExtendedProperty property;
  property.key = "Method";
  property.vendor = "MSFT";
  property.value = "Automatic";
  properties.push_back(property);

  mip::Tag general( "f42aa342-8706-4288-bd11-ebb85995028c", "General", "", "shbaruch@microsoft.com", true, "", mip::Method::NONE, "72f988bf-86f1-41af-91ab-2d7cd011db47", properties);

  QTemporaryDir tempDir;
  tempDir.autoRemove();

  foreach( const QString &fileName, QDir(":xmp_labeled_automatic").entryList() )
  {
    auto fileNameString = fileName.toStdString();
    auto extension = fileNameString.substr(fileNameString.find_last_of("."));
    auto tempFile = tempDir.path() + "/temp" +  QString::fromStdString(extension);
    if (QFile::copy(":xmp_labeled_automatic/" + fileName, tempFile)) {

      auto ifs = std::make_shared<std::ifstream>(tempFile.toStdString(), std::ios::binary);
      auto stream = rmscrypto::api::CreateStreamFromStdStream(
            std::static_pointer_cast<std::istream>(ifs));

      XMPFileFormat xmpFileFormat(stream, extension);
      auto tags = xmpFileFormat.GetTags();

      QVERIFY2(tags.size() == 1, "Tags count shoud be 1");
      mip::Tag tag = tags[0];
      QVERIFY2(VerifyTags(tag, general), "Tag is different than expected");
    }
    else
      QFAIL("Failed to copy file");
  }
}

void Xmp_Tests::SetTags_FileWithNoTags_TagsSetSuccessfully() {
  VerifySetTags(":xmp_not_labeled");
}

void Xmp_Tests::SetTags_FileWithExistingTags_TagsSetSuccessfully() {
  VerifySetTags(":xmp_labeled_manual");
}

QTEST_APPLESS_MAIN(Xmp_Tests)

#include "xmp_file_tests.moc"

