using System;
using SecureSenderLibrary.models;

namespace SecureSenderLibrary.helper
{
    public class HeaderFolder : IHeaderFolder
    {
        /// <summary>
        /// Folds a string into headers
        /// </summary>
        /// <param name="email"></param>
        /// <param name="stringToFold"></param>
        /// <param name="header"></param>
        public void FoldStringIntoHeader(IEmail email, string stringToFold, string header)
        {
            if (email == null || stringToFold == null || header == null)
                throw new ArgumentNullException();
            var headerCount = 0;
            var characterLocation = 0;
            while (characterLocation < stringToFold.Length)
            {
                var dataToAdd = "";
                for (var i = 0; i < 64 && characterLocation < stringToFold.Length; i++)
                {
                    dataToAdd += stringToFold[characterLocation++];
                }
                email.SetHeaderField(header + "-" + headerCount++, dataToAdd);
            }
        }

        /// <summary>
        /// Unfolds a string from headers
        /// </summary>
        /// <param name="email"></param>
        /// <param name="header"></param>
        /// <returns></returns>
        public string UnfoldStringFromHeader(IEmail email, string header)
        {
            if(email == null || header == null)
                throw new ArgumentNullException();
            if (!email.HasHeaderField(header + "-0"))
                return null;
            var headerData = "";
            var currentFold = 0;
            while(email.HasHeaderField(header + "-" + currentFold))
            {
                headerData += email.GetHeaderField(header + "-" + currentFold++);
            }
            return headerData;
        }
    }
}
