#ifndef ICONTEXT_H
#define ICONTEXT_H

#include "ModernAPIExport.h"
#include <memory>
#include <string>

namespace rmscore {
namespace modernapi{

class DLL_PUBLIC_RMS IContext{

public:
    static IContext* MakeContext(std::string& executablePath);
    virtual void Finish() = 0;
};

}
}

#endif // ICONTEXT_H
