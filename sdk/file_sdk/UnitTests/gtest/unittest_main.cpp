// This file contains the main driver code for all unit tests.
//#define _CRTDBG_MAP_ALLOC  
//#include <stdlib.h>  
//#include <crtdbg.h>  

#include <iostream>
#include <memory>

#include "gtest/gtest.h"

std::string GetResourceFile(const std::string& fileName) {
  std::string path("Resources\\"); // TODO: use path relative to executable
  return path + fileName;
}

int main(int argc, char** argv) {
  std::cout << "test";
  testing::InitGoogleTest(&argc, argv);
  RUN_ALL_TESTS();
  #ifdef _WIN32
    _CrtDumpMemoryLeaks();
  #endif
}
