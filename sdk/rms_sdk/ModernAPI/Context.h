#ifndef CONTEXT_H
#define CONTEXT_H

#include "ModernAPIExport.h"
#include "icontext.h"
#include <QCoreApplication>

namespace rmscore{
namespace modernapi{

class DLL_PUBLIC_RMS Context: public IContext
{
public:
    Context(int argc, char* argv[]);
    virtual void Finish() override;
private:
    QCoreApplication qApplication;
};

}
}


#endif // CONTEXT_H
