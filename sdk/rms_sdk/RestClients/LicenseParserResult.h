#ifndef LICENSEPARSERRESULT_H
#define LICENSEPARSERRESULT_H

#include "Domain.h"
#include <vector>
#include <memory>
using namespace std;
namespace rmscore {
namespace restclients {

class LicenseParserResult
{
public:
    LicenseParserResult(vector<shared_ptr<Domain>>& vDomains): LicenseParserResult(vDomains, shared_ptr<string>()){}

    LicenseParserResult(const vector<shared_ptr<Domain>> vDomains,
                        const shared_ptr<string>& pServerPublicCertificate):
                        m_vDomains(vDomains),
                        m_pServerPublicCertificate(pServerPublicCertificate){}

    const vector<shared_ptr<Domain>>& GetDomains() { return m_vDomains; }
    const shared_ptr<string> GetServerPublicCertificate() { return m_pServerPublicCertificate; }
private:
    const vector<shared_ptr<Domain>> m_vDomains;
    const shared_ptr<string> m_pServerPublicCertificate;

};


}
}
#endif // LICENSEPARSERRESULT_H
