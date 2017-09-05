using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using sdkwrapper;
using Microsoft.IdentityModel.Clients.ActiveDirectory;
using System.Diagnostics;
using System.Web;
using System.Runtime.InteropServices;
using System.IO;

namespace ADALSampleWinForms
{
    public partial class Form3 : Form
    {
        private static PFileConverterEx pfileConverter;
        private static ADALNetAuthCallback authCallback;
        private static class NativeMethods
        {
            internal const int INTERNET_OPTION_END_BROWSER_SESSION = 42;

            [DllImport("wininet.dll", SetLastError = true)]
            internal static extern bool InternetSetOption(IntPtr hInternet, int dwOption, IntPtr lpBuffer,
                int lpdwBufferLength);
        }
        public Form3(string user)
        {
            InitializeComponent();
            authCallback = new ADALNetAuthCallback();
            pfileConverter = new PFileConverterEx(authCallback);
            emailBox.Text = user;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (!String.IsNullOrEmpty(emailBox.Text))
            {
                var FD = new System.Windows.Forms.OpenFileDialog();
                if (FD.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    bool result = false;
                    try
                    {
                        result = pfileConverter.ConvertFromPFile(FD.FileName, emailBox.Text, "com.microsoft.rms-sharing-for-win://authorize/");
                        if (!result)
                            throw new Exception("No rights.");
                        if (MessageBox.Show("Decryption successful!", "Consumption result", MessageBoxButtons.OK, MessageBoxIcon.Information) == DialogResult.OK)
                        {
                            var f = new ResultForm(File.ReadAllText(FD.FileName.Replace(".pfile", "")));
                            f.Show();
                        }
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("Failed to consume file. Reason: " + ex.Message, "Consumption result");
                    }
                }
            }
            else
                MessageBox.Show("Please enter an email.");
        }

        private void button2_Click(object sender, EventArgs e)
        {
            TokenCache.DefaultShared.Clear();
            NativeMethods.InternetSetOption(IntPtr.Zero, NativeMethods.INTERNET_OPTION_END_BROWSER_SESSION, IntPtr.Zero, 0);
        }
    }
    public class ADALNetAuthCallback : IAuthenticationCallbackEx
    {
        private string buildClaims()
        {
            string param = @"{'access_token':{'polids':{'essential':true,'values':['fc5fb17c-ecba-45df-aaa8-d799c32ddf9b']}}}";
            return "claims=" + param;//HttpUtility.UrlEncode(param);
        }
        public string GetAccessToken(string authority, string resource, string scope, string userID)
        {
            UserIdentifier userId = new UserIdentifier(userID, UserIdentifierType.OptionalDisplayableId);

            AuthenticationContext context = new AuthenticationContext(authority);
            try
            {
                Task<AuthenticationResult> task = context.AcquireTokenAsync(
                    resource,
                    "6b069eef-9dde-4a29-b402-8ce866edc897",
                    new Uri("com.microsoft.rms-sharing-for-win://authorize/"),
                    new PlatformParameters(PromptBehavior.Auto),
                    userId,
                    ""//buildClaims()
                );
                return task.Result.AccessToken;
            }
            catch (AggregateException ex)
            {
                AdalServiceException adalEx = ex.InnerException as AdalServiceException;
                if (adalEx != null && adalEx.ErrorCode == AdalError.AuthenticationCanceled)
                    return "";
                MessageBox.Show("Exception when acquring token: " + ex.InnerException);
                throw;
            }
        }
    }
}
