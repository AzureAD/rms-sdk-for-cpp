using System.Collections.Generic;
using SecureSenderLibrary.core;
using SecureSenderLibrary.models;

namespace VSTOSecureSenderPlugInTests.src
{
    /// <summary>
    /// Dummy implementation of an email around a dictionary; allows for easy testing
    /// </summary>
    public class TestEmail : IEmail
    {
        private Dictionary<string, string> headers;
        private string body;
        public IHasher passedHasher;

        public TestEmail()
        {
            body = "Email Body";
            headers = new Dictionary<string, string>();
        }
        public string GetHeaderField(string title)
        {
            return headers[title];
        }

        public string GetMimeHeader()
        {
            return "test-json-name";
        }

        public bool HasHeaderField(string title)
        {
            return headers.ContainsKey(title);
        }

        public void SetHeaderField(string title, string value)
        {
            headers[title] = value;
        }

        public IList<EmailProperty> EmailPropertiesList => new List<EmailProperty> {new EmailProperty("body", body, "body")};
    }
}
