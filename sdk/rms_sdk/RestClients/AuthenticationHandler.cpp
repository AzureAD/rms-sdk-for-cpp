/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <algorithm>
#include <vector>
#include <string>
#include <stdlib.h>
#include <cstdio>
#include "AuthenticationHandler.h"
#include "../ModernAPI/RMSExceptions.h"
#include "../Platform/Http/IHttpClient.h"
#include "../Platform/Http/IUri.h"
#include "../Common/CommonTypes.h"

using namespace rmscore::modernapi;
using namespace rmscore::platform::http;
using namespace std;

namespace rmscore {
namespace restclients {
string AuthenticationHandler::GetAccessTokenForUrl(
  const string                     & sUrl,
  IAuthenticationCallbackImpl      & callback,
  std::shared_ptr<std::atomic<bool> >cancelState)
{
  AuthenticationChallenge challenge;

  // get the challenge only if needed (e.g., it's not needed in Office case
  // for now)
  if (callback.NeedsChallenge())
  {
    challenge = GetChallengeForUrl(sUrl, cancelState);
  }

  return callback.GetAccessToken(static_cast<const AuthenticationChallenge&>(
                                   challenge));
}

AuthenticationChallenge AuthenticationHandler::GetChallengeForUrl(
  const string                     & sUrl,
  std::shared_ptr<std::atomic<bool> >cancelState)
{
  // do a dummy get to the url to get the auth challenge

  auto pHttpClient = IHttpClient::Create();

  common::ByteArray response;
  StatusCode nStatusCode = pHttpClient->Get(sUrl, response, cancelState);

  // this must be an authenticated endpoint and we must get a 401
  // (unauthorized).
  // Otherwise, there is something wrong with the server
  if (StatusCode::UNAUTHORIZED != nStatusCode) {
    throw exceptions::RMSNetworkException("Server error",
                                          exceptions::RMSNetworkException::ServerError);
  }

  try
  {
    // the challenge must be in WWW-Authenticate header
    auto header = pHttpClient->GetResponseHeader("WWW-Authenticate");
    return ParseChallengeHeader(header, sUrl);
  }
  catch (exceptions::RMSException)
  {
    // if couldn't find the header or couldn't parse it, that means the server
    // returned an invalid response.
    throw exceptions::RMSNetworkException("Server error",
                                          exceptions::RMSNetworkException::ServerError);
  }
}

// trims the specified characters
static string TrimString(const string& str, const char *sCharacters)
{
  auto iFirst = str.find_first_not_of(sCharacters);

  if (string::npos == iFirst)
  {
    return string();
  }

  auto iLast = str.find_last_not_of(sCharacters);

  if (string::npos == iLast)
  {
    return string();
  }

  return str.substr(iFirst, iLast - iFirst + 1);
}

// trims whitespace
static string TrimWhiteSpace(const string& str)
{
  const char sWhiteSpaceCharacters[] = " \t\r\n";

  return TrimString(str, sWhiteSpaceCharacters);
}

// splits the string by the specified delimiter
static vector<string>SplitString(const string& str, char wDelimiter)
{
  vector<string> result;

  size_t nPosition = 0;

  while (nPosition < str.size())
  {
    auto nDelimiterPosition = str.find(wDelimiter, nPosition);

    if (string::npos == nDelimiterPosition)
    {
      result.push_back(str.substr(nPosition));
      break;
    }
    else
    {
      result.push_back(str.substr(nPosition, nDelimiterPosition - nPosition));
      nPosition = nDelimiterPosition + 1;
    }
  }

  return result;
}

// parses a name=value pair
static pair<string, string>ParseNameValuePair(const string& str)
{
  auto split = SplitString(str, L'=');

  if (2 == split.size())
  {
    // trim whitespace and double quotes from both name and value
    return make_pair(
      TrimString(TrimWhiteSpace(split[0]), "\""),
      TrimString(TrimWhiteSpace(split[1]), "\""));
  }
  else
  {
    // not a valid name and value pair so return empty
    return make_pair(string(), string());
  }
}

// parses name=value pairs delimited by ','
static vector<pair<string, string> >ParseNameValuePairList(const string& str)
{
  auto split = SplitString(str, L',');

  vector<pair<string, string> > pairs;

  for_each(begin(split), end(split),
           [&pairs](const string& str)
      {
        auto pair = ParseNameValuePair(str);

        if (!pair.first.empty())
        {
          pairs.push_back(move(pair));
        }
      });

  return move(pairs);
}

// validates that the string starts with the prefix and trims the prefix
static string ValidateAndTrimStringPrefix(const string& str,
                                          const string& prefix)
{
  if (0 == _strnicmp(prefix.c_str(), str.c_str(), prefix.size()))
  {
    return str.substr(prefix.size());
  }
  else
  {
    return string();
  }
}

AuthenticationChallenge AuthenticationHandler::ParseChallengeHeader(
  const string& header,
  const string& url)
{
  // verify that this is a bearer challenge (i.e., starts with "Bearer ")
  auto trimmed = ValidateAndTrimStringPrefix(header, "Bearer ");

  if (trimmed.empty())
  {
    throw exceptions::RMSNetworkException("Challenge is not bearer",
                                          exceptions::RMSNetworkException::ServerError);
  }


  // parse name value pairs
  auto pairs = ParseNameValuePairList(trimmed);

  AuthenticationChallenge challenge;

  // find the values of authorization_uri (authorization), realm and scope and
  // fill out the challenge struct
  for_each(begin(pairs), end(pairs),
           [&challenge](const pair<string, string>& p)
      {
        if ((0 ==
             _strcmpi("authorization_uri",
                      p.first.c_str())) ||
            (0 == _strcmpi("authorization", p.first.c_str())))
        {
          challenge.authority = p.second;
        }
        else if (0 == _strcmpi("realm", p.first.c_str()))
        {
          challenge.resource = p.second;
        }
        else if (0 == _strcmpi("scope", p.first.c_str()))
        {
          challenge.scope = p.second;
        }
      });

  if (challenge.authority.empty()) {
    throw exceptions::RMSNetworkException(
            "Invalid challenge returned by the server.",
            exceptions::RMSNetworkException::ServerError);
  }

  // if the resource (realm) is not in the header, use the source url's hostname
  // with the port
  if (challenge.resource.empty())
  {
    auto uri = IUri::Create(url);

    // validate the scheme (should be either http or https)
    auto scheme = uri->GetScheme();

    if ((scheme.compare("http") != 0) && (scheme.compare("https") != 0))
    {
      throw exceptions::RMSNetworkException("Invalid scheme",
                                            exceptions::RMSNetworkException::ServerError);
    }

    challenge.resource = uri->GetHost();

    // if the port number is not in the hostname add it
    if (string::npos == challenge.resource.find(L':'))
    {
      char sPort[20];
 #ifdef __GNUC__
      snprintf(sPort, sizeof(sPort), "%d", uri->GetPort());
#else // ifdef __GNUC__
      _snprintf_s(sPort, sizeof(sPort), "%d", uri->GetPort());
#endif // ifdef __GNUC__
      challenge.resource += ":";
      challenge.resource += sPort;
    }
  }

  return challenge;
}
} // namespace restclients
} // namespace rmscore
