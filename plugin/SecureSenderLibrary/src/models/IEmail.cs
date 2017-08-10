using System.Collections.Generic;

namespace SecureSenderLibrary.models
{
    /// <summary>
    /// Abstraction of an email
    /// </summary>
    public interface IEmail
    {
        /// <summary>
        /// Checks if a field is within the MIME headers
        /// </summary>
        /// <param name="headerFieldName">The MIME header name</param>
        /// <returns>Whether the email contains the specified header field</returns>
        bool HasHeaderField(string headerFieldName);
        /// <summary>
        /// Gets the value of a header field
        /// </summary>
        /// <param name="headerFieldName">The name of the header field</param>
        /// <returns>The value of the header field, or null if it doesn't exist</returns>
        string GetHeaderField(string headerFieldName);
        /// <summary>
        /// Sets a header field to a value, adding the header field if it doesn't exist
        /// </summary>
        /// <param name="headerFieldName">The name of the MIME header field</param>
        /// <param name="headerFieldValue">The value of the MIME header field</param>
        void SetHeaderField(string headerFieldName, string headerFieldValue);
        /// <summary>
        /// A list of properties attached to an email, such as the body or the subject line
        /// </summary>
        IList<EmailProperty> EmailPropertiesList { get; }

    }
}
