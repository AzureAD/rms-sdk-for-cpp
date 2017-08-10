using Microsoft.VisualStudio.TestTools.UnitTesting;
using SecureSenderLibrary.core;

namespace VSTOSecureSenderPlugInTests.src.core
{
    [TestClass]
    public class SHA256HasherTests
    {

        private IHasher sha256Hasher;

        [TestInitialize]
        public void Setup()
        {
            sha256Hasher = new Sha256Hasher();
        }

        [TestMethod]
        public void TestAlgorithm()
        {
            Assert.AreEqual("SHA256", sha256Hasher.Algorithm);
        }

        [TestMethod]
        public void TestHashNotNull()
        {
            string input = "This is a string";
            string expectedOutput = "4E9518575422C9087396887CE20477AB5F550A4AA3D161C5C22A996B0ABB8B35";
            string actualOutput = sha256Hasher.Hash(input);
            Assert.AreEqual(expectedOutput, actualOutput);
        }

        [TestMethod]
        public void TestHashNull()
        {
            string output = sha256Hasher.Hash(null);
            Assert.IsNull(output);
        }
    }
}
