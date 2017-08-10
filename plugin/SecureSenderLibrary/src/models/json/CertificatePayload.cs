namespace SecureSenderLibrary.models.json
{
    /// <summary>
    /// The actual data attached to a certificate
    /// </summary>
    public class CertificatePayload
    {
        /// <summary>
        /// The public data attached to a certificate
        /// </summary>
        public CertificatePublicData pub { get; set; }
    }
}
