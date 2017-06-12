#ifndef SEALPOLICY_H
#define SEALPOLICY_H

#include <string>
#include <memory>
#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {

class SealPolicy;

class DLL_PUBLIC_RMS SealPolicy {

public:

  static std::shared_ptr<SealPolicy> Acquire();

  void AddContent(std::string Key, std::string Value);

private:

  SealPolicy();

};

} // namespace modernapi
} // namespace rmscore
#endif // SEALPOLICY_H
