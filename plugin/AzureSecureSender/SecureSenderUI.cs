using System.Windows.Forms;
using System.Drawing;
using AzureSecureSender.extensions;
using SecureSenderLibrary.models.message;
using SecureSenderLibrary.models.table;
using SecureSenderLibrary.ui;

namespace AzureSecureSender
{
    partial class SecureSenderUI : IUserInterface
    {
        private static readonly Color INVALID_COLOR = Color.Red;
        private static readonly Color VALID_COLOR = Color.Green;


        #region Form Region Factory 

        [Microsoft.Office.Tools.Outlook.FormRegionMessageClass("IPM.Note.AzureSecure")]
        [Microsoft.Office.Tools.Outlook.FormRegionName("AzureSecureSender.SecureSenderUI")]
        public partial class SecureSenderUIFactory
        {
            // Occurs before the form region is initialized.
            // To prevent the form region from appearing, set e.Cancel to true.
            // Use e.OutlookItem to get a reference to the current Outlook item.
            private void SecureSenderUIFactory_FormRegionInitializing(object sender, Microsoft.Office.Tools.Outlook.FormRegionInitializingEventArgs e)
            {
            }
        }

        #endregion

        // Occurs before the form region is displayed.
        // Use this.OutlookItem to get a reference to the current Outlook item.
        // Use this.OutlookFormRegion to get a reference to the form region.
        private void SecureSenderUI_FormRegionShowing(object sender, System.EventArgs e)
        {
            Globals.ThisAddIn.EmailCollectionDataBinder.BindToUserInterface(this);
        }

        // Occurs when the form region is closed.
        // Use this.OutlookItem to get a reference to the current Outlook item.
        // Use this.OutlookFormRegion to get a reference to the form region.
        private void SecureSenderUI_FormRegionClosed(object sender, System.EventArgs e)
        {
        }

        public void SetTitle(string newTitle)
        {
            title.Text = newTitle;
        }

        public void ShowErrorMessage(InformationMessage message)
        {
            ShowInfoPanel(message, Color.Red);
        }

        private void ShowInfoPanel(InformationMessage message, Color color)
        {
            var boldSerif = new Font(FontFamily.GenericSansSerif, 14, FontStyle.Bold);

            var name = new Label()
            {
                Name = message.Name,
                ForeColor = color,
                Text = message.Name,
                AutoSize = true,
                Font = boldSerif
            };
            var detailsFont = new Font(FontFamily.GenericSansSerif, 10, FontStyle.Regular);
            var details = new Label()
            {
                Text = message.Details,
                AutoSize = true,
                Anchor = AnchorStyles.Bottom,
                Font = detailsFont
            };
            var newLoc = new Point(10, details.Height + 5);
            details.Location = newLoc;

            var errorPanel = new Panel()
            {
                BorderStyle = BorderStyle.FixedSingle,
                Width = messageTableLayoutPanel.Width - 5,
                Height = name.Height + details.Height + 10
            };
            errorPanel.Controls.Add(name);
            errorPanel.Controls.Add(details);
            AddToMessagesTable(errorPanel);
            topColorStrip.Width = messageTableLayoutPanel.Width - 20;
        }

        public void ShowInformationMessage(InformationMessage informationMessage)
        {
            ShowInfoPanel(informationMessage, Color.Green);
        }

        public void ShowTable(DataTable table, string[] tableHeaders)
        {
            var boldSerif = new Font(FontFamily.GenericSansSerif, 10, FontStyle.Bold);

            var tableLayoutPanel = new TableLayoutPanel()
            {
                ColumnCount = 2,
                CellBorderStyle = TableLayoutPanelCellBorderStyle.Single,
                Width = messageTableLayoutPanel.Width,
                AutoSize = true
            };
            var headerRow = new Control[tableHeaders.Length];
            for(var i = 0; i < tableHeaders.Length; i++)
            {
                var header = new Label()
                {
                    Text = tableHeaders[i],
                    Font = boldSerif,
                    Width = (messageTableLayoutPanel.Width - 10)/2,
                };
                header.Height = header.Height + 10;
                headerRow[i] = header;
            }
            tableLayoutPanel.AddTableRow(headerRow);
            foreach (var key in table.Elements.Keys)
            {
                var keyLabel = new Label();
                keyLabel.UpdateFromTableElement(key);
                var valueLabel = new Label();
                valueLabel.UpdateFromTableElement(table.Elements[key]);
                valueLabel.Height = valueLabel.Height + 10;
                var currentTableRow = new Control[] { keyLabel, valueLabel };
                tableLayoutPanel.AddTableRow(currentTableRow);
            }
            
            AddToMessagesTable(tableLayoutPanel);
        }

        private void AddToMessagesTable(Control control)
        {
            var controlRow = new Control[] { control };
            messageTableLayoutPanel.AddTableRow(controlRow);
        }

        public void ThemeToColor(Color color)
        {
            topColorStrip.BackColor = color;
            title.ForeColor = color;
            if (color == INVALID_COLOR)
                emailIcon.BackgroundImage = Properties.Resources.bad;
            if (color == VALID_COLOR)
                emailIcon.BackgroundImage = Properties.Resources.good;
        }
    }
}
