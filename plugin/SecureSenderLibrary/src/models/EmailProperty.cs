using SecureSenderLibrary.integrity;

namespace SecureSenderLibrary.models
{
    public class EmailProperty: IProperty
    {
        public string Name { get; }
        public string Value { get; }
        public string DisplayName { get; }

        public EmailProperty(string name, string value, string displayName)
        {
            Name = name;
            Value = value;
            DisplayName = displayName;
        }
    }
}
