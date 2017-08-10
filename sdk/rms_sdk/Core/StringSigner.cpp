#include "StringSigner.h"

namespace rmscore{
namespace core{

std::shared_ptr<IStringSigner> IStringSigner::MakeStringSigner(std::shared_ptr<IJsonObject> Certificate){
    return std::shared_ptr<IStringSigner>(new StringSigner(Certificate));
}

Signature StringSigner::GenerateSignature(std::string& data){
    Signature signature;
   signature.Certificate = certificateJsonParser.GetPublicCertificate();
    std::vector<uint8_t> dataHashVector(data.begin(), data.end());
    uint32_t returnSize;
    auto signedData = rsaKeyBlob->Sign(dataHashVector, returnSize);
    auto base64SignedData = common::ConvertBytesToBase64(signedData);
    signature.SignedDataHash = std::string(base64SignedData.begin(), base64SignedData.end());
    return signature;
}

StringSigner::StringSigner(std::shared_ptr<IJsonObject> Certificate): certificate(Certificate),
                                                            certificateJsonParser(Certificate){
    auto n = certificateJsonParser.GetPublicModulus();
    auto e = certificateJsonParser.GetPublicExponent();
    auto d = certificateJsonParser.GetPrivateModulus();
    rsaKeyBlob = rmscrypto::api::CreateCryptoEngine()->CreateRSAKeyBlob(d, e, n, true);
}

}
}
