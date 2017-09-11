#include <QString>
#include <string>
#include <QtTest>
#include <QTStreamImpl.h>
#include <XMPFileFormat/xmp_file_format.h>

using mip::file::XMPFileFormat;

class Xmp_Tests : public QObject
{
  Q_OBJECT

public:
  Xmp_Tests();

private Q_SLOTS:
  void GetTags_FileWithTag_ReturnCorrectTag();
};

Xmp_Tests::Xmp_Tests()
{
}

void Xmp_Tests::GetTags_FileWithTag_ReturnCorrectTag()
{
  /*mip::Tag general( "f42aa342-8706-4288-bd11-ebb85995028c", "General", "", "vakuras@microsoft.com", true, "", "", mip::Method::MANUAL,"72f988bf-86f1-41af-91ab-2d7cd011db47" );

  foreach( const QString &fileName, QDir(":files/xmp_labeled").entryList() )
  {
    QFile file(":files/xmp_labeled/" + fileName);
    QSharedPointer<QDataStream> obj = QSharedPointer<QDataStream>(new QDataStream(&file));
    auto stream = QTStreamImpl::Create(obj);
    XMPFileFormat xmpFileFormat(stream, ".gif");
    auto tags = xmpFileFormat.GetTags();
    QVERIFY2(tags.size() == 1, "Tags count shoud be 1");
    mip::Tag tag = tags[0];
    QVERIFY2(tag == general, "Tag is different than expected");
  }*/
}

QTEST_APPLESS_MAIN(Xmp_Tests)

#include "xmp_file_tests.moc"

