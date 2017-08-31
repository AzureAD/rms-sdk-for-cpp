#include <QString>
#include <QtTest>
#include <string>
#include <QTStreamImpl.h>
#include <PFileFileFormat/pfile_file_format.h>

using mip::file::PFileFileFormat;

class pfile_tests : public QObject
{
  Q_OBJECT

public:
  pfile_tests();

private Q_SLOTS:
  void GetTags_FileWithTag_ReturnCorrectTag();
  void GetTags_FileWithoutTag_ReturnZeroTags();
};

pfile_tests::pfile_tests()
{
}

void pfile_tests::GetTags_FileWithTag_ReturnCorrectTag()
{
  foreach( const QString &fileName, QDir(":files/taged_pfiles").entryList() )
  {
    QFile file(":files/taged_pfiles/" + fileName);
    QSharedPointer<QDataStream> obj = QSharedPointer<QDataStream>(new QDataStream(&file));
    auto stream = QTStreamImpl::Create(obj);
    PFileFileFormat pFileFileFormat(stream, ".ptxt");
    auto tags = pFileFileFormat.GetTags();
    QVERIFY2(tags.size() == 1, "Tags count shoud be 1");
    mip::Tag tag = tags[0];
    QVERIFY2(tag.GetLabelName() == "General", "Tag is different than expected");
  }
}

void pfile_tests::GetTags_FileWithoutTag_ReturnZeroTags()
{
  foreach( const QString &fileName, QDir(":files/pfiles").entryList() )
  {
    QFile file(":files/pfiles/" + fileName);
    QSharedPointer<QDataStream> obj = QSharedPointer<QDataStream>(new QDataStream(&file));
    auto stream = QTStreamImpl::Create(obj);
    PFileFileFormat pFileFileFormat(stream, ".ptxt");
    auto tags = pFileFileFormat.GetTags();
    QVERIFY2(tags.size() == 0, "Tags count shoud be 0");
  }
}

QTEST_APPLESS_MAIN(pfile_tests)

#include "pfile_tests.moc"
