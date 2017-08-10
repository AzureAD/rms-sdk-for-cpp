using System.Collections.Generic;
using rmscore.wrapper;
using SecureSenderLibrary.core;

namespace SecureSenderLibrary.integrity
{
    public class SignatureCreator<T> where T : IProperty
    {
        private readonly string _userEmail;
        private readonly IAuthenticationCallbackExp _authenticationCallback;
        private readonly IHasher _hasher;

        public SignatureCreator(string userEmail, IHasher hasher, IAuthenticationCallbackExp authenticationCallback)
        {
            _userEmail = userEmail;
            _authenticationCallback = authenticationCallback;
            _hasher = hasher;
        }

        public string GetSignature(IEnumerable<T> properties)
        {
            var writableDataCollection = new WritableDataCollectionExp(_userEmail, _authenticationCallback, Resources.client_id);
            foreach(var property in properties)
                writableDataCollection.AddDataValue(property.Name, _hasher.Hash(property.Value));
            writableDataCollection.AddDataValue(Resources.hash_algorithm_key, _hasher.Algorithm);
            return writableDataCollection.SignAndSerializeData();
        }
    }
}
