// This file contains the main driver code for all unit tests.
//#define _CRTDBG_MAP_ALLOC  
//#include <stdlib.h>  
//#include <crtdbg.h>  

#include <gtest/gtest.h>
#include "test_utils.h"

int main(int argc, char** argv) {
  mip::file::InitUnitTests(argc, argv);
  testing::InitGoogleTest(&argc, argv);
  RUN_ALL_TESTS();
  #ifdef _WIN32
    _CrtDumpMemoryLeaks();
  #endif
}