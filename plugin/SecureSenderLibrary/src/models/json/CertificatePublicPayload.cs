namespace SecureSenderLibrary.models.json
{
    /// <summary>
    /// The payload for the public part of the certificate
    /// </summary>
    public class CertificatePublicPayload
    {
        /// <summary>
        /// The Issuee
        /// </summary>
        public IssuedTo issto { get; set; }
        /// <summary>
        /// The issuer
        /// </summary>
        public Issuer iss { get; set; }
    }
}
