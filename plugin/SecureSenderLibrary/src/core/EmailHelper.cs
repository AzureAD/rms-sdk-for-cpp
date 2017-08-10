using SecureSenderLibrary.helper;
using SecureSenderLibrary.integrity;
using SecureSenderLibrary.models;

namespace SecureSenderLibrary.core
{
    /// <summary>
    /// Provides basic access to email validation/signing methods
    /// </summary>
    public class EmailHelper
    {
        private static readonly HeaderFolder HeaderFolder = new HeaderFolder();

        /// <summary>
        /// Get details on the integrity of an email
        /// </summary>
        /// <param name="email">Email to check</param>
        /// <returns>DataIntegrityCheck containing details of email integrity</returns>
        public static DataIntegrityCheck<EmailProperty> MakeDataCheckForEmail(IEmail email)
        {
            var emailProperties = email.EmailPropertiesList;
            var dataCollection = HeaderFolder.UnfoldStringFromHeader(email, Resources.mime_header);
            return new DataIntegrityCheck<EmailProperty>(dataCollection, emailProperties);
        }

        /// <summary>
        /// Generate a digital signature for the email, and attach it back into the email's headers
        /// </summary>
        /// <param name="email">Email to sign</param>
        /// <param name="authenticationCallback">AuthCallback, used to obtain a CLC for signing if one isn't cached</param>
        /// <param name="userEmail">Email of current sender</param>
        /// <returns>email with signature attached to headers</returns>
        public static IEmail SignEmail(IEmail email, IAuthenticationCallbackExp authenticationCallback,
            string userEmail)
        {
            var emailSignatureCreator = new SignatureCreator<EmailProperty>(userEmail, new Sha256Hasher(), authenticationCallback);
            var emailProperties = email.EmailPropertiesList;
            var dataCollection = emailSignatureCreator.GetSignature(emailProperties);
            HeaderFolder.FoldStringIntoHeader(email, dataCollection, Resources.mime_header);
            return email;
        }
    }
}
