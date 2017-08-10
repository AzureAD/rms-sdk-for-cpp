namespace SecureSenderLibrary.models.json
{
    /// <summary>
    /// Models the "Issuer" JSON object in the CLC
    /// </summary>
    public class Issuer
    {
        /// <summary>
        /// Extranet URL
        /// </summary>
        public string eurl { get; set; }
        /// <summary>
        /// Friendly Name
        /// </summary>
        public string fname { get; set; }
        /// <summary>
        /// Issuer ID
        /// </summary>
        public string id { get; set; }
        /// <summary>
        /// Intranet URL
        /// </summary>
        public string iurl { get; set; }
        /// <summary>
        /// Issuer's public key
        /// </summary>
        public PublicKey pubk { get; set; }
    }
}
