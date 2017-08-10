namespace SecureSenderLibrary.core
{
    /// <summary>
    /// Handles pulling different hashers for algorithms
    /// </summary>
    public class HasherManager
    {
        public IHasher GetHasherForAlgorithm(string algorithm)
        {
            return algorithm.Equals("SHA256") ? new Sha256Hasher() : null;
        }
    }
}
