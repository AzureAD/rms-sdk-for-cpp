using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ADALSampleWinForms
{
    public partial class ResultForm : Form
    {
        public ResultForm(string contents)
        {
            InitializeComponent();
            this.textBox.Text = contents;
        }
    }
}
