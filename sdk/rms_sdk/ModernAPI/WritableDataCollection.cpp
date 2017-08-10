#include "WritableDataCollection.h"

using namespace rmscore::core;

namespace rmscore {
namespace modernapi {

WritableDataCollection* WritableDataCollection::AcquireWritableDataCollection(string &UserEmail,
                                                                                              IAuthenticationCallback &AuthenticationCallback,
                                                                                              string &ClientID)
{
    return new WritableDataCollection(UserEmail, AuthenticationCallback, ClientID);
}

WritableDataCollection::WritableDataCollection(string &UserEmail, IAuthenticationCallback &AuthenticationCallback, string &ClientID)
{
    IAuthenticationCallbackImpl* authenticationCallbackImpl = new AuthenticationCallbackImpl(AuthenticationCallback, ClientID);
    auto cancelState = std::make_shared<std::atomic<bool>>(false);
    std::shared_ptr<rmscore::restclients::ICLCClient> clcClient = rmscore::restclients::ICLCClient::MakeCLCClient(cancelState);
    clientLicensorCertificate = clcClient->CreateCLC(UserEmail, *authenticationCallbackImpl);
}

void WritableDataCollection::AddData(string &Key, string &Value)
{
    collectedData[Key] = Value;
}

std::string WritableDataCollection::SignAndSerializeDataCollection()
{
    std::shared_ptr<IJsonObject> dataCollectionJson = IJsonObject::Create();
    dataCollectionJson->SetNamedObject(JsonConstants::DC_DATA, *ConvertDataToJson());
    std::shared_ptr<IStringSigner> stringSigner = IStringSigner::MakeStringSigner(clientLicensorCertificate);
    signature = stringSigner->GenerateSignature(GetDataHash());
    std::shared_ptr<IJsonObject> signatureJSON = IJsonObject::Create();
    signatureJSON->SetNamedString("signed_data_hash", signature.SignedDataHash);
    signatureJSON->SetNamedString("algorithm", signature.SigningAlgorithm);
    signatureJSON->SetNamedObject("certificate", *signature.Certificate);
    dataCollectionJson->SetNamedObject("signature", *signatureJSON);
    auto jsonAsVector = dataCollectionJson->Stringify();
    return std::string(jsonAsVector.begin(), jsonAsVector.end());
}

}
}
