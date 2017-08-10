#ifndef JSONCONSTANTS_H
#define JSONCONSTANTS_H

#include <string>

namespace rmscore {
namespace common {

class JsonConstants
{
public:
    static const std::string HEADER_VERSION;
    //PL constants
    static const std::string PAYLOAD;
    static const std::string HEADER;
    static const std::string VERSION;
    static const std::string LICENSE;
    static const std::string LICENSE_ID;
    static const std::string OWNER;
    static const std::string CREATOR;
    static const std::string REFERRER;
    static const std::string CONTENT_ID;
    static const std::string LICENSE_CREATION_DATE;
    static const std::string EXPIRATION_TIME;
    static const std::string SIGNED_APPLICATION_DATA;
    static const std::string ENCRYPTED_POLICY;
    static const std::string CREATOR_EMAIL;
    static const std::string TEMPLATE_ID;
    static const std::string ENCRYPTED_APPLICATION_DATA;
    static const std::string CUSTOM_POLICY;
    static const std::string USER_RIGHTS;
    static const std::string USERS;
    static const std::string RIGHTS;
    static const std::string INTERVAL_TIME;
    static const std::string SESSION_KEY;
    static const std::string ENCRYPTED_CONTENT_KEY;
    static const std::string ALGORITHM;
    static const std::string CIPHER_MODE;
    static const std::string KEY;
    static const std::string OWNER_KEY;
    static const std::string OWNER_RAC_PUBKEY_HASH;
    static const std::string SERVER_KEY;
    //CLC constants
    static const std::string PUBLIC_DATA;
    static const std::string ISSUED_TO;
    static const std::string EMAIL;
    static const std::string FRIENDLY_NAME;
    static const std::string ID;
    static const std::string TYPE;
    static const std::string VALUE;
    static const std::string PUBLIC_KEY;
    static const std::string KEY_TYPE;
    static const std::string PUBLIC_EXPONENT;
    static const std::string PUBLIC_MODULUS;
    static const std::string ISSUER;
    static const std::string EXTRANET_URL;
    static const std::string INTRANET_URL;
    static const std::string PRIVATE_DATA;
    static const std::string PRIVATE_KEY;
    static const std::string PRIVATE_MODULUS;
    static const std::string ISSUED_TIME;
    //sig constants
    static const std::string SIGNATURE;
    static const std::string DIGEST;
    //DataCollection constants
    static const std::string DC_DATA;
    static const std::string DC_SIGNATURE;
};

}
}


#endif // JSONCONSTANTS_H
