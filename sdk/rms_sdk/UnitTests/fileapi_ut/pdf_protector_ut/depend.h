#ifndef RMSSDK_UNITTESTS_FILEAPI_UT_PDF_PROTECTOR_UT_DEPEND_H_
#define RMSSDK_UNITTESTS_FILEAPI_UT_PDF_PROTECTOR_UT_DEPEND_H_

#include <gtest/gtest.h>
#include <QMessageBox>
#include <QFileDialog>

#include <cstdio>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include<QCoreApplication>
#include <rmsauth/HttpHelper.h>
#include <rmsauth/Exceptions.h>
#include <ModernAPI/HttpHelper.h>
#include <ModernAPI/RMSExceptions.h>
#include "FileAPI/Protector.h"
#include "FileAPI/FileAPIStructures.h"

#ifdef Q_OS_WIN32
#define CLIENTID "28b05064-105c-418e-bad3-844e786f86d7"
#define REDIRECTURL "com.foxitsoftware.com.phantompdf-for-windows://authorize"
#define CLIENTEMAIL  "max@foxitsoftwareinc.onmicrosoft.com"//加密的时候只要已经连上环境这个设置就没有什么作用。。原本就是这么设置的。

#endif
#ifdef Q_OS_LINUX
#define CLIENTID "f3295a1c-bcfe-4430-80bf-1c0a5636e8e8"
#define REDIRECTURL "com.foxitsoftware.com.reader-for-linux://authorize"
#define CLIENTEMAIL   "max@foxitsoftwareinc.onmicrosoft.com"//加密的时候只要已经连上环境这个设置就没有什么作用。。原本就是这么设置的。

#endif

#define PDF_PROTECTOR_FILTER_NAME       "MicrosoftIRMServices"
#define PDF_PROTECTOR_WRAPPER_SUBTYPE   L"MicrosoftIRMServices"
#define PDF_PROTECTOR_WRAPPER_FILENAME  L"MicrosoftIRMServices Protected PDF.pdf"
#define PDF_PROTECTOR_WRAPPER_DES       L"This embedded file is encrypted using MicrosoftIRMServices filter"
#define PDF_PROTECTOR_WRAPPER_VERSION   2

std::string GetCurrentInputFile();

#endif // RMSSDK_UNITTESTS_FILEAPI_UT_PDF_PROTECTOR_UT_DEPEND_H_
