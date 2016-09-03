#ifndef OFFLINEPUBLISHTEST_H
#define OFFLINEPUBLISHTEST_H

#include <QObject>
#include <QTest>
#include "../../../rmscrypto_sdk/CryptoAPI/ICryptoProvider.h"
#include "../../RestClients/PublishClient.h"
#include "DummyAuthCallback.h"

namespace rmscore {
namespace restclients {
class OfflinePublishTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testCreatePLWithTemplate();
    void testCreatePLCustom();
    void testEncryptBytes();
    static std::vector<uint8_t> EncryptBytesToBase64Wrapper(PublishClient obj, vector<uint8_t> bytesToEncrypt, vector<uint8_t> key, rmscrypto::api::CipherMode cm)
    {
        return obj.EncryptBytesToBase64(bytesToEncrypt, key, cm);
    }
    static shared_ptr<rmscrypto::api::IRSAKeyBlob> RSAKB(PublishClient obj)
    {
        return obj.rsaKeyBlob;
    }

private:
    static std::string generateMockCLC(std::string email, std::string &outCLC);
};

}
}
#endif // OFFLINEPUBLISHTEST_H
