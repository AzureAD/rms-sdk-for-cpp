namespace SecureSenderLibrary.integrity
{
    /// <summary>
    /// Represents any property which can be held inside a DataCollection
    /// </summary>
    public interface IProperty
    {
        /// <summary>
        /// The "Key" held inside the DataCollection
        /// </summary>
        string Name { get; }
        /// <summary>
        /// The "Value" held inside the DataCollection
        /// </summary>
        string Value { get; }
    }
}
