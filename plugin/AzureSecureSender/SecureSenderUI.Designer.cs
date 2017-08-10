namespace AzureSecureSender
{
    [System.ComponentModel.ToolboxItemAttribute(false)]
    partial class SecureSenderUI : Microsoft.Office.Tools.Outlook.FormRegionBase
    {
        public SecureSenderUI(Microsoft.Office.Interop.Outlook.FormRegion formRegion)
            : base(Globals.Factory, formRegion)
        {
            this.InitializeComponent();
        }

        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.title = new System.Windows.Forms.Label();
            this.vScrollBar1 = new System.Windows.Forms.VScrollBar();
            this.messageTableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
            this.topColorStrip = new System.Windows.Forms.Panel();
            this.emailIcon = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.emailIcon)).BeginInit();
            this.SuspendLayout();
            // 
            // title
            // 
            this.title.AutoSize = true;
            this.title.Font = new System.Drawing.Font("Segoe UI", 21.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.title.Location = new System.Drawing.Point(61, 25);
            this.title.Name = "title";
            this.title.Padding = new System.Windows.Forms.Padding(0, 5, 0, 10);
            this.title.Size = new System.Drawing.Size(43, 55);
            this.title.TabIndex = 0;
            this.title.Text = "fg";
            // 
            // vScrollBar1
            // 
            this.vScrollBar1.Location = new System.Drawing.Point(730, -53);
            this.vScrollBar1.Name = "vScrollBar1";
            this.vScrollBar1.Size = new System.Drawing.Size(10, 36);
            this.vScrollBar1.TabIndex = 3;
            // 
            // messageTableLayoutPanel
            // 
            this.messageTableLayoutPanel.AutoScroll = true;
            this.messageTableLayoutPanel.AutoSize = true;
            this.messageTableLayoutPanel.ColumnCount = 1;
            this.messageTableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.messageTableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.messageTableLayoutPanel.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.messageTableLayoutPanel.Location = new System.Drawing.Point(0, 189);
            this.messageTableLayoutPanel.Name = "messageTableLayoutPanel";
            this.messageTableLayoutPanel.RowCount = 2;
            this.messageTableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.messageTableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.messageTableLayoutPanel.Size = new System.Drawing.Size(748, 0);
            this.messageTableLayoutPanel.TabIndex = 4;
            // 
            // topColorStrip
            // 
            this.topColorStrip.BackColor = System.Drawing.Color.Red;
            this.topColorStrip.Location = new System.Drawing.Point(0, 0);
            this.topColorStrip.Name = "topColorStrip";
            this.topColorStrip.Size = new System.Drawing.Size(748, 19);
            this.topColorStrip.TabIndex = 6;
            // 
            // emailIcon
            // 
            this.emailIcon.BackgroundImage = global::AzureSecureSender.Properties.Resources.good;
            this.emailIcon.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.emailIcon.Location = new System.Drawing.Point(3, 25);
            this.emailIcon.Name = "emailIcon";
            this.emailIcon.Padding = new System.Windows.Forms.Padding(0, 5, 0, 10);
            this.emailIcon.Size = new System.Drawing.Size(52, 50);
            this.emailIcon.TabIndex = 5;
            this.emailIcon.TabStop = false;
            // 
            // SecureSenderUI
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoScroll = true;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.Controls.Add(this.topColorStrip);
            this.Controls.Add(this.emailIcon);
            this.Controls.Add(this.messageTableLayoutPanel);
            this.Controls.Add(this.vScrollBar1);
            this.Controls.Add(this.title);
            this.Name = "SecureSenderUI";
            this.Size = new System.Drawing.Size(748, 189);
            this.FormRegionShowing += new System.EventHandler(this.SecureSenderUI_FormRegionShowing);
            this.FormRegionClosed += new System.EventHandler(this.SecureSenderUI_FormRegionClosed);
            ((System.ComponentModel.ISupportInitialize)(this.emailIcon)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        #region Form Region Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private static void InitializeManifest(Microsoft.Office.Tools.Outlook.FormRegionManifest manifest, Microsoft.Office.Tools.Outlook.Factory factory)
        {
            manifest.FormRegionName = "SecureSenderUI";
            manifest.FormRegionType = Microsoft.Office.Tools.Outlook.FormRegionType.Adjoining;

        }

        #endregion

        private System.Windows.Forms.Label title;
        private System.Windows.Forms.VScrollBar vScrollBar1;
        private System.Windows.Forms.TableLayoutPanel messageTableLayoutPanel;
        private System.Windows.Forms.PictureBox emailIcon;
        private System.Windows.Forms.Panel topColorStrip;

        public partial class SecureSenderUIFactory : Microsoft.Office.Tools.Outlook.IFormRegionFactory
        {
            public event Microsoft.Office.Tools.Outlook.FormRegionInitializingEventHandler FormRegionInitializing;

            private Microsoft.Office.Tools.Outlook.FormRegionManifest _Manifest;

            [System.Diagnostics.DebuggerNonUserCodeAttribute()]
            public SecureSenderUIFactory()
            {
                this._Manifest = Globals.Factory.CreateFormRegionManifest();
                SecureSenderUI.InitializeManifest(this._Manifest, Globals.Factory);
                this.FormRegionInitializing += new Microsoft.Office.Tools.Outlook.FormRegionInitializingEventHandler(this.SecureSenderUIFactory_FormRegionInitializing);
            }

            [System.Diagnostics.DebuggerNonUserCodeAttribute()]
            public Microsoft.Office.Tools.Outlook.FormRegionManifest Manifest
            {
                get
                {
                    return this._Manifest;
                }
            }

            [System.Diagnostics.DebuggerNonUserCodeAttribute()]
            Microsoft.Office.Tools.Outlook.IFormRegion Microsoft.Office.Tools.Outlook.IFormRegionFactory.CreateFormRegion(Microsoft.Office.Interop.Outlook.FormRegion formRegion)
            {
                SecureSenderUI form = new SecureSenderUI(formRegion);
                form.Factory = this;
                return form;
            }

            [System.Diagnostics.DebuggerNonUserCodeAttribute()]
            byte[] Microsoft.Office.Tools.Outlook.IFormRegionFactory.GetFormRegionStorage(object outlookItem, Microsoft.Office.Interop.Outlook.OlFormRegionMode formRegionMode, Microsoft.Office.Interop.Outlook.OlFormRegionSize formRegionSize)
            {
                throw new System.NotSupportedException();
            }

            [System.Diagnostics.DebuggerNonUserCodeAttribute()]
            bool Microsoft.Office.Tools.Outlook.IFormRegionFactory.IsDisplayedForItem(object outlookItem, Microsoft.Office.Interop.Outlook.OlFormRegionMode formRegionMode, Microsoft.Office.Interop.Outlook.OlFormRegionSize formRegionSize)
            {
                if (this.FormRegionInitializing != null)
                {
                    Microsoft.Office.Tools.Outlook.FormRegionInitializingEventArgs cancelArgs = Globals.Factory.CreateFormRegionInitializingEventArgs(outlookItem, formRegionMode, formRegionSize, false);
                    this.FormRegionInitializing(this, cancelArgs);
                    return !cancelArgs.Cancel;
                }
                else
                {
                    return true;
                }
            }

            [System.Diagnostics.DebuggerNonUserCodeAttribute()]
            Microsoft.Office.Tools.Outlook.FormRegionKindConstants Microsoft.Office.Tools.Outlook.IFormRegionFactory.Kind
            {
                get
                {
                    return Microsoft.Office.Tools.Outlook.FormRegionKindConstants.WindowsForms;
                }
            }
        }
    }

    partial class WindowFormRegionCollection
    {
        internal SecureSenderUI SecureSenderUI
        {
            get
            {
                foreach (var item in this)
                {
                    if (item.GetType() == typeof(SecureSenderUI))
                        return (SecureSenderUI)item;
                }
                return null;
            }
        }
    }
}
