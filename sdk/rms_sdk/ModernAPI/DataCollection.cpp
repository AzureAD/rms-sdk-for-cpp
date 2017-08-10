#include "DataCollection.h"

namespace rmscore {
namespace modernapi{

DataCollection* DataCollection::AcquireVerifiedDataCollection(std::string &dataCollectionJson)
{
    return new DataCollection(dataCollectionJson);
}

DataCollection::DataCollection(){
//Default constructor for subclasses
}

DataCollection::DataCollection(string &json): signature()
{
    try{
        std::shared_ptr<IJsonParser> jsonParser = IJsonParser::Create();
        std::shared_ptr<IJsonObject> sealAsJsonObject = jsonParser->Parse(json);
        if(sealAsJsonObject == nullptr){
            throw new exceptions::RMSInvalidArgumentException("Input is not JSON");
        }
        if(sealAsJsonObject->ExistsAndNotNull(JsonConstants::DC_SIGNATURE)){
            auto signatureJSON = sealAsJsonObject->GetNamedObject(JsonConstants::DC_SIGNATURE);
            if(signatureJSON->ExistsAndNotNull("signed_data_hash") && signatureJSON->ExistsAndNotNull("certificate")){
                signature.SignedDataHash = signatureJSON->GetNamedString("signed_data_hash");
                signature.Certificate = signatureJSON->GetNamedObject("certificate");
            }else
                throw new exceptions::RMSInvalidArgumentException("Signature is not valid JSON");
        }else{
            throw new exceptions::RMSInvalidArgumentException("No signature is attached!");
        }
        if(sealAsJsonObject->HasName(JsonConstants::DC_DATA)){
            std::shared_ptr<IJsonObject> payload = sealAsJsonObject->GetNamedObject(JsonConstants::DC_DATA);
            auto payloadDictionary = payload->ToStringDictionary();
            for(AppDataHashMap::iterator it = payloadDictionary.begin(); it != payloadDictionary.end(); ++it){
                collectedData[it->first] = it->second;
            }
        }else{
            throw new exceptions::RMSInvalidArgumentException("JSON does not include any data!");
        }
        verified = ValidateSelf();
    }catch(...){
        //Something failed. In which case, automatically don't verify
        verified = false;
    }
}

std::string DataCollection::GetDataValue(string& Key)
{
    return collectedData[Key];
}

bool DataCollection::ContainsKey(string &Key)
{
    return collectedData.find(Key) != collectedData.end();
}

bool DataCollection::IsVerified()
{
    return verified;
}

Signature DataCollection::GetSignature()
{
    return signature;
}

std::shared_ptr<IJsonObject> DataCollection::ConvertDataToJson()
{
    std::shared_ptr<IJsonObject> dataJson = IJsonObject::Create();
    for(std::map<std::string, std::string>::iterator it = collectedData.begin(); it != collectedData.end(); ++it){
        dataJson->SetNamedString(it->first, it->second);
    }
    return dataJson;
}

std::string DataCollection::GetDataHash()
{
    auto dataJsonVector = ConvertDataToJson()->Stringify();
    size_t hash_size;
    std::string stringToHash(dataJsonVector.begin(), dataJsonVector.end());
    return common::Hash(stringToHash, &hash_size);
}

bool DataCollection::ValidateSelf()
{
    auto clientLicensorCertificate = signature.Certificate;
    auto stringVerifier = IStringVerifier::GetStringVerifier(clientLicensorCertificate);
    return stringVerifier->VerifyString(signature.SignedDataHash, GetDataHash());
}

}

}
