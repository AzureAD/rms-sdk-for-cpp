namespace SecureSenderLibrary.models
{
    public enum IntegrityStatus
    {
        /// <summary>
        /// Data Colleciton was not present in the JSON
        /// </summary>
        Missing,
        /// <summary>
        /// One or more values in the Data Collection don't match the actual data
        /// </summary>
        DataMismatch,
        /// <summary>
        /// Data Collection is verified and all set
        /// </summary>
        Verified,
        /// <summary>
        /// A Data Collection is present, but it's missing some values making verfication impossible
        /// </summary>
        Malformed,
        /// <summary>
        /// rms_sdk_cpp failed validation of the data collection
        /// </summary>
        FailedServerValidation
    }
}
