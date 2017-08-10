using Outlook = Microsoft.Office.Interop.Outlook;
using Microsoft.Office.Interop.Outlook;
using SecureSenderLibrary.core;
using SecureSenderLibrary.integrity;
using SecureSenderLibrary.models;
using SecureSenderLibrary.ui;

namespace AzureSecureSender
{
    public partial class ThisAddIn
    {
        public static string UserEmail;
        Outlook.Explorer _currentExplorer = null;
        public IUiDataBinder<DataIntegrityCheck<EmailProperty>> EmailCollectionDataBinder;

        private void ThisAddIn_Startup(object sender, System.EventArgs e)
        {
            UserEmail = Globals.ThisAddIn.Application.ActiveExplorer().Session.CurrentUser.AddressEntry.GetExchangeUser().PrimarySmtpAddress;
            EmailCollectionDataBinder = new DataIntegrityCheckBinder();
            _currentExplorer = this.Application.ActiveExplorer();
            Application.ItemSend += Application_ItemSend;
            _currentExplorer.SelectionChange += CurrentExplorer_Event;
        }

        private void CurrentExplorer_Event()
        {
            if (this.Application.ActiveExplorer().Selection.Count <= 0) return;
            object selObject = this.Application.ActiveExplorer().Selection[1];
            if (!(selObject is MailItem)) return;
            var mailItem = selObject as Outlook.MailItem;
            var emailItem = new EmailItem(mailItem);
            var dataIntegrityCheck = EmailHelper.MakeDataCheckForEmail(emailItem);
            if (dataIntegrityCheck.Status == IntegrityStatus.Missing) return;
            mailItem.MessageClass = "IPM.Note.AzureSecure";
            EmailCollectionDataBinder.ChangeData(dataIntegrityCheck);
        }

        void Application_ItemSend(object item, ref bool cancel)
        {
            var mail = item as Outlook.MailItem;
            if (mail != null)
            {
                EmailHelper.SignEmail(new EmailItem(mail), null, UserEmail);
            }
        }


        private void ThisAddIn_Shutdown(object sender, System.EventArgs e)
        {
            // Note: Outlook no longer raises this event. If you have code that 
            //    must run when Outlook shuts down, see https://go.microsoft.com/fwlink/?LinkId=506785
        }

        #region VSTO generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InternalStartup()
        {
            this.Startup += new System.EventHandler(ThisAddIn_Startup);
            this.Shutdown += new System.EventHandler(ThisAddIn_Shutdown);
        }
        
        #endregion
    }
}
