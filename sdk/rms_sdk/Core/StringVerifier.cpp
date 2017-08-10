#include "StringVerifier.h"

namespace rmscore {
namespace core{

std::shared_ptr<IStringVerifier> IStringVerifier::GetStringVerifier(std::shared_ptr<IJsonObject> certificate){
    return std::shared_ptr<IStringVerifier>(new StringVerifier(certificate));
}

StringVerifier::StringVerifier(std::shared_ptr<IJsonObject> certificate): certificateJsonParser(certificate){}

bool StringVerifier::VerifyString(string &hash, string &digest){
    auto publicModulus = certificateJsonParser.GetPublicModulus();
    auto publicExponent = certificateJsonParser.GetPublicExponent();
    auto rsaKeyBlob = rmscrypto::api::CreateCryptoEngine()->CreateRSAKeyBlob(publicModulus, publicExponent, publicModulus, false);
    std::vector<uint8_t> signatureVector(hash.begin(), hash.end());
    std::vector<uint8_t> byteSignatureVector = ConvertBase64ToBytes(signatureVector);
    std::vector<uint8_t> digestVector(digest.begin(), digest.end());
    uint32_t size = byteSignatureVector.size();
    std::string errmsg;
    if(rsaKeyBlob->VerifySignature(byteSignatureVector, digestVector, errmsg, size))
        return true;
    return false;
}

}
}
