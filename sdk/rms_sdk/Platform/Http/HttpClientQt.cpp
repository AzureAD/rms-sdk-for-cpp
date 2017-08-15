/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifdef QTFRAMEWORK
#include "HttpClientQt.h"
#include "../Logger/Logger.h"
#include "../../ModernAPI/RMSExceptions.h"
#include "mscertificates.h"
#include "HttpClientQt.h"

using namespace std;
using namespace rmscore::platform::logger;
using namespace web::http;
using namespace utility::conversions;

namespace rmscore {
namespace platform {
namespace http {

shared_ptr<IHttpClient>IHttpClient::Create()
{
    return make_shared<HttpClientQt>();
}

HttpClientQt::HttpClientQt() {}
HttpClientQt::~HttpClientQt() {}

pplx::task<void> http::HttpClientQt::HttpPostAsync(std::string url, common::ByteArray &body,web::http::http_headers headers,common::ByteArray &res, http::StatusCode &code)
{
    Logger::Info("==> HttpClientQt::POST %s", url.data());
    http_request request;

    request.set_method(methods::POST);
    std::string reqBody(reinterpret_cast<char const*>(body.data()), static_cast<int>(body.size()));
    request.set_body(to_utf8string(reqBody),utf8string("text/plain; charset=utf-8"));
    web::uri* clientUri=new  web::uri(to_string_t(url));
    client::http_client client(*clientUri);

    Logger::Hidden("==> Request Headers:");
    http_headers::const_iterator i;
    for(i=headers.begin();i!=headers.end();i++)
    {
        request.headers().add(i->first,i->second);
        Logger::Hidden("%s : %s", i->first, i->second);
    }
    Logger::Hidden("==> Request Body: %s", reqBody.c_str());

        try
        {
            // Make HTTP request.
            return client.request(request).then([&code, &res](web::http::http_response response)
            {
                try
                {
                    code = http::StatusCode(response.status_code());
                    res = response.extract_vector().get();
                    Logger::Info("Response StatusCode: %i", code);
                    Logger::Hidden("--> Response Headers:");
                    web::http::http_headers::const_iterator i;
                    for(i=response.headers().begin();i!=response.headers().end();i++)
                    {
                        Logger::Hidden("%s : %s", i->first, i->second);
                    }
                    Logger::Hidden("--> Response Body:");
                    Logger::Hidden(string(res.begin(), res.end()));
                }
                catch (const http_exception& e)
                {
                    Logger::Error("Error: %s",e.what());
                }
            });
        }
        catch (const std::system_error& e)
        {
            Logger::Error("Error: %s",e.what());

            // Return an empty task.
            return pplx::task_from_result();
        }
}

pplx::task<void> http::HttpClientQt::HttpGetAsync(std::string uri, common::ByteArray &res,http_headers headers, http::StatusCode &code )
{
    Logger::Info("==> HttpClientQt::POST %s", uri.data());
    http_request request;

    request.set_method(methods::GET);
    web::uri* clientUri=new  web::uri(to_string_t(uri));
    /*client::http_client_config config;

    config.set_ssl_context_callback([](boost::asio::ssl::context &ctx) {
            ctx.load_verify_file("cert.PEM");
    });*/

    web::http::client::http_client client(*clientUri);

    Logger::Hidden("==> Request Headers:");
    http_headers::const_iterator i;
    for( i=headers.begin();i!=headers.end();i++)
    {
       request.headers().add(i->first,i->second);
       Logger::Hidden("%s : %s", i->first, i->second);
    }
    try
    {
        return client.request(request).then([&code, &res](http_response response)
        {
            try
            {
                code = http::StatusCode(response.status_code());
                res = response.extract_vector().get();
                Logger::Info("Response StatusCode: %i", code);
                Logger::Hidden("--> Response Headers:");
                for(web::http::http_headers::const_iterator i=response.headers().begin();i!=response.headers().end();i++)
                {
                    Logger::Hidden("%s : %s", i->first, i->second);
                }
                Logger::Hidden("--> Response Body:");
                Logger::Hidden(string(res.begin(), res.end()));
                }
                catch (const web::http::http_exception& e)
                {
                    Logger::Error("Error: %s",e.what());
                }
            });
        }
        catch (const std::system_error& e)
        {
            Logger::Error("Error: %s",e.what());

            // Return an empty task.
            return pplx::task_from_result();
        }
}

void HttpClientQt::AddAuthorizationHeader(const string& authToken)
{
    this->AddHeader("Authorization", authToken);
}

void HttpClientQt::AddAcceptMediaTypeHeader(const string& mediaType)
{
    this->AddHeader("Accept", mediaType);
}

void HttpClientQt::AddAcceptLanguageHeader(const string& language)
{
    this->AddHeader("Accept-Language", language);
}

void HttpClientQt::AddHeader(const string& headerName,const string& headerValue)
{
    http_headers::key_type i=to_string_t(headerName);
    requestheaders_.add(i, headerValue.c_str());
}

StatusCode HttpClientQt::Post(const string                     & url,
                              const common::ByteArray          & request,
                              const string                     & mediaType,
                              common::ByteArray                & response,
                              std::shared_ptr<std::atomic<bool> >cancelState)
{
    StatusCode code;
    common::ByteArray body= request;
    this->AddAcceptMediaTypeHeader(mediaType);
    http::HttpClientQt::HttpPostAsync(url,body,requestheaders_,response,code).wait();
    return code;
}

StatusCode HttpClientQt::Get(const string                     & url,
                             common::ByteArray                & response,
                             std::shared_ptr<std::atomic<bool> >cancelState)
{
    StatusCode code;
    http::HttpClientQt::HttpGetAsync(url,response,requestheaders_,code).wait();
    return code;
}

const string HttpClientQt::GetResponseHeader(const string& headerName)
{
    http_headers::key_type key= to_string_t(headerName);
    return replyheaders_.has(key) ? to_utf8string(replyheaders_[key]) : "";
}

void HttpClientQt::SetAllowUI(bool /* allow*/)
{
    throw exceptions::RMSNotFoundException("Not implemented");
}

}
}
} // namespace rmscore { namespace platform { namespace http {
#endif // ifdef QTFRAMEWORK
