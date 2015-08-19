REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin
TARGET    = rmsauth

# QT 5.4 has useful features
QT_CUR_VER =  "v$${QT_MAJOR_VERSION}.$${QT_MINOR_VERSION}"
message(Qt current version: $$QT_CUR_VER)

lessThan(QT_CUR_VER, v5.4) {
    message(QT_VER_LESS_THEN_54)
    DEFINES += QT_VER_LESS_THEN_54
}

TEMPLATE = lib

DEFINES += RMSAUTH_LIBRARY

QT     += core widgets network
CONFIG += plugin c++11 debug_and_release warn_on

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -L$$DESTDIR -lrmsauthWebAuthDialogd -lrmscryptod
} else {
    LIBS +=  -L$$DESTDIR -lrmsauthWebAuthDialog -lrmscrypto
}

INCLUDEPATH = ./rmsauth

SOURCES += \
    AuthenticationContext.cpp \
    Url.cpp \
    DateTime.cpp \
    Guid.cpp \
    utils.cpp \
    Authenticator.cpp \
    AuthenticatorTemplate.cpp \
    AuthenticatorTemplateList.cpp \
    CallState.cpp \
    AcquireTokenHandlerBase.cpp \
    AcquireTokenInteractiveHandler.cpp \
    AcquireTokenNonInteractiveHandler.cpp \
    ClientKey.cpp \
    TokenCache.cpp \
    AuthenticationResult.cpp \
    RequestParameters.cpp \
    ClientAssertionCertificate.cpp \
    ClientCredential.cpp \
    ClientAssertion.cpp \
    IWebUI.cpp \
    UserIdentifier.cpp \
    RmsauthIdHelper.cpp \
    Logger.cpp \
    TokenCacheKey.cpp \
    TokenCacheItem.cpp \
    UserCredential.cpp \
    UserAssertion.cpp \
    UserRealmDiscoveryResponseQt.cpp\
    AcquireTokenForClientHandler.cpp \
    FileCacheEncrypted.cpp \
    IRMSAuthEnvironment.cpp \
    IRMSAuthEnvironmentImpl.cpp

HEADERS += \
    rmsauth/AuthenticationContext.h \
    rmsauth/types.h \
    rmsauth/Exceptions.h \
    rmsauth/Url.h \
    rmsauth/DateTime.h \
    rmsauth/Guid.h \
    rmsauth/utils.h \
    rmsauth/Authenticator.h \
    rmsauth/AuthenticatorTemplate.h \
    rmsauth/AuthenticatorTemplateList.h \
    rmsauth/AuthorityType.h \
    rmsauth/CallState.h \
    rmsauth/Constants.h \
    rmsauth/AcquireTokenHandlerBase.h \
    rmsauth/AcquireTokenInteractiveHandler.h \
    rmsauth/AcquireTokenNonInteractiveHandler.h \
    rmsauth/ClientKey.h \
    rmsauth/TokenCache.h \
    rmsauth/TokenSubjectType.h \
    rmsauth/AuthenticationResult.h \
    rmsauth/RequestParameters.h \
    rmsauth/OAuthConstants.h \
    rmsauth/OAuth2Response.h\
    rmsauth/ClientAssertionCertificate.h \
    rmsauth/ClientCredential.h \
    rmsauth/ClientAssertion.h \
    rmsauth/HttpHelper.h \
    rmsauth/Entities.h \
    rmsauth/AuthorizationResult.h \
    rmsauth/IWebUI.h \
    rmsauth/UserIdentifier.h \
    rmsauth/RmsauthIdHelper.h \
    rmsauth/Logger.h \
    rmsauth/TokenCacheKey.h \
    rmsauth/TokenCacheNotificationArgs.h \
    rmsauth/TokenCacheItem.h \
    rmsauth/UserInfo.h \
    rmsauth/FileCache.h \
    rmsauth/rmsauthExport.h \
    rmsauth/UserCredential.h \
    rmsauth/UserAssertion.h \
    rmsauth/UserRealmDiscoveryResponse.h \
    rmsauth/AcquireTokenForClientHandler.h \
    rmsauth/FileCacheEncrypted.h \
    rmsauth/IRMSAuthEnvironment.h \
    rmsauth/IRMSAuthEnvironmentImpl.h

# Framework specific (qt based)

HEADERS += \
    GuidQt.h \
    UrlQt.h \
    DateTimeQt.h \
    JsonUtilsQt.h \
    HttpHelperQt.h

SOURCES += \
    GuidQt.cpp \
    UrlQt.cpp \
    DateTimeQt.cpp \
    HttpHelperQt.cpp \
    OAuth2ResponseQt.cpp \
    JsonUtilsQt.cpp \
    RmsauthIdHelperQt.cpp \
    WebUIQt.cpp \
    AuthenticatorTemplateQt.cpp \
    LoggerImplQt.cpp \
    AuthenticationResultQt.cpp \
    UserInfoQt.cpp \
    FileCacheQt.cpp \
    utilsQt.cpp \
    TokenCacheQt.cpp\
    AcquireTokenNonInteractiveHandlerQt.cpp \
    RequestParametersQt.cpp

unix {
    contains(QMAKE_HOST.arch, x86_64) {
        target.path = /usr/lib64
        INSTALLS += target
    } else {
        target.path += /usr/lib
        INSTALLS += target
    }
}
