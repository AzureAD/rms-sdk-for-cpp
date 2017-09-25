#include <QString>
#include <string>
#include <QtTest>
#include <XMPFileFormat/xmp_file_format.h>
#include <Common/std_stream_adapter.h>
#include <sstream>
#include <fstream>
#include <regex>
#include <Common/tag.h>

using std::static_pointer_cast;
using std::make_shared;
using mip::file::XMPFileFormat;
using mip::Tag;
using mip::file::StdStreamAdapter;

namespace {
static bool VerifyTags(Tag tag1, Tag tag2) {
  if(tag1.GetExtendedProperties() != tag2.GetExtendedProperties()){
    return false;
  }

  return
      tag1.GetLabelId() == tag2.GetLabelId() &&
      tag1.GetLabelName() == tag2.GetLabelName() &&
      tag1.GetLabelParentId() == tag2.GetLabelParentId() &&
      tag1.GetEnabled() == tag2.GetEnabled() &&
      tag1.GetOwner() == tag2.GetOwner() &&
      tag1.GetSiteId() == tag2.GetSiteId();
}

void VerifyUpdateProperties(string folder) {
  vector<mip::ExtendedProperty> properties;
  mip::ExtendedProperty property1;
  property1.key = "TestKey";
  property1.vendor = "TestVendor";
  property1.value = "Test";
  properties.push_back(property1);

  mip::ExtendedProperty property2;
  property2.key = "TestKey2";
  property2.vendor = "TestVendor2";
  property2.value = "Test2";
  properties.push_back(property2);

  time_t  now;
  time(&now);
  tm* localTime = localtime(&now);
  char setTime[30];
  strftime(setTime, 30, "%Y-%m-%d %H:%M:%S.000", localTime);
  mip::Tag parent( "5dd9004f-e5fd-429e-9243-d10516ef5a01", "Parent", "", "shbaruch@microsoft.com", true, setTime, mip::Method::AUTOMATIC, "72f988bf-86f1-41af-91ab-2d7cd011db47", properties);
  mip::Tag child( "a3af17be-e32b-4bda-a3d6-574cfa83d9ff", "Child", "5dd9004f-e5fd-429e-9243-d10516ef5a01", "shbaruch@microsoft.com", true, setTime, mip::Method::AUTOMATIC, "72f988bf-86f1-41af-91ab-2d7cd011db47", properties);

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

      auto fileStream = std::make_shared<std::ifstream>(tempFile.toStdString(), std::ios::binary);
      auto stream = StdStreamAdapter::Create(fileStream);

      XMPFileFormat xmpFileFormat(stream, extension);
      auto currentProperties = xmpFileFormat.GetProperties();
      vector<string> keysToRemove;
      for(int i = 0; i <currentProperties.size(); i++){
        keysToRemove.push_back(currentProperties[i].first);
      }

      xmpFileFormat.UpdateProperties(Tag::ToProperties(tags), keysToRemove);

      auto outputFileName = regex_replace(tempFile.toStdString(), std::regex(extension), "out" + extension);
      std::ofstream temp(outputFileName);
      temp.close();

      auto outfile = std::make_shared<std::fstream>(outputFileName, std::ios::binary| std::ios::in | std::ios::out);
      auto outStream = StdStreamAdapter::Create(std::static_pointer_cast<std::iostream>(outfile));

      string newExtention;
      xmpFileFormat.Commit(outStream, newExtention);
      outStream->Flush();
      outfile->close();

      fileStream = std::make_shared<std::ifstream>(outputFileName, std::ios::binary);
      stream = StdStreamAdapter::Create(fileStream);

      XMPFileFormat xmpOut(stream, newExtention);
      vector<Tag> tags = Tag::FromProperties(xmpOut.GetProperties());
      QVERIFY2(tags.size() == 2, "Tags count shoud be 2");
      QVERIFY2(VerifyTags(tags[0], parent), "Tag1 is different than expected");
      QVERIFY2(VerifyTags(tags[1], child), "Tag2 is different than expected");
      QVERIFY2(tags[0].GetSetTime() == setTime, "SetTime is different than expected");
      QVERIFY2(tags[1].GetSetTime() == setTime, "SetTime is different than expected");
    }
  }
}

}

class Xmp_Tests : public QObject
{
  Q_OBJECT

public:
  Xmp_Tests();

private Q_SLOTS:
  void GetProperties_FileWithAutomaticTag_ReturnCorrectTag();
  void GetProperties_FileWithManualTag_ReturnCorrectTag();
  void SetProperties_FileWithNoTags_TagsSetSuccessfully();
  void SetProperties_FileWithExistingTags_TagsSetSuccessfully();
};

Xmp_Tests::Xmp_Tests() {
}

void Xmp_Tests::GetProperties_FileWithManualTag_ReturnCorrectTag() {
  try{
    mip::Tag general( "f42aa342-8706-4288-bd11-ebb85995028c", "General", "", "vakuras@microsoft.com", true, "", mip::Method::MANUAL, "72f988bf-86f1-41af-91ab-2d7cd011db47");

    QTemporaryDir tempDir;
    tempDir.autoRemove();
    foreach( const QString &fileName, QDir(":xmp_labeled_manual").entryList() )
    {
      auto fileNameString = fileName.toStdString();
      auto extension = fileNameString.substr(fileNameString.find_last_of("."));
      auto tempFile = tempDir.path() + "/temp" + QString::fromStdString(extension);
      if (QFile::copy(":xmp_labeled_manual/"+fileName, tempFile)) {

        auto fileStream = std::make_shared<std::ifstream>(tempFile.toStdString(), std::ios_base::binary);
        auto stream = StdStreamAdapter::Create(fileStream);

        XMPFileFormat xmpFileFormat(stream, extension);
        auto tags = Tag::FromProperties(xmpFileFormat.GetProperties());
        QVERIFY2(tags.size() == 1, "Tags count shoud be 1");
        mip::Tag tag = tags[0];
        QVERIFY2(VerifyTags(tag, general), "Tag is different than expected");
      }
      else
        QFAIL("Failed to copy file");
    }
  }
  catch(...){
    QFAIL("An error occured");
  }
}

void Xmp_Tests::GetProperties_FileWithAutomaticTag_ReturnCorrectTag() {
  try{
    mip::Tag general( "f42aa342-8706-4288-bd11-ebb85995028c", "General", "", "shbaruch@microsoft.com", true, "", mip::Method::AUTOMATIC, "72f988bf-86f1-41af-91ab-2d7cd011db47");

    QTemporaryDir tempDir;
    tempDir.autoRemove();

    foreach( const QString &fileName, QDir(":xmp_labeled_automatic").entryList() )
    {
      auto fileNameString = fileName.toStdString();
      auto extension = fileNameString.substr(fileNameString.find_last_of("."));
      auto tempFile = tempDir.path() + "/temp" +  QString::fromStdString(extension);
      if (QFile::copy(":xmp_labeled_automatic/" + fileName, tempFile)) {

        auto ifs = std::make_shared<std::ifstream>(tempFile.toStdString(), std::ios::binary);
        auto stream = StdStreamAdapter::Create(std::static_pointer_cast<std::istream>(ifs));

        XMPFileFormat xmpFileFormat(stream, extension);
        auto tags = Tag::FromProperties(xmpFileFormat.GetProperties());
        QVERIFY2(tags.size() == 1, "Tags count shoud be 1");
        mip::Tag tag = tags[0];
        QVERIFY2(VerifyTags(tag, general), "Tag is different than expected");

        QVERIFY2(tag.GetSetTime().find("2017-09-11") == 0, "SetTime is different than expected");
      }
      else
        QFAIL("Failed to copy file");
    }
  }
  catch(...){
    QFAIL("An error occured");
  }
}

void Xmp_Tests::SetProperties_FileWithNoTags_TagsSetSuccessfully() {
  VerifyUpdateProperties(":xmp_not_labeled");
}

void Xmp_Tests::SetProperties_FileWithExistingTags_TagsSetSuccessfully() {
  VerifyUpdateProperties(":xmp_labeled_manual");
}


QTEST_APPLESS_MAIN(Xmp_Tests)

#include "xmp_file_tests.moc"
