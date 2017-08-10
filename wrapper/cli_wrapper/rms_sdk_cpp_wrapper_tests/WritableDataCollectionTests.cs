using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using rmscore.wrapper;


namespace DotNetRmsWrapperTests
{
    [TestClass]
    public class WritableDataCollectionTests
    {

        [TestMethod]
        public void TestAddGetData()
        {
            WritableDataCollectionExp writableDataCollectionExp = new WritableDataCollectionExp("t-joanto@microsoft.com", new DummyAuthCallback(), "ClientID");
            writableDataCollectionExp.AddDataValue("Key1", "Value1");
            writableDataCollectionExp.AddDataValue("Key2", "Value2");
            Assert.IsTrue(writableDataCollectionExp.ContainsKey("Key1"));
            Assert.IsTrue(writableDataCollectionExp.ContainsKey("Key2"));
            Assert.AreEqual("Value1", writableDataCollectionExp.GetDataValue("Key1"));
            Assert.AreEqual("Value2", writableDataCollectionExp.GetDataValue("Key2"));
        }

        [TestMethod]
        public void TestWriteData()
        {
            WritableDataCollectionExp writableDataCollectionExp = new WritableDataCollectionExp("t-joanto@microsoft.com", new DummyAuthCallback(), "ClientID");
            writableDataCollectionExp.AddDataValue("Key", "Value");
            string result = writableDataCollectionExp.SignAndSerializeData();
            DataCollectionExp dataCollectionExp = new DataCollectionExp(result);
            Assert.IsTrue(dataCollectionExp.IsVerified());
            Assert.IsTrue(dataCollectionExp.ContainsKey("Key"));
            Assert.AreEqual("Value", dataCollectionExp.GetDataValue("Key"));
        }
    }
}
