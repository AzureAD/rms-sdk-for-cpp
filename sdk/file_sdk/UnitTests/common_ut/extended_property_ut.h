#ifndef EXTENDED_PROPERTY_UT_H
#define EXTENDED_PROPERTY_UT_H

#include "include.h"
#include <QString>

class ExtendedPropertyTests : public QObject
{
  Q_OBJECT

public:
  ExtendedPropertyTests();

private Q_SLOTS:
  void Equal_SameProperty_ReturnTrue();
  void Equal_DifferentKey_ReturnFalse();
  void Equal_DifferentVendor_ReturnFalse();
  void Equal_DifferentValue_ReturnFalse();

};

#endif // EXTENDED_PROPERTY_UT_H
