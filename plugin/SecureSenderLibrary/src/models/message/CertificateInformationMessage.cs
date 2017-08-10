using SecureSenderLibrary.models.json;

namespace SecureSenderLibrary.models.message
{
    public class CertificateInformationMessage: InformationMessage
    {
        public CertificateInformationMessage(CertificateJsonObject certificateInformation)
        {
            Name = Resources.certificate_info_title;
            Details = Resources.certificate_info_details;
            Details = Details.Replace("$1", certificateInformation.pld.pub.pld.issto.em);
            Details = Details.Replace("$2", certificateInformation.pld.pub.pld.iss.fname);
        }
    }
}
