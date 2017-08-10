#include "../Platform/Json/IJsonObject.h"
#include "../ModernAPI/IAuthenticationCallbackImpl.h"

#ifndef CLCCLIENT_H
#define CLCCLIENT_H

#include "iclcclient.h"
#include <unordered_map>
#include <atomic>
#include "Platform/Json/IJsonObject.h"
#include "Platform/Json/IJsonParser.h"
#include "Json/IJsonSerializer.h"
#include "RestClientCache.h"
#include "RestServiceUrlClient.h"
#include "RestHttpClient.h"
#include "Platform/Http/IHttpClient.h"
#include "RestClientErrorHandling.h"
#include "ModernAPI/RMSExceptions.h"
#include "Common/tools.h"

using namespace std;
using namespace rmscore::platform::json;

namespace rmscore{
namespace restclients{

/**
 * @brief The CLCClient class
 *
 * Taken initially from the PublishClient class,
 * this class has the sole duty of pulling + parsing the CLC from the server
 *
 * Returns a IJsonObject pointer to the CLC
 */
class CLCClient: public ICLCClient
{
public:
    CLCClient(std::shared_ptr<std::atomic<bool>> cancelState);

    /**
     * @brief Get the CLC from either the server, or the cache (if present)
     * @param email
     * @param authenticationCallback - Only used if the CLC is not in the cache
     * @return
     */
    virtual std::shared_ptr<rmscore::platform::json::IJsonObject> CreateCLC(
                                           const std::string email,
                                           rmscore::modernapi::IAuthenticationCallbackImpl& authenticationCallback) override;

private:


    std::shared_ptr<std::atomic<bool>> cancelState;

    /**
     * @brief RetrieveCLCFromServer pulls the CLC from the server
     * @param sEmail
     * @param authenticationCallback
     * @param cancelState
     * @return
     */
    RestHttpClient::Result RetrieveCLCFromServer(const std::string& sEmail,
                            modernapi::IAuthenticationCallbackImpl& authenticationCallback,
                            std::shared_ptr<std::atomic<bool>> cancelState);

    /**
     * @brief ParseCLC transforms the result from RetrieveCLCFromServer into readable JSON
     * @param unparsedCLC
     * @return
     */
    std::string ParseCLC(RestHttpClient::Result unparsedCLC);


    /**
     * @brief SaveToCache
     * @param cache
     * @param email
     * @param clc
     */
    void SaveToCache(shared_ptr<IRestClientCache> cache, const std::string& email, const string stringCLC, const std::shared_ptr<IJsonObject> jsonCLC);

};
}

}

#endif // CLCCLIENT_H
