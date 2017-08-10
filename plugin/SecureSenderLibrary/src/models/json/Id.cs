namespace SecureSenderLibrary.models.json
{
    /// <summary>
    /// Models the ID object for an Issuer
    /// </summary>
    public class Id
    {
        /// <summary>
        /// The ID Type
        /// </summary>
        public string typ { get; set; }
        /// <summary>
        /// The actual ID
        /// </summary>
        public string val { get; set; }
    }
}
