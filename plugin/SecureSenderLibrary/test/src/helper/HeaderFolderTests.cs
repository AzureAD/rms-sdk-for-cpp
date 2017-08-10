using Microsoft.VisualStudio.TestTools.UnitTesting;
using SecureSenderLibrary.helper;
using SecureSenderLibrary.models;
using VSTOSecureSenderPlugInTests.src;

namespace VSTOSecureSenderPlugInTests.helper
{
    [TestClass]
    public class HeaderFolderTests
    {
        private IHeaderFolder _headerFolder;

        [TestInitialize]
        public void Setup()
        {
            _headerFolder = new HeaderFolder();
        }

        [TestMethod]
        public void TestFoldIntoEmail_Small()
        {
            var email = GetTestEmail();
            string smallMessage = "This is a small message (under 64 chars)";
            _headerFolder.FoldStringIntoHeader(email, smallMessage, "small_header");
            Assert.IsTrue(email.HasHeaderField("small_header-0"));
            Assert.IsFalse(email.HasHeaderField("small_header-1"));
            Assert.AreEqual(smallMessage, email.GetHeaderField("small_header-0"));
        }

        [TestMethod]
        public void TestFoldIntoEmail_Large()
        {
            IEmail email = GetTestEmail();
            string largeMessage = "";
            string expectedResult = "";
            for (int i = 0; i < 64; i++)
            {
                expectedResult += "a";
                largeMessage += "aaaa";
            }
            string header = "large_header";
            _headerFolder.FoldStringIntoHeader(email, largeMessage, header);
            for (int i = 0; i < 4; i++)
            {
                Assert.IsTrue(email.HasHeaderField(header + "-" + i));
                Assert.AreEqual(expectedResult, email.GetHeaderField(header + "-" + i));
            }
            Assert.IsFalse(email.HasHeaderField(header + "-4"));
        }

        [TestMethod]
        public void TestFoldIntoEmail_Uneven()
        {
            IEmail email = GetTestEmail();
            string largeMessage = "";
            string expectedResult = "";
            for (int i = 0; i < 64; i++)
            {
                expectedResult += "a";
                largeMessage += "aaaa";
            }
            largeMessage += "a";
            string header = "large_header";
            _headerFolder.FoldStringIntoHeader(email, largeMessage, header);
            for (int i = 0; i < 4; i++)
            {
                Assert.IsTrue(email.HasHeaderField(header + "-" + i));
                Assert.AreEqual(expectedResult, email.GetHeaderField(header + "-" + i));
            }
            Assert.IsTrue(email.HasHeaderField(header + "-4"));
            Assert.AreEqual("a", email.GetHeaderField(header + "-4"));
            Assert.IsFalse(email.HasHeaderField(header + "-5"));
        }

        [TestMethod]
        public void TestUnfoldNonexistentHeader()
        {
            IEmail email = GetTestEmail();
            string result = _headerFolder.UnfoldStringFromHeader(email, "header_isnt_present");
            Assert.IsNull(result);
        }

        [TestMethod]
        public void TestUnfoldHeader()
        {
            IEmail email = GetTestEmail();
            string unfoldedString = _headerFolder.UnfoldStringFromHeader(email, "test");
            Assert.AreEqual("This is a multipart string right here. ", unfoldedString);
        }

        private IEmail GetTestEmail()
        {
            TestEmail testEmail = new TestEmail();
            testEmail.SetBody("Body");
            testEmail.SetHeaderField("test-0", "This ");
            testEmail.SetHeaderField("test-1", "is ");
            testEmail.SetHeaderField("test-2", "a ");
            testEmail.SetHeaderField("test-3", "multipart ");
            testEmail.SetHeaderField("test-4", "string ");
            testEmail.SetHeaderField("test-5", "right ");
            testEmail.SetHeaderField("test-6", "here. ");
            return testEmail;
        }
    }
}
