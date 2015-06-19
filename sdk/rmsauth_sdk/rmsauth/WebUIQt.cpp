/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <IWebUI.h>
#include <Logger.h>
#include <Exceptions.h>
#include "../WebAuthDialog/Dialog.h"
#include <QDialog>
#include <QApplication>
#include <QTimer>
#include <QDebug>

namespace rmsauth {

static String jobAuthenticate(const String& requestUri, const String& callbackUri, bool useCookie)
{
    Dialog d(requestUri.data(), callbackUri.data(), useCookie);

    d.exec();
    if(d.result() == QDialog::Accepted)
    {
        return d.respondUrl().toStdString();
    }

    throw  RmsauthException("Canceled by user");
}

#ifdef QT_VER_LESS_THEN_54
static void messageInterceptor(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "%s\n", localMsg.constData());
        break;
    case QtWarningMsg:
        Logger::warning("WebUIQT", "Warning: %", localMsg.constData());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}

static String jobRunnerAuthenticate(const String& requestUri, const String& callbackUri, bool useCookie)
{
    Logger::info("WebUIQT", "jobRunnerAuthenticate");
    int argc = 1;
    char name[] = "jobRunnerAuthenticate";
    char ** argv = new char*[argc];
    argv[0] = name;

    // To suppress "WARNING: QApplication was not created in the main() thread" on the console
    // we redirect it to the Logger.
    Logger::info("WebUIQT", "Redirecting all the warnings to the Logger");
    auto oldMH = qInstallMessageHandler(messageInterceptor);

    QApplication a(argc, argv);

    auto result = jobAuthenticate(requestUri, callbackUri, useCookie);
    QTimer::singleShot(0, &a, SLOT(quit()));
    a.exec();

    Logger::info("WebUIQT", "Warnings redirection stoped");
    qInstallMessageHandler(oldMH);

    return std::move(result);
}
#endif

String WebUI::authenticate(const String& requestUri, const String& callbackUri)
{
    bool useCookie = promptBehavior_ != PromptBehavior::Always;

    if(qApp == nullptr)
    {
#ifdef QT_VER_LESS_THEN_54
        // To avoid issue #17 (https://github.com/MSOpenTech/rms-sdk-cpp/issues/17)
        // for QT versions less then 5.4
        // we need to create QApplication instance in a separate thread.
        auto fut  = std::async(std::launch::async, &jobRunnerAuthenticate, requestUri, callbackUri, useCookie);
        auto result = fut.get();
        return std::move(result);
#else
        int argc = 1;
        char name[] = "authenticate";
        char ** argv = new char*[argc];
        argv[0] = name;

        QApplication a(argc, argv);

        auto result = jobAuthenticate(requestUri, callbackUri, useCookie);
        QTimer::singleShot(0, &a, SLOT(quit()));
        a.exec();
        return std::move(result);
#endif
    }

    auto result = jobAuthenticate(requestUri, callbackUri, useCookie);
    return std::move(result);
}

} // namespace rmsauth {
