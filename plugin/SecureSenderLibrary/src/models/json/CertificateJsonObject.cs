namespace SecureSenderLibrary.models.json
{
    /// <summary>
    /// Models a JSON object for a certificate
    /// </summary>
    public class CertificateJsonObject
    {
        /// <summary>
        /// The payload attached to a certificate
        /// </summary>
        public CertificatePayload pld { get; set; }
    }
}
