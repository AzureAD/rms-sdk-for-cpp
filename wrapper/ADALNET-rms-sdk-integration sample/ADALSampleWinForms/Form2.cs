using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net.Http;
using System.Web;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using Microsoft.IdentityModel.Clients.ActiveDirectory;

namespace ADALSampleWinForms {
  public partial class AadAuthFlowsForm : Form {
    public AadAuthFlowsForm() {
      InitializeComponent();
            // the chagne needed to make BlackForest flows work is only deployed to test slice.
            // So set an environment variable that will make ADAL pass this extra request parameter
            // to every request it performs.
            //Environment.SetEnvironmentVariable("ExtraQueryParameter", "dc=PROD-WST-TEST");
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

    private void checkBox1_CheckedChanged(object sender, EventArgs e) {
      authorityTextBox.Enabled = !checkBox1.Checked;
    }

    private void label1_Click(object sender, EventArgs e) {

    }

    private void textBox3_TextChanged(object sender, EventArgs e) {

    }

    private string emailAddressHint;

    private String getAuthority(String email) {
      String authority = null;
      if (checkBox1.Checked) {
        // Auto-detecting authority requires to collect email address of the user
        emailAddressHint = email;
        if (emailAddressHint == null) {
          EmailAddressDialog emailDialog = new EmailAddressDialog();
          DialogResult dialogResult = emailDialog.ShowDialog();
          if (dialogResult != DialogResult.OK)
            return null;
          emailAddressHint = emailDialog.EmailAddress;
        }
        authority = computeAuthority(emailAddressHint);
        if (authority == null) {
          MessageBox.Show("Could not auto-compute authority for email " + emailAddressHint);
          return null;
        }
        authorityTextBox.Text = authority;
      } else {
        authority = authorityTextBox.Text.Trim();
      }

      if (!Uri.IsWellFormedUriString(authority, UriKind.Absolute)) {
        MessageBox.Show("Authority should be a valid URI");
        return null;
      }
      return authority;
    }

    private String getResource() {
      String resource = resourceTextBox.Text.Trim();
      if (string.IsNullOrEmpty(resource)) {
        MessageBox.Show("Resource is a required field");
        return null;
      }
      return resource;
    }

    private String getClientID() {
      String clientId = clientIdTextBox.Text.Trim();
      if (string.IsNullOrEmpty(clientId)) {
        MessageBox.Show("Client ID is a required field");
        return null;
      }
      return clientId;
    }

    private Uri getRedirectUri() {
      string redirectUriStr = redirectUriTextBox.Text.Trim();
      if (string.IsNullOrEmpty(redirectUriStr) || !Uri.IsWellFormedUriString(redirectUriStr, UriKind.Absolute)) {
        MessageBox.Show("Redirect URi is a required field and should be a valid URI");
        return null;
      }
      return new Uri(redirectUriStr);
    }

    private void button1_Click(object sender, EventArgs e) {
      if (authTypeTabControl.SelectedIndex == 0) {
        DoNormalUserSignIn();
      } else if (authTypeTabControl.SelectedIndex == 1) {
        DoSilentUserSignIn();
      } else if (authTypeTabControl.SelectedIndex == 2) {
        DoSilentAppSignIn();
      } else if (authTypeTabControl.SelectedIndex == 3) {
                Form3 f = new Form3(consumeEmail.Text);
                f.Show();
            }
    }

    private void displayToken(String token) {
      fullJwtTextBox.Text = token;
      string[] tokenParts = token.Split('.');
      jwtHeaderTextBox.Text = tokenParts[0];
      jwtBodyTextBox.Text = tokenParts[1];
      jwtSignatureTextBox.Text = tokenParts[2];
      Console.WriteLine(token);
    }

    private void checkBox1_CheckedChanged_1(object sender, EventArgs e) {
      authorityTextBox.ReadOnly = checkBox1.Checked;
    }

    private string buildClaims()
    {
            string param = @"{'access_token':{'polids':{'essential':true,'values':['fc5fb17c-ecba-45df-aaa8-d799c32ddf9b']}}}";
            return "claims=" + param;//HttpUtility.UrlEncode(param);
    }
    private void DoNormalUserSignIn() {
      String authority = getAuthority(null);
      if (String.IsNullOrEmpty(authority)) return;

      String resource = getResource();
      if (string.IsNullOrEmpty(resource)) return;

      String clientId = getClientID();
      if (string.IsNullOrEmpty(clientId)) return;

      Uri redirectUri = getRedirectUri();
      if (redirectUri == null) return;

      UserIdentifier userId = (emailAddressHint != null ? new UserIdentifier(emailAddressHint, UserIdentifierType.OptionalDisplayableId) : UserIdentifier.AnyUser);

      AuthenticationContext context = new AuthenticationContext(authority);
      try {
        Task<AuthenticationResult> task = context.AcquireTokenAsync(
            resource,
            clientId,
            redirectUri,
            new PlatformParameters(PromptBehavior.Always),
            userId,
            buildClaims()
        );
        AuthenticationResult result = task.Result;
        Console.WriteLine(result.IdToken);
        displayToken(result.AccessToken);
      } catch (AggregateException ex) {
        AdalServiceException adalEx = ex.InnerException as AdalServiceException;
        if (adalEx != null && adalEx.ErrorCode == AdalError.AuthenticationCanceled) {
          return;
        }
        MessageBox.Show("Exception when acquring token: " + ex.InnerException);
      }
    }

    private void DoSilentUserSignIn() {
      String email = emailTextBox.Text.Trim();
      if (string.IsNullOrEmpty(email)) {
        MessageBox.Show("Email is required");
        return;
      }
      String password = passwordTextBox.Text;
      if (String.IsNullOrEmpty(password)) {
        MessageBox.Show("Password is required");
        return;
      }

      String authority = getAuthority(email);
      if (String.IsNullOrEmpty(authority)) return;

      String resource = getResource();
      if (string.IsNullOrEmpty(resource)) return;

      String clientId = getClientID();
      if (string.IsNullOrEmpty(clientId)) return;

      Uri redirectUri = getRedirectUri();
      if (redirectUri == null) return;

      AuthenticationContext context = new AuthenticationContext(authority);
      UserPasswordCredential pwdCreds = new UserPasswordCredential(email, password);
      try {
        Task<AuthenticationResult> task = context.AcquireTokenAsync(
            resource,
            clientId,
            pwdCreds);
        displayToken(task.Result.AccessToken);
      } catch (AggregateException ex) {
        MessageBox.Show("Exception when acquiring token: " + ex.InnerException);
      }
    }

    private void DoSilentAppSignIn() {
      String appId = appIdTextBox.Text.Trim();
      if (string.IsNullOrEmpty(appId)) {
        MessageBox.Show("App ID is required");
        return;
      }
      String appSecret = appSecretTextBox.Text;
      if (String.IsNullOrEmpty(appSecret)) {
        MessageBox.Show("App secret is required");
        return;
      }

      String authority = getAuthority(null);
      if (String.IsNullOrEmpty(authority)) return;

      String resource = getResource();
      if (string.IsNullOrEmpty(resource)) return;

      AuthenticationContext context = new AuthenticationContext(authority);
      ClientCredential cred = new ClientCredential(appId, appSecret);
      Task<AuthenticationResult> task = context.AcquireTokenAsync(resource, cred);
      try {
        AuthenticationResult result = task.Result;
        displayToken(result.AccessToken);
      } catch (AggregateException ex) {
        MessageBox.Show("Exception when acquiring token: " + ex.InnerException);
      }
    }

    private void authTypeTabControl_Selected(object sender, TabControlEventArgs e) {
      bool value = e.TabPage != tabPage3;
      clientIdTextBox.Enabled = value;
      redirectUriTextBox.Enabled = value;
      if (!value) checkBox1.Checked = false;
      checkBox1.Enabled = value;
    }
  }
}
