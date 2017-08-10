using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace SecureSenderLibrary.models.table
{
    /// <summary>
    /// Models some data table that should be displayed to the user
    /// </summary>
    public class DataTable
    {
        public string Title { get; protected set; }
        public IDictionary<TableElement, TableElement> Elements { get; protected set; }

        public DataTable()
        {
            //Generic Constructor
        }

        public DataTable(string title, IDictionary<TableElement, TableElement> elements)
        {
            Title = title;
            Elements = new ReadOnlyDictionary<TableElement, TableElement>(elements);
        }
    }
}
