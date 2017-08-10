namespace SecureSenderLibrary.models.message
{
    /// <summary>
    /// An info message to be displayed to the user
    /// </summary>
    public class InformationMessage
    {
        /// <summary>
        /// Message Name
        /// </summary>
        public string Name { get; protected set; }
        /// <summary>
        /// The details attached to the message
        /// </summary>
        public string Details { get; protected set; }

        public InformationMessage()
        {
            //Empty constructor for subclasses
        }

        public InformationMessage(string name, string details)
        {
            Name = name;
            Details = details;
        }
    }
}
