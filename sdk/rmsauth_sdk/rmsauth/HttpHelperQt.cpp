/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <HttpHelper.h>
#include <Entities.h>
#include <OAuthConstants.h>
#include <CallState.h>
#include <Exceptions.h>
#include <RmsauthIdHelper.h>
#include <CallState.h>
#include <Logger.h>
#include "JsonUtilsQt.h"
#include "HttpHelperQt.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QEventLoop>
#include <QUuid>
#include <QCoreApplication>
#include <QTimer>

namespace rmsauth {

QByteArray HttpHelperQt::jobGet(QNetworkRequest& request, CallStatePtr callState)
{
    Logger::info(Tag(), "jobGet");

    HttpHelperQt::addCorrelationIdHeadersToRequest(request, callState);
    HttpHelperQt::logRequestHeaders(request);

    QNetworkAccessManager nam;
    QNetworkReply* pReply = nam.get(request);
    QEventLoop loop;
    QObject::connect(pReply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    HttpHelperQt::logResponseHeaders(pReply);

    QNetworkReply::NetworkError networkError = pReply->error();
    if (networkError != QNetworkReply::NoError)
    {
        auto errMessage = pReply->errorString();
        qDebug() << QString("error: %1").arg(errMessage);
        ErrorResponsePtr errorResponse = HttpHelperQt::parseResponseError(pReply);

        if (QString::fromStdString(errorResponse->error).compare("invalid_instance", Qt::CaseInsensitive) == 0)
        {
            throw RmsauthServiceException(Constants::rmsauthError().AuthorityNotInValidList, errMessage.toStdString());
        }
        else
        {
            StringStream ss;
            ss <<  Constants::rmsauthErrorMessage().AuthorityValidationFailed << ". "
                << errorResponse->error << ": "
                << errorResponse->errorDescription << ". "
                << errMessage.toStdString();

            throw RmsauthServiceException(
                Constants::rmsauthError().AuthorityValidationFailed,
                ss.str());
        }
    }

    HttpHelperQt::verifyCorrelationIdHeaderInReponse(pReply, callState);

    auto body = pReply->readAll();
    HttpHelperQt::logResponseBody(body);
    return std::move(body);
}

QByteArray HttpHelperQt::jobGetRunner(QNetworkRequest& request, CallStatePtr callState)
{
    Logger::info(Tag(), "jobGetRunner");

    int argc = 1;
    char name[] = "jobGetRunner";
    char ** argv = new char*[argc];
    argv[0] = name;
    QCoreApplication a(argc, argv);

    auto body = jobGet(request, callState);

    QTimer::singleShot(0, &a, SLOT(quit()));
    a.exec();

    return std::move(body);
}

QByteArray HttpHelperQt::jobPost(QNetworkRequest& request, const RequestParameters& requestParameters, CallStatePtr callState)
{
    Logger::info(Tag(), "jobPost");

    if ((callState != nullptr) && !callState->correlationId().empty())
    {
        HttpHelperQt::addCorrelationIdHeadersToRequest(request, callState);
    }
    HttpHelperQt::logRequestHeaders(request);
    Logger::info(Tag(), "request url:  %", request.url().toString().toStdString());
    Logger::info(Tag(), "request body: %", requestParameters.toString());

    QNetworkAccessManager nam;
    QNetworkReply *pReply = nam.post(request, requestParameters.toString().data());
    QEventLoop loop;
    QObject::connect(pReply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    HttpHelperQt::logResponseHeaders(pReply);

    QNetworkReply::NetworkError error_type = pReply->error();

    if (error_type != QNetworkReply::NoError)
    {
        String errString = QString("error: %1").arg(pReply->errorString()).toStdString();
        Logger::error(Tag(), errString);
        throw RmsauthServiceException(errString);
    }

    HttpHelperQt::verifyCorrelationIdHeaderInReponse(pReply, callState);

    auto body = pReply->readAll();
    HttpHelperQt::logResponseBody(body);
    return std::move(body);
}

QByteArray HttpHelperQt::jobPostRunner(QNetworkRequest& request, const RequestParameters& requestParameters, CallStatePtr callState)
{
    Logger::info(Tag(), "jobPostRunner");

    int argc      = 1;
    char   name[] = "jobPostRunner";
    char **argv   = new char *[argc];

    argv[0] = name;
    QCoreApplication a(argc, argv);

    auto body = jobPost(request, requestParameters, callState);


    QTimer::singleShot(0, &a, SLOT(quit()));
    a.exec();

    return std::move(body);
}

TokenResponsePtr HttpHelper::sendPostRequestAndDeserializeJsonResponseAsync(const String& url, const RequestParameters& requestParameters, CallStatePtr callState)
{
    Logger::info(Tag(), "sendPostRequestAndDeserializeJsonResponseAsync");

    QNetworkRequest request = HttpHelperQt::createRequest();

    request.setUrl(QUrl(url.data()));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    HttpHelperQt::addHeadersToRequest(request, RmsauthIdHelper::getProductHeaders());

    if (QCoreApplication::instance() == nullptr)
    {
        auto fut = std::async(&HttpHelperQt::jobPostRunner, std::ref(request), requestParameters, callState);
        auto body = fut.get();
        return HttpHelperQt::deserializeTokenResponse(body);
    }

    auto body = HttpHelperQt::jobPost(request, requestParameters, callState);
    return HttpHelperQt::deserializeTokenResponse(body);
}

static bool addCACertificate(const std::vector<uint8_t>& certificate, QSsl::EncodingFormat format)
{
    Logger::info("local", "addCACertificate");

    QSslConfiguration SslConfiguration(QSslConfiguration::defaultConfiguration());

    QList<QSslCertificate> certificates = SslConfiguration.caCertificates();

    QList<QSslCertificate> cert = QSslCertificate::fromData(QByteArray(
    reinterpret_cast<const char*>(certificate.data()), static_cast<int>(certificate.size())), format);

    if (cert.length() == 0) return false;

    certificates.append(cert);

    SslConfiguration.setCaCertificates(certificates);
    QSslConfiguration::setDefaultConfiguration(SslConfiguration);

    return true;
}

bool HttpHelper::addCACertificateBase64(const std::vector<uint8_t>& certificate)
{
    Logger::info(Tag(), "addCACertificateBase64");
    return addCACertificate(certificate, QSsl::Pem);
}

bool HttpHelper::addCACertificateDer(const std::vector<uint8_t>& certificate)
{
    Logger::info(Tag(), "addCACertificateDer");
    return addCACertificate(certificate, QSsl::Der);
}

QNetworkRequest HttpHelperQt::createRequest()
{
    Logger::info(Tag(), "createRequest");

    QNetworkRequest request;

    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setRawHeader("User-Agent", Constants::rmsauthIdParameter().UserAgent.data());
    return std::move(request);
}

void HttpHelperQt::addCorrelationIdHeadersToRequest(QNetworkRequest& request, CallStatePtr callState)
{
    Logger::info(Tag(), "deserializeTokenResponse");

    request.setRawHeader(OAuthConstants::oAuthHeader().CorrelationId.data(), callState->correlationId().toString().data());
    request.setRawHeader(OAuthConstants::oAuthHeader().RequestCorrelationIdInResponse.data(), "true");
}

void HttpHelperQt::verifyCorrelationIdHeaderInReponse(QNetworkReply* pReply, CallStatePtr callState)
{
    Logger::info(Tag(), "verifyCorrelationIdHeaderInReponse");

    if ((callState == nullptr) || callState->correlationId().empty())
    {
        return;
    }

    QUuid correlationIdInRequest(QString::fromStdString(callState->correlationId().toString()));

    auto headers = pReply->rawHeaderPairs();

    for (auto header : headers)
    {
        if (QString(header.first).compare(OAuthConstants::oAuthHeader().CorrelationId.data(), Qt::CaseInsensitive) == 0)
        {
            QString correlationIdStr(header.second);

            QUuid correlationIdInResponse(correlationIdStr);

            if (correlationIdInResponse.isNull())
            {
                Logger::warning(Tag(), "Returned correlation id '%' is not in GUID format.", correlationIdStr.toStdString());
            }
            else if (correlationIdInResponse != correlationIdInRequest)
            {
                Logger::warning(Tag(), "Returned correlation id '%' does not match the sent correlation id '%'",
                    correlationIdInResponse.toString().toStdString(),
                    correlationIdInRequest.toString().toStdString());
            }

            break;
        }
    }
}

TokenResponsePtr HttpHelperQt::deserializeTokenResponse(const QByteArray& body)
{
    Logger::info(Tag(), "deserializeTokenResponse");
    auto pTokenResponse = std::make_shared<TokenResponse>();

    StringStream ss; ss << "jsonObject: " << String(body.begin(), body.end());
    Logger::hidden(Tag(), ss.str());

    QJsonParseError error;
    auto qdoc = QJsonDocument::fromJson(body, &error);

    if (error.error != QJsonParseError::NoError)
    {
        throw RmsauthException(String("deserializeTokenResponse: ") + error.errorString().toStdString());
    }

    QJsonObject qobj = qdoc.object();

    pTokenResponse->tokenType     = JsonUtilsQt::getStringOrDefault(qobj,       TokenResponse::jsonNames().tokenType);
    pTokenResponse->accessToken   = JsonUtilsQt::getStringOrDefault(qobj,       TokenResponse::jsonNames().accessToken);
    pTokenResponse->refreshToken  = JsonUtilsQt::getStringOrDefault(qobj,       TokenResponse::jsonNames().refreshToken);
    pTokenResponse->resource      = JsonUtilsQt::getStringOrDefault(qobj,       TokenResponse::jsonNames().resource);
    pTokenResponse->idToken       = JsonUtilsQt::getStringOrDefault(qobj,       TokenResponse::jsonNames().idToken);
    pTokenResponse->createdOn     = JsonUtilsQt::getStringAsIntOrDefault(qobj,  TokenResponse::jsonNames().createdOn);
    pTokenResponse->expiresOn     = JsonUtilsQt::getStringAsIntOrDefault(qobj,  TokenResponse::jsonNames().expiresOn);
    pTokenResponse->expiresIn     = JsonUtilsQt::getStringAsIntOrDefault(qobj,  TokenResponse::jsonNames().expiresIn);
    pTokenResponse->correlationId = JsonUtilsQt::getStringOrDefault(qobj,       TokenResponse::jsonNames().correlationId);

    pTokenResponse->error            = JsonUtilsQt::getStringOrDefault(qobj,    TokenResponse::jsonNames().error);
    pTokenResponse->errorDescription = JsonUtilsQt::getStringOrDefault(qobj,    TokenResponse::jsonNames().errorDescription);
    pTokenResponse->errorCodes       = JsonUtilsQt::getIntArrayOrEmpty(qobj,    TokenResponse::jsonNames().errorCodes);

    return pTokenResponse;
}

InstanceDiscoveryResponsePtr HttpHelperQt::deserializeInstanceDiscoveryResponse(const QByteArray& body)
{
    Logger::info(Tag(), "deserializeInstanceDiscoveryResponse");

    auto pInstanceDiscoveryResponse = std::make_shared<InstanceDiscoveryResponse>();

    StringStream ss; ss << "jsonObject: " << String(body.begin(), body.end());
    Logger::hidden(Tag(), ss.str());

    QJsonParseError parseError;
    auto qdoc = QJsonDocument::fromJson(body, &parseError);

    if (parseError.error != QJsonParseError::NoError)
    {
        throw RmsauthException(String("deserializeInstanceDiscoveryResponse QJsonDocument::fromJson: ") + parseError.errorString().toStdString());
    }

    QJsonObject qobj = qdoc.object();

    pInstanceDiscoveryResponse->tenantDiscoveryEndpoint = JsonUtilsQt::getStringOrDefault(qobj, InstanceDiscoveryResponse::jsonNames().tenantDiscoveryEndpoint);

    return pInstanceDiscoveryResponse;
}

ErrorResponsePtr HttpHelperQt::parseResponseError(QNetworkReply* pReply)
{
    Logger::info(Tag(), "parseResponseError");

    auto pErrorResponse = std::make_shared<ErrorResponse>();

    auto jsonObject = pReply->readAll();

    Logger::info(Tag(), "jsonObject: %", String(jsonObject.begin(), jsonObject.end()));

    if (jsonObject.isEmpty())
    {
        pErrorResponse->error = Constants::rmsauthError().ServiceReturnedError;
        pErrorResponse->errorDescription = Constants::rmsauthErrorMessage().ServiceReturnedError;
    }
    else
    {
        QJsonParseError parseError;
        auto qdoc = QJsonDocument::fromJson(jsonObject, &parseError);

        if (parseError.error != QJsonParseError::NoError)
        {
                }

        QJsonObject qobj = qdoc.object();

        pErrorResponse->error            = JsonUtilsQt::getStringOrDefault(qobj, ErrorResponse::jsonNames().error);
        pErrorResponse->errorDescription = JsonUtilsQt::getStringOrDefault(qobj, ErrorResponse::jsonNames().errorDescription);
        pErrorResponse->errorCodes       = JsonUtilsQt::getIntArrayOrEmpty(qobj, ErrorResponse::jsonNames().errorCodes);
    }

    return pErrorResponse;
}

void HttpHelperQt::addHeadersToRequest(QNetworkRequest& request, const Headers& headers)
{
    for (auto& header : headers)
    {
        request.setRawHeader(header.first.data(), header.second.data());
    }
}
void  HttpHelperQt::logRequestHeaders(const QNetworkRequest& req)
{
    Logger::info(Tag(), "logRequestHeaders");
    QList<QByteArray> headerList = req.rawHeaderList();
    if(headerList.length() > 0)
    {
        Logger::info(Tag(), "--> Headers:");
        for(auto& header : headerList)
        {
            const QByteArray& headerVal = req.rawHeader(header);
            StringStream ss; ss << String(header.data(), header.size()) << ": " << String(headerVal.data(), headerVal.size());
            Logger::info(Tag(), ss.str());
        }
    }
}

void HttpHelperQt::logResponseHeaders(QNetworkReply *pReply)
{
    Logger::hidden(Tag(), "logResponseHeaders");
    if (pReply->rawHeaderPairs().length() > 0)
    {
        Logger::hidden(Tag(), "--> Headers:");
        foreach (const QNetworkReply::RawHeaderPair& pair, pReply->rawHeaderPairs())
        {
             StringStream ss; ss << String(pair.first.data(), pair.first.size()) << ": " <<  String(pair.second.data(), pair.second.size());
             Logger::hidden(Tag(), ss.str());
        }
    }
}
void HttpHelperQt::logResponseBody(const QByteArray& body)
{
    Logger::hidden(Tag(), "logResponseBody");
    if (body.length() > 0)
    {
        Logger::hidden(Tag(), "==> Body:");
        Logger::hidden(Tag(), String(body.begin(), body.end()));
    }
}

} // namespace rmsauth {
