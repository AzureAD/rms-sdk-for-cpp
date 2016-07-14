#include <fstream>
#include <algorithm>
#include "../Platform/Filesystem/IFileSystem.h"
#include "../Platform/Logger/Logger.h"
#include "ConsentDBHelper.h"

using namespace std;
using namespace rmscore::platform::logger;

namespace rmscore {
namespace consent {
// Static method to get the instance of DBHelper
ConsentDBHelper& ConsentDBHelper::GetInstance()
{
  static ConsentDBHelper instance;

  return instance;
}

// Initialize the DBHelper
// Creates the storage files if not present
// Reads the file contents and keep it in in-memory cache
bool ConsentDBHelper::Initialize(const string& path)
{
  if (m_init == true) return true;
  m_init = true;

  // create cache folder if non-existant
  platform::filesystem::IFileSystem::CreateDirectory(path);

  m_serviceUrlFile.open(path + m_serviceUrlDBFileName,
                        fstream::in | fstream::out);

  m_docTrackingFile.open(path + m_documentTrackingDBFileName,
                         fstream::in | fstream::out);

  if (m_serviceUrlFile.is_open())
      ReadFileContent(m_serviceUrlFile, m_serviceUrlCache);
  if(m_docTrackingFile.is_open())
      ReadFileContent(m_docTrackingFile, m_docTrackingCache);

  m_serviceUrlFile.close();
  m_serviceUrlFile.open(path + m_serviceUrlDBFileName,
                        fstream::out |
                        ios_base::app);
  m_docTrackingFile.close();
  m_docTrackingFile.open(path + m_documentTrackingDBFileName,
                        fstream::out |
                        ios_base::app);
  return true;
}

// Helper to read the storage file
// Read the full content - split it into line by line
// Parse each line as USerDomain. Add them to cache.
bool ConsentDBHelper::ReadFileContent(fstream           & stream,
                                      vector<UserDomain>& content)
{
    std::string line;

  try
  {
    while (std::getline(stream, line))
    {
      if(line.length() <= 0)
        break;

      auto pos = line.find_first_of("|");

      if (pos != string::npos)
      {
        auto user   = line.substr(0, pos);
        auto domain = line.substr(pos + 1);
        content.push_back(UserDomain { user, domain });
      }
    }
    stream.seekg(0,ios_base::end);
    return true;
  }
  catch (...)
  {}
  return false;
}

// Verifies whether given domain is present for given user in service url db
bool ConsentDBHelper::IsApprovedServiceDomainPresent(const string& userId,
                                                     const string& domain)
{
  return IsConsentPresentCommon(userId, domain, m_serviceUrlCache);
}

// Verifies whether given domain is present for given user in doc tracking db
bool ConsentDBHelper::IsDocumentTrackingConsentnPresent(const string& userId,
                                                        const string& domain)
{
  return IsConsentPresentCommon(userId, domain, m_docTrackingCache);
}

// Adds the given entry to service url db
void ConsentDBHelper::AddApprovedServiceDomain(const string& userId,
                                               const string& domain)
{
  m_serviceUrlCache.push_back(AddConsentCommon(userId, domain, m_serviceUrlFile));
}

// Adds the given entry to doc tracking db
void ConsentDBHelper::AddDocumentTrackingConsent(const string& userId,
                                                 const string& domain)
{
  m_docTrackingCache.push_back(AddConsentCommon(userId,
                                                domain,
                                                m_docTrackingFile));
}

// Helper to add an entry to any of dbs
UserDomain ConsentDBHelper::AddConsentCommon(const string& userId,
                                             const string& domain,
                                             fstream     & stream)
{
  string user = userId.size() == 0 ? m_genericUserName : userId;
  string line = user + string("|") + domain;

  AddLine(line, stream);
  return UserDomain { user, domain };
}

// Helper to verify whether an entry is present in db
bool ConsentDBHelper::IsConsentPresentCommon(const string           & userId,
                                             const string           & domain,
                                             std::vector<UserDomain>& cache)
{
  vector<string> possibleDomains = GetPossibleDomainNames(domain);
  bool bDomainFound              = false;

  string user = userId.size() == 0 ? m_genericUserName : userId;
  find_if(possibleDomains.begin(), possibleDomains.end(),
          [&](string domain) -> bool
      {
        if (GetApprovedDomainsForConsent(user, domain, cache).size() > 0)
        {
          bDomainFound = true;
          return true;
        }

        return false;
      });

  return bDomainFound;
}

// Helper to check whether a user|domain combination is present in the cache
// Returns all matches
vector<UserDomain>ConsentDBHelper::GetApprovedDomainsForConsent(
  const string      & userId,
  const string      & domain,
  vector<UserDomain>& cache)
{
  vector<UserDomain> list;
  for_each(cache.begin(), cache.end(), [&](UserDomain userDomain)
      {
        if ((_stricmp(userDomain.userId.c_str(), userId.c_str()) == 0) &&
            (_stricmp(userDomain.domain.c_str(), domain.c_str()) == 0))
        {
          list.push_back(userDomain);
        }
      });

  return list;
}

// Gets the possible domains from a single domain.
// e.g.main.corp.contoso.com
//  returns main.corp.contoso.com, corp.contoso.com, contoso.com
vector<string>ConsentDBHelper::GetPossibleDomainNames(const string& domain)
{
  vector<string> splitList;
  vector<string> possibleDomains;
  char *fullString = new char[domain.size() + 1];
#ifdef __GNUC__
  strcpy(fullString, domain.c_str());
  char *token = strtok(fullString, ".");
#else // ifdef __GNUC__
  char *next_token = NULL;
  strcpy_s(fullString, domain.size() + 1, domain.c_str());
  char *token = strtok_s(fullString, ".", &next_token);
#endif // ifdef __GNUC__

  while (token != NULL)
  {
    splitList.push_back(string(token));

#ifdef __GNUC__
    token = strtok(NULL, ".");
#else // ifdef __GNUC__
    token = strtok_s(NULL, ".", &next_token);
#endif // ifdef __GNUC__
  }

  if (fullString != NULL)
  {
    delete[] fullString;
    fullString = NULL;
  }

  size_t numberElements = domain.size() - 1;
  size_t membersLength  = splitList.size() - 1;

  for (size_t i = 0, lengthFromStart = 0; i < membersLength; i++)
  {
    size_t size = numberElements - lengthFromStart + 1;
    possibleDomains.push_back(domain.substr(lengthFromStart, size));
    lengthFromStart += (splitList.at(i).size() + 1);
  }

  return possibleDomains;
}

// Adds a line to the file
void ConsentDBHelper::AddLine(const string& line, std::fstream& stream)
{
    stream << line << std::endl;
    stream.flush();
}
} // namespace consent
} // namespace rmscore
