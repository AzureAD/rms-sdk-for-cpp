#include <QString>
#include <QtTest>
#undef signals // should have this because of coalogion between QtTest and LibGsf.
#include "test_zipapiteststest.h"
#include <Windows.h>
#include <Cstring>
#include <sstream>
#include <Cstring>
#include <Tchar.h>
#include <string>
#include "../../OPCFileFormat/zipapi.h"
#include <gsf/gsf.h>
#include <gio/gdbusintrospection.h>
#include <gsf/gsf.h>
#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-utils.h>
#include <gsf/gsf-infile.h>
#include <gsf/gsf-infile-zip.h>
#include <gio/giotypes.h>
#include <gio/gio.h>
#include <glib/gmessages.h>

void ZipApiTestsTest::testCase1()
{
   ZipApi a;
   auto val = a.GetEntry("C:\\Users\\amassi\\Desktop\\aaaa.docx", "docProps/custom.xml");
   //a.SetEntry("C:\\Users\\amassi\\Desktop\\aaaa.docx","","");
    std::cout << val << std::endl;
}
