using SecureSenderLibrary.core;
using SecureSenderLibrary.helper;
using SecureSenderLibrary.models;

namespace SecureSenderLibrary
{
    public class EmailSigner : IEmailSigner
    {
        private readonly IHeaderFolder _headerFolder;
        private readonly IAuthenticationCallbackExp _authenticationCallback;
        private readonly string _clientId;
        private readonly IHasher _hasher;

        public EmailSigner(IAuthenticationCallbackExp authenticationCallback, string clientId)
        {
            _authenticationCallback = authenticationCallback;
            _clientId = clientId;
            _headerFolder = new HeaderFolder();
            _hasher = new Sha256Hasher();
        }

        public IEmail SignEmail(IEmail original, string userEmail)
        {
            IWritableEmailCollection writableEmailCollection = new WritableEmailCollection(userEmail, _authenticationCallback, _clientId);
            writableEmailCollection.AddEmail(original, _hasher);
            var emailCollectionJson = writableEmailCollection.SignAndSerializeData();
            _headerFolder.FoldStringIntoHeader(original, emailCollectionJson, original.GetMimeHeader());
            return original;
        }
    }
}
