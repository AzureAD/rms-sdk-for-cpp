namespace SecureSenderLibrary.models.json
{
    /// <summary>
    /// The public data for a signature
    /// </summary>
    public class CertificatePublicData
    {
        /// <summary>
        /// The payload attached to the certificate
        /// </summary>
        public CertificatePublicPayload pld { get; set; }
        /// <summary>
        /// The signature attached to the certificate
        /// </summary>
        public CertificateSignature sig { get; set; }
    }
}
