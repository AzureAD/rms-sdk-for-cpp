using SecureSenderLibrary.integrity;
using SecureSenderLibrary.models;
using SecureSenderLibrary.models.message;
using SecureSenderLibrary.models.table;

namespace SecureSenderLibrary.ui
{
    /// <summary>
    /// Binds an IDataIntegrityCheck object to a user interface
    /// </summary>
    public class DataIntegrityCheckBinder : IUiDataBinder<DataIntegrityCheck<EmailProperty>>
    {
        private DataIntegrityCheck<EmailProperty> _integrityCheck;

        public void BindToUserInterface(IUserInterface userInterface)
        {
            if (_integrityCheck.Status == IntegrityStatus.Missing)
                return; //If there isn't a signature, don't do anything
            userInterface.SetTitle(_integrityCheck.Status != IntegrityStatus.Verified
                ? Resources.unverified_title
                : Resources.verified_title);
            if (_integrityCheck.Status != IntegrityStatus.Verified)
                userInterface.ShowErrorMessage(new EmailErrorMessage(_integrityCheck.Status));
            else
                userInterface.ShowInformationMessage(new CertificateInformationMessage(_integrityCheck.Certificate));
            userInterface.ThemeToColor(_integrityCheck.Status == IntegrityStatus.Verified
                ? System.Drawing.Color.Green
                : System.Drawing.Color.Red);
            userInterface.ShowTable(new EmailPropertiesDataTable(_integrityCheck), 
                new[] { Resources.properties_table_key_header, Resources.properties_table_value_header });
        }

        public void ChangeData(DataIntegrityCheck<EmailProperty> newData)
        {
            _integrityCheck = newData;
        }
    }
}
