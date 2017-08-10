using System.Drawing;
using System.Windows.Forms;
using SecureSenderLibrary.models.table;

namespace AzureSecureSender.extensions
{
    public static class LabelExtensions
    {
        public static void UpdateFromTableElement(this Label label, TableElement tableElement)
        {
            label.Text = tableElement.Value;
            label.ForeColor = tableElement.TextColor;
            var font = new Font(label.Font, tableElement.Bold ? FontStyle.Bold : FontStyle.Regular);
            label.Font = font;
        }
    }
}
