#include <QCoreApplication>
#include "profile_test.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  int res = 0;
  res += QTest::qExec(new ProfileTest(), argc, argv);

  return a.exec();
}
