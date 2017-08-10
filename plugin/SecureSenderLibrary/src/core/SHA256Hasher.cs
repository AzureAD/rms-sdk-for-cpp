using System;
using System.Security.Cryptography;
using System.Text;

namespace SecureSenderLibrary.core
{
    /// <summary>
    /// Implementation of IHasher - hashes using the SHA-256 Hashing Algorithm
    /// </summary>
    public class Sha256Hasher : IHasher
    {

        private readonly SHA256CryptoServiceProvider _serviceProvider;

        public Sha256Hasher()
        {
            _serviceProvider = new SHA256CryptoServiceProvider();
        }

        public string Algorithm => "SHA256";

        public string Hash(string original)
        {
            if (original == null)
                return null;
            var soureInBytes = Encoding.ASCII.GetBytes(original);
            var hashInBytes = _serviceProvider.ComputeHash(soureInBytes);
            return BitConverter.ToString(hashInBytes).Replace("-", "");
        }
    }
}
