#include "SealPolicy.h"

namespace rmscore{
namespace modernapi{

std::shared_ptr<SealPolicy> SealPolicy::Acquire(){
    return std::shared_ptr<SealPolicy>();
}

SealPolicy::SealPolicy(){

}

}
}
