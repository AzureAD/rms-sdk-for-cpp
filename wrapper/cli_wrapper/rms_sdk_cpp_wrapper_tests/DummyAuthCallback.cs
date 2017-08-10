namespace DotNetRmsWrapperTests
{
    class DummyAuthCallback : IAuthenticationCallbackExp
    {
        public string authority, resource, scope, userID;

        public string GetAccessToken(string authority, string resource, string scope, string userID)
        {
            this.authority = authority;
            this.resource = resource;
            this.scope = scope;
            this.userID = userID;
            return "AccessToken";
        }
    }
}
