using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using Microsoft.Office.Interop.Outlook;
using SecureSenderLibrary.core;
using SecureSenderLibrary.models;

namespace AzureSecureSender
{
    public class EmailItem : IEmail
    {
        private readonly MailItem _mailItem;
        private const string PrAttachDataBin = "http://schemas.microsoft.com/mapi/proptag/0x37010102";
        private const string HeaderRegex = @"^(?<header_key>[-A-Za-z0-9]+)(?<seperator>:[ \t]*)" + "(?<header_value>([^\r\n]|\r\n[ \t]+)*)(?<terminator>\r\n)";
        private const string TransportMessageHeadersSchema = "http://schemas.microsoft.com/mapi/proptag/0x007D001E";

        public EmailItem(MailItem mailItem)
        {
            _mailItem = mailItem;
        }

        public string GetHeaderField(string headerFieldName)
        {
            var headers = HeaderLookup(_mailItem);
            if (!headers.Contains(headerFieldName)) return null;
            var array = headers[headerFieldName].ToArray();
            return array.Aggregate("", (current, item) => current + item);
        }

        public bool HasHeaderField(string headerFieldName)
        {
            return GetHeaderField(headerFieldName) != null;
        }

        public void SetHeaderField(string headerFieldName, string headerFieldValue)
        {
            var propertyTitle = "http://schemas.microsoft.com/mapi/string/{00020386-0000-0000-C000-000000000046}/" + headerFieldName;
            _mailItem.PropertyAccessor.SetProperty(propertyTitle, headerFieldValue);
        }

        public IList<EmailProperty> EmailPropertiesList
        {
            get
            {
                var properties = new List<EmailProperty>
                {
                    new EmailProperty(Resources.body_hash_key, _mailItem.HTMLBody, Resources.body_hash_display_name),
                    new EmailProperty(Resources.subject_key, _mailItem.Subject, Resources.subject_display_name),
                    new EmailProperty(Resources.sender_email_address_key, _mailItem.Subject, Resources.sender_email_address_display_name),
                    new EmailProperty(Resources.attachments_count_key, "" + _mailItem.Attachments.Count, Resources.attachments_count_display_name)
                };
                for (var i = 1; i <= _mailItem.Attachments.Count; i++)
                {
                    var attachment = _mailItem.Attachments[i];
                    var attachmentData = attachment.PropertyAccessor.GetProperty(PrAttachDataBin);
                    var attachmentText = System.Text.Encoding.UTF32.GetString(attachmentData);
                    properties.Add(new EmailProperty(Resources.attachments_hash_key + i, attachmentText, Resources.attachments_hash_display_name));
                }
                return properties;
            }
        }

        public static ILookup<string, string> HeaderLookup(MailItem mailItem)
        {
            var headerString = HeaderString(mailItem);
            var headerMatches = Regex.Matches
            (headerString, HeaderRegex, RegexOptions.Multiline).Cast<Match>();
            return headerMatches.ToLookup(
             h => h.Groups["header_key"].Value,
             h => h.Groups["header_value"].Value);
        }

        public static string HeaderString(MailItem mailItem)
        {
            return (string)mailItem.PropertyAccessor.GetProperty(TransportMessageHeadersSchema);
        }
    }
}
