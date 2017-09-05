namespace ADALSampleWinForms {
  partial class Form1 {
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
      this.button1 = new System.Windows.Forms.Button();
      this.tokenTextBox = new System.Windows.Forms.TextBox();
      this.headerTextBox = new System.Windows.Forms.TextBox();
      this.bodyTextBox = new System.Windows.Forms.TextBox();
      this.signatureTextBox = new System.Windows.Forms.TextBox();
      this.button2 = new System.Windows.Forms.Button();
      this.textBox1 = new System.Windows.Forms.TextBox();
      this.button3 = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // button1
      // 
      this.button1.Location = new System.Drawing.Point(12, 12);
      this.button1.Name = "button1";
      this.button1.Size = new System.Drawing.Size(112, 37);
      this.button1.TabIndex = 0;
      this.button1.Text = "Sign In";
      this.button1.UseVisualStyleBackColor = true;
      this.button1.Click += new System.EventHandler(this.button1_Click);
      // 
      // tokenTextBox
      // 
      this.tokenTextBox.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.tokenTextBox.Location = new System.Drawing.Point(12, 69);
      this.tokenTextBox.Multiline = true;
      this.tokenTextBox.Name = "tokenTextBox";
      this.tokenTextBox.Size = new System.Drawing.Size(469, 564);
      this.tokenTextBox.TabIndex = 4;
      // 
      // headerTextBox
      // 
      this.headerTextBox.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.headerTextBox.Location = new System.Drawing.Point(504, 69);
      this.headerTextBox.Multiline = true;
      this.headerTextBox.Name = "headerTextBox";
      this.headerTextBox.Size = new System.Drawing.Size(558, 113);
      this.headerTextBox.TabIndex = 5;
      // 
      // bodyTextBox
      // 
      this.bodyTextBox.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.bodyTextBox.Location = new System.Drawing.Point(504, 188);
      this.bodyTextBox.Multiline = true;
      this.bodyTextBox.Name = "bodyTextBox";
      this.bodyTextBox.Size = new System.Drawing.Size(558, 285);
      this.bodyTextBox.TabIndex = 6;
      // 
      // signatureTextBox
      // 
      this.signatureTextBox.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.signatureTextBox.Location = new System.Drawing.Point(504, 479);
      this.signatureTextBox.Multiline = true;
      this.signatureTextBox.Name = "signatureTextBox";
      this.signatureTextBox.Size = new System.Drawing.Size(558, 154);
      this.signatureTextBox.TabIndex = 7;
      // 
      // button2
      // 
      this.button2.Location = new System.Drawing.Point(504, 672);
      this.button2.Name = "button2";
      this.button2.Size = new System.Drawing.Size(112, 37);
      this.button2.TabIndex = 8;
      this.button2.Text = "Sign In";
      this.button2.UseVisualStyleBackColor = true;
      this.button2.Click += new System.EventHandler(this.button2_Click);
      // 
      // textBox1
      // 
      this.textBox1.Location = new System.Drawing.Point(12, 679);
      this.textBox1.Name = "textBox1";
      this.textBox1.Size = new System.Drawing.Size(469, 22);
      this.textBox1.TabIndex = 9;
      // 
      // button3
      // 
      this.button3.Location = new System.Drawing.Point(183, 12);
      this.button3.Name = "button3";
      this.button3.Size = new System.Drawing.Size(112, 37);
      this.button3.TabIndex = 10;
      this.button3.Text = "Silent";
      this.button3.UseVisualStyleBackColor = true;
      this.button3.Click += new System.EventHandler(this.button3_Click);
      // 
      // Form1
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(1134, 733);
      this.Controls.Add(this.button3);
      this.Controls.Add(this.textBox1);
      this.Controls.Add(this.button2);
      this.Controls.Add(this.signatureTextBox);
      this.Controls.Add(this.bodyTextBox);
      this.Controls.Add(this.headerTextBox);
      this.Controls.Add(this.tokenTextBox);
      this.Controls.Add(this.button1);
      this.Name = "Form1";
      this.Text = "Form1";
      this.Load += new System.EventHandler(this.Form1_Load);
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Button button1;
    private System.Windows.Forms.TextBox tokenTextBox;
    private System.Windows.Forms.TextBox headerTextBox;
    private System.Windows.Forms.TextBox bodyTextBox;
    private System.Windows.Forms.TextBox signatureTextBox;
    private System.Windows.Forms.Button button2;
    private System.Windows.Forms.TextBox textBox1;
    private System.Windows.Forms.Button button3;
  }
}

