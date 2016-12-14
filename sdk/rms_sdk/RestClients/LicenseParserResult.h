#ifndef LICENSEPARSERRESULT_H
#define LICENSEPARSERRESULT_H

#include "Domain.h"
#include <vector>
#include <memory>

namespace rmscore {
namespace restclients {

class LicenseParserResult
{
public:

    LicenseParserResult(std::vector<std::shared_ptr<Domain>>& vDomains):
        LicenseParserResult(vDomains, std::shared_ptr<std::string>()){}

    LicenseParserResult(const std::vector<std::shared_ptr<Domain>> vDomains,
        const std::shared_ptr<std::string>& pServerPublicCertificate):
            m_vDomains(vDomains),
            m_pServerPublicCertificate(pServerPublicCertificate){}

    const std::vector<std::shared_ptr<Domain>>& GetDomains() { return m_vDomains; }
    const std::shared_ptr<std::string> GetServerPublicCertificate() { return m_pServerPublicCertificate; }
private:
    const std::vector<std::shared_ptr<Domain>> m_vDomains;
    const std::shared_ptr<std::string> m_pServerPublicCertificate;

};


}
}
#endif // LICENSEPARSERRESULT_H
