#include "Context.h"


namespace rmscore {
namespace modernapi{

IContext* IContext::MakeContext(std::string& exectuablePath){
    int argc = 1;
    char ** argv = new char*[1]();
    char * writable = new char[exectuablePath.size() + 1];
    std::copy(exectuablePath.begin(), exectuablePath.end(), writable);
    writable[exectuablePath.size()] = '\0';
    argv[0]= writable;
    return new Context(argc, argv);
}

Context::Context(int argc, char* argv[]): qApplication(argc, argv){
}

void Context::Finish(){
   qApplication.exec();
}

}
}
