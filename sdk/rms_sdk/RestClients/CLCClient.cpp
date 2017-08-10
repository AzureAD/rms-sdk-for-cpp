#include "CLCClient.h"

namespace rmscore{
namespace restclients{

const static string CLCCacheName = "CLC";
const static string CLCCacheTag = "CLIENT_LICENSOR_CERTIFICATES_UR";

std::shared_ptr<ICLCClient> ICLCClient::MakeCLCClient(std::shared_ptr<std::atomic<bool>> cancelState)
{
    return std::shared_ptr<ICLCClient>(new CLCClient(cancelState));
}

CLCClient::CLCClient(std::shared_ptr<std::atomic<bool>> cancelState): cancelState(cancelState){}

std::shared_ptr<rmscore::platform::json::IJsonObject> CLCClient::CreateCLC(const std::string email,
                     modernapi::IAuthenticationCallbackImpl &authenticationCallback){
    std::string stringCLC;
    size_t size;
    auto restClientCache = RestClientCache::Create(RestClientCache::CACHE_PLAINDATA);
    std::vector<uint8_t> cacheSearchIndex = common::HashString(std::vector<uint8_t>(email.begin(), email.end()), &size);
    auto cacheSearchResult = restClientCache->Lookup(CLCCacheName, CLCCacheTag, &cacheSearchIndex[0], size, false);
    if(cacheSearchResult.capacity() > 0){
        stringCLC = cacheSearchResult.at(0);
    }else{
        auto unparsedCLC = RetrieveCLCFromServer(email, authenticationCallback, cancelState);
        stringCLC = ParseCLC(unparsedCLC);
    }
    auto jsonCLC = rmscore::platform::json::IJsonParser::Create()->Parse(vector<uint8_t>(stringCLC.begin(), stringCLC.end()));
    if(cacheSearchResult.capacity() == 0)
        SaveToCache(restClientCache, email, stringCLC, jsonCLC);
    return jsonCLC;
}

//retrives CLC from server or cache, depending on if the cache exists
RestHttpClient::Result CLCClient::RetrieveCLCFromServer(const std::string& sEmail,
                        modernapi::IAuthenticationCallbackImpl& authenticationCallback,
                        std::shared_ptr<std::atomic<bool>> cancelState)
{
    auto pRestServiceUrlClient = RestServiceUrlClient::Create();
    auto clcUrl = pRestServiceUrlClient->GetClientLicensorCertificatesUrl(sEmail, authenticationCallback, cancelState);
    if (clcUrl == ""){
        throw rmscore::exceptions::RMSEndpointNotFoundException("Could not find ClientLicensorCertificates URL.");
    }
    auto request = rmscore::platform::json::IJsonObject::Create();
    request->SetNamedString("SignatureEncoding", "utf-8");
    auto result = RestHttpClient::Post(clcUrl, request->Stringify(),authenticationCallback, cancelState);
    if (result.status != rmscore::platform::http::StatusCode::OK)
        HandleRestClientError(result.status, result.responseBody);
    return result;
}

void CLCClient::SaveToCache(shared_ptr<IRestClientCache> cache, const string &email, const string stringCLC, const std::shared_ptr<IJsonObject> jsonCLC){
    auto clcAsVector = vector<uint8_t>(stringCLC.begin(), stringCLC.end());
    auto privateKeyExpiry = jsonCLC->GetNamedObject("pld")->GetNamedObject("pri")->GetNamedString("exp");
    size_t size;
    auto cacheIndex = common::HashString(vector<uint8_t>(email.begin(), email.end()), &size);
    cache->Store(CLCCacheName, CLCCacheTag, &cacheIndex[0], size, privateKeyExpiry, clcAsVector, false);
}

std::string CLCClient::ParseCLC(RestHttpClient::Result unparsedCLC)
{
    auto pJsonSerializer = rmscore::json::IJsonSerializer::Create();
    try{
        auto clcJSON = pJsonSerializer->DeserializeCertificateResponse(unparsedCLC.responseBody);
        string search = R"(\"pub\":)";
        auto pos = clcJSON.serializedCert.find(search);
        if (pos == std::string::npos)
                throw exceptions::RMSInvalidArgumentException("Invalid CLC from server.");
        return common::Unescape(clcJSON.serializedCert);
    }catch(exceptions::RMSException){
        throw exceptions::RMSNetworkException("PublishClient: Recieved invalid Json from REST endpoint.", exceptions::RMSNetworkException::ServerError);
    }
}

}
}

