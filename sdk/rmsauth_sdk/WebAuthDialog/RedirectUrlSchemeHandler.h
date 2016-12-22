#ifndef REDIRECTURLSCHEMEHANDLER_H
#define REDIRECTURLSCHEMEHANDLER_H

#include <QWebEngineUrlSchemeHandler>
#include <QUrl>

class RedirectUrlSchemeHandler : public QWebEngineUrlSchemeHandler
{
    Q_OBJECT

public:
    explicit RedirectUrlSchemeHandler(QObject *parent = 0) : QWebEngineUrlSchemeHandler(parent) {}
    void requestStarted(QWebEngineUrlRequestJob *request);

signals:
    void urlCapture(QUrl);
};

#endif // REDIRECTURLSCHEMEHANDLER_H
