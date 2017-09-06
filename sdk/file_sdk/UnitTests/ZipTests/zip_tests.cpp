#include <QString>
#include <QtTest>
#include "../../OPCFileFormat/zipapi.h"
#include <iostream>

class ZipTests : public QObject
{
  Q_OBJECT

public:
  ZipTests();

private Q_SLOTS:
  void testCase1();
};

ZipTests::ZipTests()
{
}

void ZipTests::testCase1()
{
  auto zip = ZipApi();
  auto val = zip.GetEntry("\\\\sislands\\Public\\RnD\\rms-sdk\\Tests\\LabeledGeneral.docx", "docProps/custom.xml");
  //zip.SetEntry("\\\\sislands\\Public\\RnD\rms-sdk\\Tests\\Output.docx","","");
  std::cout << val << std::endl;

  QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(ZipTests)

#include "zip_tests.moc"
