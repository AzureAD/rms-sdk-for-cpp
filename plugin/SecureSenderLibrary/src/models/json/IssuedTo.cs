namespace SecureSenderLibrary.models.json
{
    /// <summary>
    /// Models the "IssuedTo" JSON object
    /// </summary>
    public class IssuedTo
    {
        /// <summary>
        /// The email for the issuee
        /// </summary>
        public string em { get; set; }
        /// <summary>
        /// Issuee ID
        /// </summary>
        public Id id { get; set; }
        /// <summary>
        /// Issuee public key
        /// </summary>
        public PublicKey pubk { get; set; }
    }
}
