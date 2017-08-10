namespace SecureSenderLibrary.core
{
    /// <summary>
    /// Handles the hashing of all strings
    /// </summary>
    public interface IHasher
    {
        /// <summary>
        /// Creates a hash for data
        /// </summary>
        /// <param name="original"></param>
        /// <returns></returns>
        string Hash(string original);

        /// <summary>
        /// The name of the algorithm used for the hasing
        /// </summary>
        string Algorithm { get; }
    }
}
