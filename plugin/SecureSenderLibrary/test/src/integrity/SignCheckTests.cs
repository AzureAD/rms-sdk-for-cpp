using System.Collections.Generic;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SecureSenderLibrary.core;
using SecureSenderLibrary.integrity;
using SecureSenderLibrary.models;

namespace VSTOSecureSenderPlugInTests.integrity
{
    [TestClass]
    public class SignCheckTests
    {
        private SignatureCreator<MockProperty> _signatureCreator;

        [TestInitialize]
        public void Setup()
        {
            _signatureCreator = new SignatureCreator<MockProperty>("joe@microsoft.com", new Sha256Hasher(), null);
        }

        [TestMethod]
        public void TestSignValidateValid()
        {
            var data = GenerateMockProperties();
            var signature = _signatureCreator.GetSignature(data);
            var dataIntegrityCheck = new DataIntegrityCheck<MockProperty>(signature, data);
            Assert.AreEqual(IntegrityStatus.Verified, dataIntegrityCheck.Status);
            foreach(var property in data)
            {
                Assert.IsTrue(dataIntegrityCheck.VerifiedProperties.Contains(property));
            }
            Assert.AreEqual(0, dataIntegrityCheck.UnverifiedProperties.Count);
        }

        private static IList<MockProperty> GenerateMockProperties()
        {
            var propertiesList = new List<MockProperty>();
            for (var i = 0; i < 10; i++)
            {
                propertiesList.Add(new MockProperty("Name" + i, "Value" + i));
            }
            return propertiesList;
        }
    }
}
