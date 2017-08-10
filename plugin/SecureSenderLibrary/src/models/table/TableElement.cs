using System.Drawing;

namespace SecureSenderLibrary.models.table
{
    public class TableElement
    {
        public string Value { get; }
        public Color TextColor { get; }
        public bool Bold { get; }

        public TableElement(string value, Color color, bool bold)
        {
            Value = value;
            TextColor = color;
            Bold = bold;
        }
    }
}
