/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "../ModernAPI/RMSExceptions.h"
#include "../Common/CommonTypes.h"
#include "CXMLUtils.h"

using namespace std;

namespace rmscore {
namespace restclients {
static bool DoesTagMatch(const char *szTag, const string& strName)
{
  return !_strnicmp(szTag, strName.c_str(), strName.size()) &&
         !isalnum(*(szTag + strName.size())); // check that the character after
                                              // the name is not a letter or
                                              // digit
}

bool CXMLUtils::FindTag(const char   *szXML,
                        const string& strTagName,
                        const char *& szStart,
                        const char *& szEnd)
{
  bool found     = false;
  bool inQuote   = false;
  bool inTag     = false;
  bool inComment = false;
  char quoteChar = '\0';

  static const char szCommentStart[] = "<!--";
  static const char szCommentEnd[]   = "-->";

  static const int nCommentStartLen = sizeof(szCommentStart) - 1; // -1 for
                                                                  // null
                                                                  // terminator
  static const int nCommentEndLen = sizeof(szCommentEnd) - 1;     // -1 for

  // null
  // terminator

  while (*szXML)
  {
    if (inComment)
    {
      if (!_strnicmp(szXML, szCommentEnd, nCommentEndLen))
      {
        // reached to the end of the comment, so set inComment to true,
        // skip the "-->" and continue with the loop
        inComment = false;
        szXML    += nCommentEndLen;
        continue;
      }
    }
    else if (!_strnicmp(szXML, szCommentStart, nCommentStartLen))
    {
      // encountered a start of a comment so set inComment to true,
      // skip the <!-- and continue with the loop until we see the end of the
      // comment
      inComment = true;
      szXML    += nCommentStartLen;
      continue;
    }
    else if (!inTag)
    {
      if (*szXML == '<') // start of tag
      {
        if (DoesTagMatch(szXML + 1, strTagName))
        {
          found   = true; // Found
          szStart = szXML;
        }

        inTag = true;
      }
    }
    else if (!inQuote)   // inTag
    {
      if (*szXML == '>') // end of tag
      {
        if (found)
        {
          szEnd = szXML + 1;
          return true;
        }

        inTag = false;
      }
      else if ((*szXML == '\"') || (*szXML == '\'')) // start of quote
      {
        inQuote   = true;
        quoteChar = *szXML;
      }
    }
    else                       // inTag and inQuote
    {
      if (*szXML == quoteChar) // end of quote
      {
        inQuote = false;
      }
    }

    ++szXML;
  }

  return false;
}

bool CXMLUtils::FindElement(const char *szXML, const string& strElementName,
                            const char *& szStartOuter, const char *& szEndOuter,
                            const char *& szStartInner, const char *& szEndInner)
{
  string strElementStarter = strElementName;
  string strElementEnder   = string("/") + strElementName;

  return FindTag(szXML, strElementStarter, szStartOuter, szStartInner) &&
         FindTag(szStartInner, strElementEnder, szEndInner, szEndOuter);
}

string CXMLUtils::ExtractElementFirst(const char   *szXML,
                                      const string& strElementName,
                                      bool          includeTag)
{
  const char *szStartOuter = NULL, *szEndOuter = NULL, *szStartInner = NULL,
             *szEndInner   = NULL;

  if (!FindElement(szXML, strElementName, szStartOuter, szEndOuter, szStartInner,
                   szEndInner))
  {
    throw exceptions::RMSInvalidArgumentException("Invalid XML argument");
  }

  return includeTag ? string(szStartOuter, szEndOuter - szStartOuter) : string(
    szStartInner,
    szEndInner - szStartInner);
}

void CXMLUtils::ExtractElementAll(const char     *szXML,
                                  const string  & strElementName,
                                  vector<string>& vOccurrences,
                                  bool            includeTag)
{
  while (1)
  {
    const char *szStartOuter = NULL, *szEndOuter = NULL, *szStartInner = NULL,
               *szEndInner   = NULL;

    if (!FindElement(szXML, strElementName, szStartOuter, szEndOuter,
                     szStartInner, szEndInner))
    {
      break;
    }

    if (includeTag)
    {
      vOccurrences.push_back(string(szStartOuter, szEndOuter - szStartOuter));
    }
    else
    {
      vOccurrences.push_back(string(szStartInner, szEndInner - szStartInner));
    }

    szXML = szEndOuter;
  }
}

void CXMLUtils::WrapWithRoot(const char *szXML,
                             size_t      xmlSize,
                             string    & strWrapped)
{
  const char szVersionTag[] = "?xml";
  const char szRootOpen[]   = "<Root>";
  const char szRootClose[]  = "</Root>";

  // Let's see if XML contains an XML Declaration (e.g. <?xml version="1.0"
  // encoding="UTF-8" ?>)
  const char *szStart = NULL, *szEnd = NULL;

  if (FindTag(szXML, szVersionTag, szStart, szEnd))
  {
    // skip the declaration before wrapping with root (otherwise the XML will
    // not be well formed)
    szXML = szEnd;
  }

  strWrapped.clear();
  strWrapped.reserve(xmlSize + sizeof(szRootOpen) + sizeof(szRootClose));

  strWrapped += szRootOpen;
  strWrapped += szXML;
  strWrapped += szRootClose;
}
} // namespace restclients
} // namespace rmscore
