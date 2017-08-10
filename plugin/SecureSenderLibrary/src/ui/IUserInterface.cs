using System.Drawing;
using SecureSenderLibrary.models.message;
using SecureSenderLibrary.models.table;

namespace SecureSenderLibrary.ui
{
    /// <summary>
    /// Models a user interface, capable of displaying various data to the user
    /// </summary>
    public interface IUserInterface
    {
        /// <summary>
        /// Sets the title of the user interface
        /// </summary>
        /// <param name="title"></param>
        void SetTitle(string title);

        /// <summary>
        /// Displays an error to the user
        /// </summary>
        /// <param name="errorMessage"></param>
        void ShowErrorMessage(InformationMessage errorMessage);

        /// <summary>
        /// Displays some non-error information to the user
        /// </summary>
        /// <param name="informationMessage"></param>
        void ShowInformationMessage(InformationMessage informationMessage);

        /// <summary>
        /// Shows a table to the user
        /// </summary>
        /// <param name="table"></param>
        /// <param name="tableHeaders"></param>
        void ShowTable(DataTable table, string[] tableHeaders);

        /// <summary>
        /// Changes various UI elements to match a certain color
        /// </summary>
        /// <param name="color"></param>
        void ThemeToColor(Color color);
    }
}
