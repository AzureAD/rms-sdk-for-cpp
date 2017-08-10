namespace SecureSenderLibrary.ui
{
    public interface IUiDataBinder<in T>
    {
        /// <summary>
        /// Change the data this UIDataBinder will bind with
        /// </summary>
        /// <param name="newData"></param>
        void ChangeData(T newData);

        /// <summary>
        /// Binds this data to a user interface
        /// </summary>
        /// <param name="userInterface"></param>
        void BindToUserInterface(IUserInterface userInterface);
    }
}
