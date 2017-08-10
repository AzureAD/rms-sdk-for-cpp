#ifndef ISTRINGSIGNER_H
#define ISTRINGSIGNER_H

#include "Platform/Json/IJsonObject.h"
using namespace rmscore::platform::json;

namespace rmscore{
namespace core {

/**
 * @brief Represents a signature on a Seal
 */
struct Signature{
public:
    std::string SignedDataHash; //The signed data in the signature
    std::string SigningAlgorithm; //Signing algorithm used for the signature
    std::shared_ptr<IJsonObject> Certificate; //Certificate used to sign this signature
};


/**
 * @brief The IStringSigner interface generates a signature object by signing a string
 */
class IStringSigner{

public:

    /**
     * @brief Initializes a new StringSigner
     * @param Certificate
     * @return
     */
    static std::shared_ptr<IStringSigner> MakeStringSigner(std::shared_ptr<IJsonObject> Certificate);

    /**
     * @brief Creates a new signature
     * @param data
     * @return
     */
    virtual Signature GenerateSignature(std::string& data) = 0;

};

}
}
#endif // ISTRINGSIGNER_H
