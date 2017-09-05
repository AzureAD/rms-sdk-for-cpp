namespace ADALSampleWinForms {
  partial class AadAuthFlowsForm {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing) {
      if (disposing && (components != null)) {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent() {
            this.authTypeTabControl = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.label9 = new System.Windows.Forms.Label();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.label10 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.passwordTextBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.emailTextBox = new System.Windows.Forms.TextBox();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.label11 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.appSecretTextBox = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.appIdTextBox = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.jwtSignatureTextBox = new System.Windows.Forms.TextBox();
            this.jwtBodyTextBox = new System.Windows.Forms.TextBox();
            this.jwtHeaderTextBox = new System.Windows.Forms.TextBox();
            this.fullJwtTextBox = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.clientIdTextBox = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.redirectUriTextBox = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.resourceTextBox = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.authorityTextBox = new System.Windows.Forms.TextBox();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.label12 = new System.Windows.Forms.Label();
            this.consumeEmail = new System.Windows.Forms.TextBox();
            this.label13 = new System.Windows.Forms.Label();
            this.authTypeTabControl.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.tabPage4.SuspendLayout();
            this.SuspendLayout();
            // 
            // authTypeTabControl
            // 
            this.authTypeTabControl.Controls.Add(this.tabPage1);
            this.authTypeTabControl.Controls.Add(this.tabPage2);
            this.authTypeTabControl.Controls.Add(this.tabPage3);
            this.authTypeTabControl.Controls.Add(this.tabPage4);
            this.authTypeTabControl.Location = new System.Drawing.Point(20, 93);
            this.authTypeTabControl.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.authTypeTabControl.Name = "authTypeTabControl";
            this.authTypeTabControl.SelectedIndex = 0;
            this.authTypeTabControl.Size = new System.Drawing.Size(922, 77);
            this.authTypeTabControl.TabIndex = 0;
            this.authTypeTabControl.Selected += new System.Windows.Forms.TabControlEventHandler(this.authTypeTabControl_Selected);
            // 
            // tabPage1
            // 
            this.tabPage1.BackColor = System.Drawing.Color.Transparent;
            this.tabPage1.Controls.Add(this.label9);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.tabPage1.Size = new System.Drawing.Size(914, 51);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "User";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(7, 10);
            this.label9.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(138, 13);
            this.label9.TabIndex = 1;
            this.label9.Text = "Just click the Sign In button";
            // 
            // tabPage2
            // 
            this.tabPage2.BackColor = System.Drawing.Color.Transparent;
            this.tabPage2.Controls.Add(this.label10);
            this.tabPage2.Controls.Add(this.label2);
            this.tabPage2.Controls.Add(this.passwordTextBox);
            this.tabPage2.Controls.Add(this.label1);
            this.tabPage2.Controls.Add(this.emailTextBox);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.tabPage2.Size = new System.Drawing.Size(914, 51);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "User Silent";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(7, 11);
            this.label10.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(261, 13);
            this.label10.TabIndex = 11;
            this.label10.Text = "Enter email and password and click the Sign In button";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(443, 30);
            this.label2.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(53, 13);
            this.label2.TabIndex = 10;
            this.label2.Text = "Password";
            // 
            // passwordTextBox
            // 
            this.passwordTextBox.Location = new System.Drawing.Point(500, 28);
            this.passwordTextBox.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.passwordTextBox.Name = "passwordTextBox";
            this.passwordTextBox.PasswordChar = '*';
            this.passwordTextBox.Size = new System.Drawing.Size(380, 20);
            this.passwordTextBox.TabIndex = 9;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(7, 30);
            this.label1.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(32, 13);
            this.label1.TabIndex = 8;
            this.label1.Text = "Email";
            // 
            // emailTextBox
            // 
            this.emailTextBox.Location = new System.Drawing.Point(41, 28);
            this.emailTextBox.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.emailTextBox.Name = "emailTextBox";
            this.emailTextBox.Size = new System.Drawing.Size(382, 20);
            this.emailTextBox.TabIndex = 7;
            // 
            // tabPage3
            // 
            this.tabPage3.BackColor = System.Drawing.Color.Transparent;
            this.tabPage3.Controls.Add(this.label11);
            this.tabPage3.Controls.Add(this.label7);
            this.tabPage3.Controls.Add(this.appSecretTextBox);
            this.tabPage3.Controls.Add(this.label8);
            this.tabPage3.Controls.Add(this.appIdTextBox);
            this.tabPage3.Location = new System.Drawing.Point(4, 22);
            this.tabPage3.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Size = new System.Drawing.Size(914, 51);
            this.tabPage3.TabIndex = 2;
            this.tabPage3.Text = "Application";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(7, 10);
            this.label11.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(250, 13);
            this.label11.TabIndex = 15;
            this.label11.Text = "Enter app id and secret and click the Sign In button";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(442, 32);
            this.label7.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(60, 13);
            this.label7.TabIndex = 14;
            this.label7.Text = "App Secret";
            // 
            // appSecretTextBox
            // 
            this.appSecretTextBox.Location = new System.Drawing.Point(506, 29);
            this.appSecretTextBox.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.appSecretTextBox.Name = "appSecretTextBox";
            this.appSecretTextBox.PasswordChar = '*';
            this.appSecretTextBox.Size = new System.Drawing.Size(380, 20);
            this.appSecretTextBox.TabIndex = 13;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(7, 30);
            this.label8.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(40, 13);
            this.label8.TabIndex = 12;
            this.label8.Text = "App ID";
            // 
            // appIdTextBox
            // 
            this.appIdTextBox.Location = new System.Drawing.Point(47, 28);
            this.appIdTextBox.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.appIdTextBox.Name = "appIdTextBox";
            this.appIdTextBox.Size = new System.Drawing.Size(382, 20);
            this.appIdTextBox.TabIndex = 11;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(20, 178);
            this.button1.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(98, 28);
            this.button1.TabIndex = 2;
            this.button1.Text = "Sign In";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.jwtSignatureTextBox);
            this.groupBox1.Controls.Add(this.jwtBodyTextBox);
            this.groupBox1.Controls.Add(this.jwtHeaderTextBox);
            this.groupBox1.Controls.Add(this.fullJwtTextBox);
            this.groupBox1.Location = new System.Drawing.Point(20, 210);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.groupBox1.Size = new System.Drawing.Size(922, 515);
            this.groupBox1.TabIndex = 2;
            this.groupBox1.TabStop = false;
            // 
            // jwtSignatureTextBox
            // 
            this.jwtSignatureTextBox.Font = new System.Drawing.Font("Consolas", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.jwtSignatureTextBox.Location = new System.Drawing.Point(451, 438);
            this.jwtSignatureTextBox.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.jwtSignatureTextBox.Multiline = true;
            this.jwtSignatureTextBox.Name = "jwtSignatureTextBox";
            this.jwtSignatureTextBox.Size = new System.Drawing.Size(456, 71);
            this.jwtSignatureTextBox.TabIndex = 5;
            // 
            // jwtBodyTextBox
            // 
            this.jwtBodyTextBox.Font = new System.Drawing.Font("Consolas", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.jwtBodyTextBox.Location = new System.Drawing.Point(451, 108);
            this.jwtBodyTextBox.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.jwtBodyTextBox.Multiline = true;
            this.jwtBodyTextBox.Name = "jwtBodyTextBox";
            this.jwtBodyTextBox.Size = new System.Drawing.Size(456, 326);
            this.jwtBodyTextBox.TabIndex = 4;
            // 
            // jwtHeaderTextBox
            // 
            this.jwtHeaderTextBox.Font = new System.Drawing.Font("Consolas", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.jwtHeaderTextBox.Location = new System.Drawing.Point(451, 13);
            this.jwtHeaderTextBox.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.jwtHeaderTextBox.Multiline = true;
            this.jwtHeaderTextBox.Name = "jwtHeaderTextBox";
            this.jwtHeaderTextBox.Size = new System.Drawing.Size(456, 91);
            this.jwtHeaderTextBox.TabIndex = 3;
            // 
            // fullJwtTextBox
            // 
            this.fullJwtTextBox.Font = new System.Drawing.Font("Consolas", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.fullJwtTextBox.Location = new System.Drawing.Point(4, 13);
            this.fullJwtTextBox.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.fullJwtTextBox.Multiline = true;
            this.fullJwtTextBox.Name = "fullJwtTextBox";
            this.fullJwtTextBox.Size = new System.Drawing.Size(433, 496);
            this.fullJwtTextBox.TabIndex = 2;
            this.fullJwtTextBox.TextChanged += new System.EventHandler(this.textBox3_TextChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(490, 16);
            this.label5.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(47, 13);
            this.label5.TabIndex = 10;
            this.label5.Text = "Client ID";
            // 
            // clientIdTextBox
            // 
            this.clientIdTextBox.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.clientIdTextBox.Location = new System.Drawing.Point(539, 10);
            this.clientIdTextBox.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.clientIdTextBox.Name = "clientIdTextBox";
            this.clientIdTextBox.Size = new System.Drawing.Size(380, 22);
            this.clientIdTextBox.TabIndex = 9;
            this.clientIdTextBox.Text = "6b069eef-9dde-4a29-b402-8ce866edc897";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(469, 38);
            this.label6.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(69, 13);
            this.label6.TabIndex = 12;
            this.label6.Text = "Redirect URI";
            // 
            // redirectUriTextBox
            // 
            this.redirectUriTextBox.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.redirectUriTextBox.Location = new System.Drawing.Point(539, 35);
            this.redirectUriTextBox.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.redirectUriTextBox.Name = "redirectUriTextBox";
            this.redirectUriTextBox.Size = new System.Drawing.Size(380, 22);
            this.redirectUriTextBox.TabIndex = 11;
            this.redirectUriTextBox.Text = "com.microsoft.rms-sharing-for-win://authorize/";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(26, 13);
            this.label4.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(53, 13);
            this.label4.TabIndex = 17;
            this.label4.Text = "Resource";
            // 
            // resourceTextBox
            // 
            this.resourceTextBox.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.resourceTextBox.Location = new System.Drawing.Point(81, 10);
            this.resourceTextBox.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.resourceTextBox.Name = "resourceTextBox";
            this.resourceTextBox.Size = new System.Drawing.Size(382, 22);
            this.resourceTextBox.TabIndex = 16;
            this.resourceTextBox.Text = "https://api.aadrm.com";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(30, 38);
            this.label3.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(48, 13);
            this.label3.TabIndex = 15;
            this.label3.Text = "Authority";
            // 
            // authorityTextBox
            // 
            this.authorityTextBox.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.authorityTextBox.Location = new System.Drawing.Point(81, 35);
            this.authorityTextBox.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.authorityTextBox.Name = "authorityTextBox";
            this.authorityTextBox.Size = new System.Drawing.Size(382, 22);
            this.authorityTextBox.TabIndex = 14;
            this.authorityTextBox.Text = "https://login.microsoftonline.com/common";
            // 
            // checkBox1
            // 
            this.checkBox1.AutoSize = true;
            this.checkBox1.Location = new System.Drawing.Point(81, 60);
            this.checkBox1.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(124, 17);
            this.checkBox1.TabIndex = 13;
            this.checkBox1.Text = "Auto-detect authority";
            this.checkBox1.UseVisualStyleBackColor = true;
            this.checkBox1.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged_1);
            // 
            // tabPage4
            // 
            this.tabPage4.BackColor = System.Drawing.Color.WhiteSmoke;
            this.tabPage4.Controls.Add(this.label13);
            this.tabPage4.Controls.Add(this.consumeEmail);
            this.tabPage4.Controls.Add(this.label12);
            this.tabPage4.Location = new System.Drawing.Point(4, 22);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage4.Size = new System.Drawing.Size(914, 51);
            this.tabPage4.TabIndex = 3;
            this.tabPage4.Text = "Consume";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(6, 24);
            this.label12.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(32, 13);
            this.label12.TabIndex = 9;
            this.label12.Text = "Email";
            // 
            // consumeEmail
            // 
            this.consumeEmail.Location = new System.Drawing.Point(42, 21);
            this.consumeEmail.Margin = new System.Windows.Forms.Padding(2);
            this.consumeEmail.Name = "consumeEmail";
            this.consumeEmail.Size = new System.Drawing.Size(382, 20);
            this.consumeEmail.TabIndex = 10;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.BackColor = System.Drawing.Color.WhiteSmoke;
            this.label13.Location = new System.Drawing.Point(6, 6);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(164, 13);
            this.label13.TabIndex = 11;
            this.label13.Text = "Enter your email and click Sign In";
            // 
            // AadAuthFlowsForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(958, 743);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.resourceTextBox);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.authorityTextBox);
            this.Controls.Add(this.checkBox1);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.redirectUriTextBox);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.clientIdTextBox);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.authTypeTabControl);
            this.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.Name = "AadAuthFlowsForm";
            this.Text = "AAD auth playground";
            this.authTypeTabControl.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.tabPage3.ResumeLayout(false);
            this.tabPage3.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.tabPage4.ResumeLayout(false);
            this.tabPage4.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.TabControl authTypeTabControl;
    private System.Windows.Forms.TabPage tabPage1;
    private System.Windows.Forms.TabPage tabPage2;
    private System.Windows.Forms.TabPage tabPage3;
    private System.Windows.Forms.Button button1;
    private System.Windows.Forms.GroupBox groupBox1;
    private System.Windows.Forms.TextBox fullJwtTextBox;
    private System.Windows.Forms.TextBox jwtSignatureTextBox;
    private System.Windows.Forms.TextBox jwtBodyTextBox;
    private System.Windows.Forms.TextBox jwtHeaderTextBox;
    private System.Windows.Forms.Label label5;
    private System.Windows.Forms.TextBox clientIdTextBox;
    private System.Windows.Forms.Label label6;
    private System.Windows.Forms.TextBox redirectUriTextBox;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.TextBox passwordTextBox;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.TextBox emailTextBox;
    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.TextBox resourceTextBox;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.TextBox authorityTextBox;
    private System.Windows.Forms.CheckBox checkBox1;
    private System.Windows.Forms.Label label7;
    private System.Windows.Forms.TextBox appSecretTextBox;
    private System.Windows.Forms.Label label8;
    private System.Windows.Forms.TextBox appIdTextBox;
    private System.Windows.Forms.Label label9;
    private System.Windows.Forms.Label label10;
    private System.Windows.Forms.Label label11;
        private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.TextBox consumeEmail;
        private System.Windows.Forms.Label label12;
    }
}