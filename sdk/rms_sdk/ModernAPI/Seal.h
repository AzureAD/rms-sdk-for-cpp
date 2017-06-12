#ifndef SEAL_H
#define SEAL_H

#include "ModernAPIExport.h"
#include "../Platform/Json/IJsonObject.h"
#include <string>
#include <memory>
#include <map>

namespace rmscore{
namespace modernapi{

/**
 * @brief Represents a signature on a Seal
 */
struct Signature{
public:
    std::string Value; //The signed data in the signature
    std::string SigningAlgorithm; //Signing algorithm used for the signature
    std::shared_ptr<platform::json::IJsonObject> Certificate; //Certificate used to sign this signature
};

/**
 * @brief The Seal class
 *
 * General usage for Seal
 *
 * When signing, Seal() is called. Insert in values to the seal using AddValue. When ready to sign, use GetDataHash() to generate the hash to sign.
 * Sign that string, and pass the signature back to the seal using AddSignature().
 *
 * When verifying, use Seal(SealJSON) to generate a seal from a JSON String. Next, use GetSignature() to get the signature on the seal.
 * Use GetDataHash() to get the hash of the seal's data. Check these two against each other the verify the seal is attached to the signature.
 * Next, check every desired data value using GetValue().
 *
 */

class DLL_PUBLIC_RMS Seal
{
public:

    /**
     * @brief Creates an empty seal.
     */
    Seal();

    /**
     * @brief Creates a new seal from a JSON string. Parses the string and populates it's members accordingly
     * @param SealJSON - JSON String to parse
     */
    Seal(std::string& SealJSON);

    /**
     * @brief Inserts a value into the seal. The Seal object does not handle signing itself, so calling this method after a Seal has been signed will
     * invalidate the Seal's signature.
     * @param Key - The Key to insert
     * @param Value - The value of the inserted data
     */
    void AddValue(std::string& Key, std::string& Value);

    /**
     * @brief Returns the data value associated with the key inside the seal, or null if the data doesn't exist
     * @param Key - Key for desired data
     * @return - String or Null
     */
    std::string GetValue(std::string& Key);

    /**
     * @brief Checks if the desired key exists in the seal
     * @param Key
     * @return
     */
    bool HasKey(std::string& Key);

    /**
     * @brief Append a signature to this seal
     * @param SealSignature
     */
    void AddSignature(Signature SealSignature);

    /**
     * @brief Returns the signature attached to the seal
     * @return
     */
    Signature GetSignature();

    /**
     * @brief Converts the seal to a JSON object, and returns a string of the object
     * @return
     */
    std::string ToJSONString();

    /**
     * @brief Hashes all the key/value pairs inside the seal, and returns the string
     * @return
     */
    std::string GetDataHash();

private:
    std::map<std::string, std::string> sealData;
    Signature SealSignature;
};

}
}

#endif // SEAL_H
