/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_PROTECTIONEXCEPTION_H_
#define _RMS_LIB_PROTECTIONEXCEPTION_H_

namespace rmslib {
namespace common {
#include <string>

class ResultException
{
public:

    ResultException(int hr, PCWSTR wszFile, PCWSTR wszFunction, DWORD dwLine);
    ResultException(int hr, PCWSTR wszFile, PCWSTR wszFunction, DWORD dwLine, PCWSTR wszMessageFormat, ...);

    HRESULT GetHResult() const
    {
        return m_hr;
    }

    const std::string& GetMessage() const
    {
        return m_message;
    }

    const std::string& GetFile() const
    {
        return m_file;
    }

    const std::string& GetFunction() const
    {
        return m_function;
    }

    DWORD GetLine() const
    {
        return m_dwLine;
    }

private:

    void Initialize(HRESULT hr, PCWSTR wszFile, PCWSTR wszFunction, DWORD dwLine, PCWSTR wszMessageFormat, va_list args);

private:

    HRESULT m_hr;
    std::string m_message;
    std::string m_file;
    std::string m_function;
    DWORD m_dwLine;
};
} // namespace common
} // namespace rmslib

#define THROW_HR(hr, ...) \
{ \
    prot::ResultException exc((hr), __FILEW__, __FUNCTIONW__, __LINE__, __VA_ARGS__); \
    prot::Logger::LogException(exc); \
    throw exc; \
}

#define FAIL_HR_X(hr, ...) \
{ \
    THROW_HR((hr), __VA_ARGS__); \
}

#define CHECK_HR_X(hr, ...) \
{ \
    if (FAILED((hr))) { FAIL_HR_X((hr), __VA_ARGS__); } \
}

#define CHECK_BOOL_X_HR(b, hr, ...) \
{ \
    if (FALSE == (b)) { FAIL_HR_X((hr), __VA_ARGS__); } \
}

#define CHECK_BOOL_X(b, ...) \
{ \
    CHECK_BOOL_X_HR((b), E_UNEXPECTED, __VA_ARGS__); \
}

#define CHECK_BOOL_X_ARG(b, ...) \
{ \
     CHECK_BOOL_X_HR((b), E_INVALIDARG, __VA_ARGS__); \
}

#define CHECK_BOOL_X_GLE(b, ...) \
{ \
     CHECK_BOOL_X_HR((b), HRESULT_FROM_WIN32(GetLastError()), __VA_ARGS__); \
}

#define CHECK_PTR_X_HR(p, hr, ...) \
{ \
    if (nullptr == (p)) { FAIL_HR_X((hr), __VA_ARGS__); } \
}

#define CHECK_PTR_X(p, ...) \
{ \
    CHECK_PTR_X_HR((p), E_UNEXPECTED, __VA_ARGS__); \
}

#define CHECK_PTR_X_ARG(p, ...) \
{ \
    CHECK_PTR_X_HR((p), E_INVALIDARG, __VA_ARGS__); \
}

#define CHECK_PTR_X_GLE(p, ...) \
{ \
    CHECK_PTR_X_HR((p), HRESULT_FROM_WIN32(GetLastError()), __VA_ARGS__); \
}
#endif //_RMS_LIB_PROTECTIONEXCEPTION_H_
