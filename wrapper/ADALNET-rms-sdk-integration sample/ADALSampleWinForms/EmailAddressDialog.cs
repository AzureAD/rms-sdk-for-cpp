using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ADALSampleWinForms {
  public partial class EmailAddressDialog : Form {
    public EmailAddressDialog() {
      InitializeComponent();
    }

    public string EmailAddress { get; private set; }

    public String Authority { get; private set; }

    private void button1_Click(object sender, EventArgs e) {
      String email = textBox1.Text.Trim();
      if (String.IsNullOrWhiteSpace(email)) {
        MessageBox.Show("Email address is required");
        return;
      }
      int index = email.IndexOf('@');
      if (index == -1) {
        MessageBox.Show("Email address is not valid");
        return;
      }
      this.EmailAddress = email;
      this.DialogResult = DialogResult.OK;
      this.Close();
    }

    private void button2_Click(object sender, EventArgs e) {
      this.DialogResult = DialogResult.Cancel;
      this.Close();
    }
  }
}
