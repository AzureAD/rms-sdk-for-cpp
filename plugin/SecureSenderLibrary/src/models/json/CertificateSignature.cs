namespace SecureSenderLibrary.models.json
{
    /// <summary>
    /// Models the "Signature" object attached to a certificate
    /// </summary>
    public class CertificateSignature
    {
        /// <summary>
        /// Algorithm used for signature
        /// </summary>
        public string alg { get; set; }
        /// <summary>
        /// Signature digest
        /// </summary>
        public string dig { get; set; }
        /// <summary>
        /// Encoding type for signature
        /// </summary>
        public string penc { get; set; }
    }
}
