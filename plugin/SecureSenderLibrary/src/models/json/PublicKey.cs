namespace SecureSenderLibrary.models.json
{
    /// <summary>
    /// Models the "Public Key" json object recieved in the CLC
    /// </summary>
    public class PublicKey
    {
        /// <summary>
        /// The key's exponent
        /// </summary>
        public string e { get; set; }
        /// <summary>
        /// The key type
        /// </summary>
        public string kty { get; set; }
        /// <summary>
        /// The key's modulus
        /// </summary>
        public string n { get; set; }
    }
}
