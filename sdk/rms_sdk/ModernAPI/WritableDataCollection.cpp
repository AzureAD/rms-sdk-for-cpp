#include "WritableDataCollection.h"

namespace rmscore {
namespace modernapi {

std::shared_ptr<IWritableDataCollection> IWritableDataCollection::AcquireWritableDataCollection(std::string &UserEmail, IAuthenticationCallback &AuthenticationCallback, std::string &ClientID){
    return std::shared_ptr<IWritableDataCollection>(new WritableDataCollection(UserEmail, AuthenticationCallback, ClientID));
}

WritableDataCollection::WritableDataCollection(std::string &UserEmail, IAuthenticationCallback &AuthenticationCallback, std::string &ClientID){

}

void WritableDataCollection::AddData(std::string& Key, std::string& Value){

}

std::string WritableDataCollection::SignAndSerializeDataCollection(){
    return "";
}

bool WritableDataCollection::IsVerified(){
    return false;
}

}
}
