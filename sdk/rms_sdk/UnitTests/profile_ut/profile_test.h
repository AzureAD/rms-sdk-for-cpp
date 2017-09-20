#ifndef PROFILETEST_H
#define PROFILETEST_H

#include <QObject>
#include <QTest>

class ProfileTest : public QObject
{
  Q_OBJECT
public:
  ProfileTest() { }

private slots:
  void TestProfileLoad();
};

#endif // PROFILETEST_H
