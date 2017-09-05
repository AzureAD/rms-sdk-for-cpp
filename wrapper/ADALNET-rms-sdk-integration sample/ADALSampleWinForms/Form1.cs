using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Web;

using Microsoft.IdentityModel.Clients.ActiveDirectory;

namespace ADALSampleWinForms {
  public partial class Form1 : Form {
    private const string CommonEndPoint = "https://login.windows.net/common";
    private const string CommonResource = "https://api.aadrm.com";

    private const string DEEndPoint = "https://login.microsoftonline.de/common";
    private const string DEResource = "https://api.aadrm.de";

    private const string TenantID = "283c1e74-e4f8-4e34-af9f-dbbcad90b51b";
    private const string UserID = "user1@testbfoffice.onmicrosoft.de";
    private const string UserPassword = "Pa$$w0rd";
    private const string ClientID = "6b069eef-9dde-4a29-b402-8ce866edc897"; //clientid of app in AzureAD that is authorized to access Azure Service Management API
    private const string RedirectUri = "com.microsoft.rms-sharing-for-win://authorize/";

    public Form1() {
      Environment.SetEnvironmentVariable("ExtraQueryParameter", "slice=TestSlice");
      InitializeComponent();
    }

    private String computeAuthority(String email) {
      int index = email.IndexOf('@');
      if (index == -1) return null;

      string domain = email.Substring(index + 1);
      string result = lookupDomain(domain);
      index = result.IndexOf("\"authorization_endpoint\"");
      if (index < 0) return null;
      int startIndex = result.IndexOf("https", index);
      if (startIndex < 0) return null;
      int endIndex = result.IndexOf("/authorize", startIndex);
      return result.Substring(startIndex, endIndex - startIndex);
    }

    private String lookupDomain(String domain) {
      String urlFormat = @"https://login.microsoftonline.com/{0}/.well-known/openid-configuration";
      String url = String.Format(urlFormat, domain);
      using (HttpClient client = new HttpClient()) {
        string result = client.GetStringAsync(url).Result;
        return result;
      }
    }

    private void button1_Click(object sender, EventArgs e) {
      EmailAddressDialog emailDialog = new EmailAddressDialog();
      DialogResult dialogResult = emailDialog.ShowDialog();
      if (dialogResult != DialogResult.OK) return;
      String email = emailDialog.EmailAddress;
      String authority = computeAuthority(emailDialog.EmailAddress);
      if (authority == null) {
        MessageBox.Show("Could not auto-detect authority for email " + email);
        return;
      }

      AuthenticationContext context = new AuthenticationContext(authority);
      Task<AuthenticationResult> task = context.AcquireTokenAsync(
          CommonResource,
          ClientID,
          new Uri(RedirectUri),
          new PlatformParameters(PromptBehavior.Always),
          new UserIdentifier(email, UserIdentifierType.OptionalDisplayableId));
      AuthenticationResult result = task.Result;
      displayToken(result.AccessToken);
    }

    private void Form1_Load(object sender, EventArgs e) {
    }

    private void button2_Click(object sender, EventArgs e) {
      String tenant = textBox1.Text.Trim();
      if (String.IsNullOrEmpty(tenant)) return;
      AuthenticationContext context = new AuthenticationContext("https://login.windows.net/" + tenant);
      ClientCredential cred = new ClientCredential("545c615c-66f9-4711-930f-b5f1646d0bc8", "fxbfE/muZvQguONiYEJW2+MLMqRV231N6bcVegbTA0s=");
      Task<AuthenticationResult> task = context.AcquireTokenAsync("https://api.aadrm.com", cred);
      AuthenticationResult result = task.Result;
      displayToken(result.AccessToken);
    }

    private void displayToken(String token) {
      tokenTextBox.Text = token;
      string[] tokenParts = token.Split('.');
      headerTextBox.Text = tokenParts[0];
      bodyTextBox.Text = tokenParts[1];
      signatureTextBox.Text = tokenParts[2];
      Console.WriteLine(token);
    }

    private void button3_Click(object sender, EventArgs e) {
      AuthenticationContext context = new AuthenticationContext(CommonEndPoint);
      UserPasswordCredential pwdCreds = new UserPasswordCredential("alice@isvtenant998.onmicrosoft.com", "A123qwe!!!");
      Task<AuthenticationResult> task = context.AcquireTokenAsync(
          CommonResource,
          ClientID,
          pwdCreds);
      displayToken(task.Result.AccessToken);
    }
  }
}
