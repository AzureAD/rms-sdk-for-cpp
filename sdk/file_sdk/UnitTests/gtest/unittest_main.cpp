// This file contains the main driver code for all unit tests.
//#define _CRTDBG_MAP_ALLOC  
//#include <stdlib.h>  
//#include <crtdbg.h>  

#include <iostream>
#include <memory>

#include "gtest/gtest.h"

int main(int argc, char** argv) {
  std::cout << "test";
  testing::InitGoogleTest(&argc, argv);
  RUN_ALL_TESTS();
  _CrtDumpMemoryLeaks();
}
