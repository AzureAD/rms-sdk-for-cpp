using System.Drawing;
using System.Linq;
using SecureSenderLibrary.integrity;

namespace SecureSenderLibrary.models.table
{
    public class EmailPropertiesDataTable: DataTable
    {
        public EmailPropertiesDataTable(DataIntegrityCheck<EmailProperty> dataIntegrityCheck)
        {
            if (dataIntegrityCheck == null || dataIntegrityCheck.UnverifiedProperties == null)
                return;
            Elements = dataIntegrityCheck.UnverifiedProperties
                            .ToDictionary(emailProperty => new TableElement(emailProperty.DisplayName, Color.Black, true),
                            emailProperty => new TableElement(Resources.property_failed_verification_text, Color.Red, true));
            foreach (var emailProperty in dataIntegrityCheck.VerifiedProperties)
            {
                Elements.Add(new TableElement(emailProperty.DisplayName, Color.Black, true),
                                new TableElement(Resources.property_verified_text, Color.Green, false));
            }
        }
    }
}
