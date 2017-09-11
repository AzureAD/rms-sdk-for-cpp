#include <QTest>

#include "extended_property_ut.h"
#include "common_ut.h"

int main(int argc, char *argv[])
{
  QTest::qExec(new ExtendedPropertyTests());
  QTest::qExec(new CommonTests());

  return 0;
}
