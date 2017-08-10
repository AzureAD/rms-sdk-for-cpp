#ifndef ICLCCLIENT_H
#define ICLCCLIENT_H

#include <atomic>
#include "Platform/Json/IJsonObject.h"
#include "ModernAPI/IAuthenticationCallbackImpl.h"

namespace rmscore {
namespace restclients {

/**
 * @brief The ICLCClient class fetches a CLC from the server (or the cache, if available)
 */
class ICLCClient{

public:

    /**
     * @brief Generate a new CLC Client
     * @return
     */
    static std::shared_ptr<ICLCClient> MakeCLCClient(std::shared_ptr<std::atomic<bool>> cancelState);

    /**
     * @brief Get the CLC from either the server, or the cache (if present)
     * @param email
     * @param authenticationCallback - Only used if the CLC is not in the cache
     * @return
     */
    virtual std::shared_ptr<rmscore::platform::json::IJsonObject> CreateCLC(
                                           const std::string email,
                                           rmscore::modernapi::IAuthenticationCallbackImpl& authenticationCallback) = 0;
};

}
}

#endif // ICLCCLIENT_H
