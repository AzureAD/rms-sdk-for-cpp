#include "extended_property_ut.h"
#include <extended_property.h>

using std::string;

ExtendedPropertyTests::ExtendedPropertyTests() {
}

void SetExtendedProperty(mip::ExtendedProperty& property, string key, string vendor, string value ) {
  property.key = key;
  property.vendor = vendor;
  property.value = value;
}

void ExtendedPropertyTests::Equal_SameProperty_ReturnTrue() {
  mip::ExtendedProperty property1;
  SetExtendedProperty(property1, "key", "vendor", "value");
  mip::ExtendedProperty property2;
  SetExtendedProperty(property2, "KEY", "VENDOR", "value");
  QVERIFY2(property1 == property2, "Proeprties should be equal");
}

void ExtendedPropertyTests::Equal_DifferentKey_ReturnFalse() {
  mip::ExtendedProperty property1;
  SetExtendedProperty(property1, "key", "vendor", "value");
  mip::ExtendedProperty property2;
  SetExtendedProperty(property2, "key1", "vendor", "value");
  bool equal = property1 == property2;
  QVERIFY2(!equal, "Proeprties should not be equal");
}

void ExtendedPropertyTests::Equal_DifferentVendor_ReturnFalse() {
  mip::ExtendedProperty property1;
  SetExtendedProperty(property1, "key", "vendor", "value");
  mip::ExtendedProperty property2;
  SetExtendedProperty(property2, "key", "vendor1", "value");
  bool equal = property1 == property2;
  QVERIFY2(!equal, "Proeprties should not be equal");
}

void ExtendedPropertyTests::Equal_DifferentValue_ReturnFalse() {
  mip::ExtendedProperty property1;
  SetExtendedProperty(property1, "key", "vendor", "value");
  mip::ExtendedProperty property2;
  SetExtendedProperty(property2, "key", "vendor", "Value");
  bool equal = property1 == property2;
  QVERIFY2(!equal, "Proeprties should not be equal");
}
