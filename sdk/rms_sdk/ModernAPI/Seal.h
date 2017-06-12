#ifndef SEAL_H
#define SEAL_H

#include "ModernAPIExport.h"
#include <string>
#include <map>

namespace rmscore{
namespace modernapi{

/**
 * @brief Represents a signature on a Seal
 */
struct DLL_PUBLIC_RMS Signature{
public:
    std::string Value; //The signed data in the signature
    std::string SigningAlgorithm; //Signing algorithm used for the signature
};

/**
 * @brief The Seal class
 *
 * General usage for Seal
 *
 * When signing, Seal() is called. Insert in values to the seal using AddValue. When ready to sign, use ToJSONString() to get the JSON for
 * the seal. Sign that string, and pass the signature back to the seal using AddSignature().
 *
 * When verifying, use GetSignature() to get the signature on the seal. Use ToJSONString() to get the Seal's JSON. Check these two against
 * each other the verify the seal is attached to the signature. Next, check every desired data value using GetValue().
 *
 */

class DLL_PUBLIC_RMS Seal
{
public:
    Seal();
    /**
     * @brief Creates a new seal from a JSON string. Parses the string and populates it's members accordingly
     * @param SealJSON - JSON String to parse
     */
    Seal(std::string SealJSON);
    std::string AddValue(std::string Key, std::string Value);
    std::string GetValue(std::string Key);
    bool HasKey(std::string Key);
    void AddSignature(Signature SealSignature);
    Signature GetSignature();
    std::string ToJSONString();

private:
    std::map<std::string, std::string> sealData;
    Signature SealSignature;

};

}
}

#endif // SEAL_H
