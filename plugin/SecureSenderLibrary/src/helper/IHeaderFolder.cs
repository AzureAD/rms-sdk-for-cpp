using SecureSenderLibrary.models;

namespace SecureSenderLibrary.helper
{
    /// <summary>
    /// Handles extracting/inserting long strings into an email using header folding
    /// </summary>
    public interface IHeaderFolder
    {
        /// <summary>
        /// Folds a string into the email's headers
        /// </summary>
        /// <param name="email">Email to perform insertion into</param>
        /// <param name="stringToFold">String to split up</param>
        /// <param name="header">Name of header to perform the folding into</param>
        void FoldStringIntoHeader(IEmail email, string stringToFold, string header);

        /// <summary>
        /// Unfolds a multipart string using the header specified into a string
        /// </summary>
        /// <param name="email"></param>
        /// <param name="header"></param>
        /// <returns></returns>
        string UnfoldStringFromHeader(IEmail email, string header);

    }
}
