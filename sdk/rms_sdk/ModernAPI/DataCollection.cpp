#include "DataCollection.h"

namespace rmscore {
namespace modernapi {

std::shared_ptr<IDataCollection> IDataCollection::AcquireVerifiedDataCollection(std::string &dataCollectionJson){
    return std::shared_ptr<IDataCollection>(new DataCollection(dataCollectionJson));
}

DataCollection::DataCollection(){

}

DataCollection::DataCollection(std::string &json){

}

bool DataCollection::IsVerified(){
    return true;
}

}
}
