#include "CertificateJsonParser.h"

namespace rmscore{
namespace json{

CertificateJsonParser::CertificateJsonParser(shared_ptr<IJsonObject> certificateJSON): certificate(certificateJSON){}

vector<uint8_t> CertificateJsonParser::GetPrivateModulus(){
    return common::ConvertBase64ToBytes(GetPayload()
                                              ->GetNamedObject(JsonConstants::PRIVATE_DATA)
                                              ->GetNamedObject(JsonConstants::PRIVATE_KEY)
                                              ->GetNamedValue(JsonConstants::PRIVATE_MODULUS));
}

vector<uint8_t> CertificateJsonParser::GetPublicExponent(){
    return GetPublicKey()->GetNamedValue(JsonConstants::PUBLIC_EXPONENT);
}

vector<uint8_t> CertificateJsonParser::GetPublicModulus(){
    return common::ConvertBase64ToBytes(GetPublicKey()->GetNamedValue(JsonConstants::PUBLIC_MODULUS));
}

shared_ptr<IJsonObject> CertificateJsonParser::GetPublicCertificate(){
    std::shared_ptr<IJsonObject> PublicCertificate = IJsonObject::Create();
    std::shared_ptr<IJsonObject> Payload = IJsonObject::Create();
    Payload->SetNamedObject(JsonConstants::PUBLIC_DATA, *(certificate->GetNamedObject(JsonConstants::PAYLOAD)->GetNamedObject(JsonConstants::PUBLIC_DATA)));
    PublicCertificate->SetNamedObject(JsonConstants::PAYLOAD, *Payload);
    return PublicCertificate;
}

shared_ptr<IJsonObject> CertificateJsonParser::GetPayload(){
    return certificate->GetNamedObject(JsonConstants::PAYLOAD);
}

shared_ptr<IJsonObject> CertificateJsonParser::GetPublicKey(){
    return *(new shared_ptr<IJsonObject>(GetPayload()
            ->GetNamedObject(JsonConstants::PUBLIC_DATA)
            ->GetNamedObject(JsonConstants::PAYLOAD)
            ->GetNamedObject(JsonConstants::ISSUED_TO)
            ->GetNamedObject(JsonConstants::PUBLIC_KEY)));
}

}
}
