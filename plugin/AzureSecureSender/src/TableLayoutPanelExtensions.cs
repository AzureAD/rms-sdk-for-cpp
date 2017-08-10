using System.Windows.Forms;

namespace AzureSecureSender.extensions
{
    public static class TableLayoutPanelExtensions
    {
        /// <summary>
        /// Extension method which automatically adds in a "Row" of controls to a table layout panel, and handles expansion accordingly
        /// </summary>
        /// <param name="tableLayoutPanel"></param>
        /// <param name="controlRow"></param>
        public static void AddTableRow(this TableLayoutPanel tableLayoutPanel, Control[] controlRow)
        {
            int currentRow = tableLayoutPanel.RowCount;

            //Expand if we can't currently fit this row
            if (tableLayoutPanel.ColumnCount < controlRow.Length)
                tableLayoutPanel.ColumnCount = controlRow.Length;

            for (int columnIndex = 0; columnIndex < controlRow.Length; columnIndex++)
            {
                tableLayoutPanel.Controls.Add(controlRow[columnIndex]);
                tableLayoutPanel.SetColumn(controlRow[columnIndex], columnIndex);
                tableLayoutPanel.SetRow(controlRow[columnIndex], currentRow);
            }
            tableLayoutPanel.RowCount += 1;
        }
    }
}
