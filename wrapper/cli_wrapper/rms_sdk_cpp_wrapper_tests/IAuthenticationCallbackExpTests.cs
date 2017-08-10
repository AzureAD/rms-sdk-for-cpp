using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace DotNetRmsWrapperTests
{
    [TestClass]
    public class IAuthenticationCallbackExpTests
    {
        [TestMethod]
        public void TestAuthenticationCallback()
        {
            DummyAuthCallback dummyAuthCallback = new DummyAuthCallback();
            AuthenticationCallbackTester authenticationCallbackTester = new AuthenticationCallbackTester();
            string result = authenticationCallbackTester.TestRoute("resource", "authority", "redirect", "user_id", dummyAuthCallback);
            Assert.AreEqual(result, "AccessToken");
            Assert.AreEqual(dummyAuthCallback.authority, "authority");
            Assert.AreEqual(dummyAuthCallback.resource, "resource");
            Assert.AreEqual(dummyAuthCallback.scope, "redirect");
            Assert.AreEqual(dummyAuthCallback.userID, "user_id");
        }
    }
}
