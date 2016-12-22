#include "RedirectUrlSchemeHandler.h"
#include <QDebug>
#include <QWebEngineUrlRequestJob>

/**
 * @brief RedirectUrlSchemeHandler::requestStarted
 * This is used to handle the RedirectUrl which has a custom scheme. We capture the url
 * and fail the request.
 * @param request
 */
void RedirectUrlSchemeHandler::requestStarted(QWebEngineUrlRequestJob* request){
    request->fail(QWebEngineUrlRequestJob::RequestAborted);
    QUrl url = request->requestUrl();

    qDebug() << "Request URL in RedirectUrlSchemeHandler: " << url;

    emit urlCapture(url);
}
