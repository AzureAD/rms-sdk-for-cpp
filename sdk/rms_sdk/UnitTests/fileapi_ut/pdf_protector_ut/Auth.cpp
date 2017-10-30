/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */
#include "Auth.h"

#include <QMessageBox>
#include <QFileDialog>

#include <cstdio>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>

#include <rmsauth/HttpHelper.h>
#include <rmsauth/Exceptions.h>
#include <ModernAPI/HttpHelper.h>
#include <ModernAPI/RMSExceptions.h>
#include "FileAPI/Protector.h"
#include "FileAPI/FileAPIStructures.h"

static void postToMainThread(const std::function<void()>& func,
                      QObject                     *mainApp) {
    QObject signalSource;
    QObject::connect(&signalSource, &QObject::destroyed, mainApp, [ = ](
                     QObject *) {
    func();
    });
}

AuthCallback::AuthCallback(const std::string& clientId, const std::string& redirectUrl)
  : clientId_(clientId)
  , redirectUrl_(redirectUrl)
{
  FileCachePtr = std::make_shared<rmsauth::FileCache>();
}

std::string AuthCallback::GetToken(std::shared_ptr<rmscore::modernapi::AuthenticationParameters>& ap)
{
  try
  {
    if (redirectUrl_.empty()) {
      throw rmscore::exceptions::RMSInvalidArgumentException(
              "redirect Url is empty");
    }

    if (clientId_.empty()) {
      throw rmscore::exceptions::RMSInvalidArgumentException("client Id is empty");
    }

    rmsauth::AuthenticationContext authContext(
      ap->Authority(), rmsauth::AuthorityValidationType::False, FileCachePtr);

    auto result = authContext.acquireToken(ap->Resource(),
                                           clientId_,
                                           redirectUrl_,
                                           PromptBehavior::Auto,
                                           ap->UserId());
    return result->accessToken();
  }
  catch (const rmsauth::Exception& /*ex*/)
  {
    // out logs
    throw;
  }
}

AuthCallbackUI::AuthCallbackUI(QObject      *mainApp,
                               const std::string& clientId,
                               const std::string& redirectUrl)
  : _mainApp(mainApp), _callback(clientId, redirectUrl)
{}

std::string AuthCallbackUI::GetToken(
    std::shared_ptr<rmscore::modernapi::AuthenticationParameters>& ap) {
    std::promise<std::string> prom;
    auto res = prom.get_future();

    // readdress call to main UI thread
    postToMainThread([&]() {
    prom.set_value(_callback.GetToken(ap));
    }, _mainApp);

    // wait for result and return it
    return res.get();
}

rmscore::modernapi::ConsentList ConsentCallback::Consents(rmscore::modernapi::ConsentList& consents) {
  QStringList columnsNames { "User", "URLs", "Type", "Domain" };

  static QMap<QString, QStringList> preventsList;

  bool added = false;

  for (size_t row = 0, rowMax = consents.size(); row < rowMax; ++row) {
    std::shared_ptr<rmscore::modernapi::IConsent> consent = consents[row];

    // check for preventing
    auto user = consent->User();

    if (user.empty()) continue;

    auto elems         = preventsList[QString::fromStdString(user)];
    bool isAnyNotFound = false;

    // insert urls
    for (auto& url : consent->Urls()) {
      auto urlQt = QString::fromStdString(url);
      bool found = false;

      for (auto& u : elems) {
        if (u.compare(urlQt, Qt::CaseInsensitive) == 0) {
          found = true;
          break;
        }
      }

      if (!found) {
        isAnyNotFound = true;
        break;
      }
    }

    if(!isAnyNotFound) continue;


    for (int col = 0, colMax = columnsNames.size(); col < colMax; ++col) {
      QString val;

      switch (col) {
      case 0:
        val = QString::fromStdString(consent->User());
        break;

      case 1:

        for (auto& url : consent->Urls()) {
          val += QString::fromStdString(url) + ";";
        }
        break;

      case 2:
        val = QString::fromStdString(
          consent->Type() == rmscore::modernapi::ConsentType::DocumentTrackingConsent ?
          "DocumentTracking" : "ServiceUrl");
        break;

      case 3:
        val = QString::fromStdString(consent->Domain());
        break;
      }

      added = true;
    }
  }
  bool accepted = true ;
  for (auto& consent : consents) {
    rmscore::modernapi::ConsentResult result(accepted, false);
    consent->Result(result);
  }
  return consents;
}
