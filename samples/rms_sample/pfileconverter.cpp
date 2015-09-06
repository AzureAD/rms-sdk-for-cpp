/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <algorithm>
#include <QDebug>

#include <CryptoAPI.h>
#include <ModernAPI/RMSExceptions.h>
#include <UserRights.h>
#include <rights.h>
#include <PolicyDescriptor.h>
#include "pfileconverter.h"

using namespace rmscore::modernapi;
using namespace std;

static mutex   threadLocker;
static int64_t totalSize     = 0;
static int64_t readPosition  = 0;
static int64_t writePosition = 0;
static vector<thread> threadPool;

#define THREADS_NUM 12


static void WorkerThread(shared_ptr<iostream>           stdStream,
                         shared_ptr<ProtectedFileStream>pStream,
                         bool                           modeWrite) {
  vector<uint8_t> buffer(4096);
  int64_t bufferSize = static_cast<int64_t>(buffer.size());

  while (totalSize - readPosition > 0) {
    // lock
    threadLocker.lock();

    // check remain
    if (totalSize - readPosition <= 0) {
      threadLocker.unlock();
      return;
    }

    // get read/write offset
    int64_t offsetRead  = readPosition;
    int64_t offsetWrite = writePosition;
    int64_t toProcess   = min(bufferSize, totalSize - readPosition);
    readPosition  += toProcess;
    writePosition += toProcess;

    // no need to lock more
    threadLocker.unlock();

    if (modeWrite) {
      // stdStream is not thread safe!!!
      try {
        threadLocker.lock();
        qDebug() << "WRITE offsetRead = " << offsetRead << " offsetWrite = " <<
          offsetWrite << " toProcess = " << toProcess;

        stdStream->seekg(offsetRead);
        stdStream->read(reinterpret_cast<char *>(&buffer[0]), toProcess);
        threadLocker.unlock();
        auto written =
          pStream->WriteAsync(
            buffer.data(), toProcess, offsetWrite, launch::deferred).get();

        if (written != toProcess) {
          throw rmscore::exceptions::RMSStreamException("Error while writing data");
        }
      }
      catch (exception& e) {
        qDebug() << "Exception: " << e.what();
      }
    } else {
      auto read =
        pStream->ReadAsync(&buffer[0],
                           toProcess,
                           offsetRead,
                           launch::deferred).get();

      if (read == 0) {
        break;
      }

      try {
        // stdStream is not thread safe!!!
        threadLocker.lock();

        qDebug() << "READ offsetRead = " << offsetRead << " offsetWrite = " <<
          offsetWrite << " toProcess = " << toProcess;

        // seek to write
        stdStream->seekp(offsetWrite);
        stdStream->write(reinterpret_cast<const char *>(buffer.data()), read);
        threadLocker.unlock();
      }
      catch (exception& e) {
        qDebug() << "Exception: " << e.what();
      }
    }
  }
}

PFileConverter::PFileConverter()
{}

PFileConverter::~PFileConverter()
{}

void PFileConverter::ConvertToPFilePredefinedRights(
  const string                     & userId,
  shared_ptr<istream>                inStream,
  const string                     & fileExt,
  shared_ptr<iostream>               outStream,
  IAuthenticationCallback          & auth,
  IConsentCallback& /*consent*/,
  const vector<UserRights>         & userRights,
  std::shared_ptr<std::atomic<bool> >cancelState)
{
  auto endValidation = chrono::system_clock::now() + chrono::hours(48);


  PolicyDescriptor desc(userRights);

  desc.Referrer(make_shared<string>("https://client.test.app"));
  desc.ContentValidUntil(endValidation);
  desc.AllowOfflineAccess(false);
  desc.Name("Test Name");
  desc.Description("Test Description");

  auto policy = UserPolicy::Create(desc, userId, auth,
                                   USER_AllowAuditedExtraction, cancelState);
  ConvertToPFileUsingPolicy(policy, inStream, fileExt, outStream);
}

future<bool>PFileConverter::ConvertToPFileTemplatesAsync(
  const string                     & userId,
  shared_ptr<istream>                inStream,
  const string                     & fileExt,
  shared_ptr<iostream>               outStream,
  IAuthenticationCallback          & auth,
  IConsentCallback& /*consent*/,
  ITemplatesCallback               & templ,
  launch                             launchType,
  std::shared_ptr<std::atomic<bool> >cancelState)
{
  auto templatesFuture = TemplateDescriptor::GetTemplateListAsync(
    userId, auth, launch::deferred, cancelState);

  return async(launchType, [templatesFuture, &templ, &auth, cancelState]
                 (const string _userId,
                 shared_ptr<istream>_inStream,
                 const string _fileExt,
                 shared_ptr<iostream>_outStream) -> bool
  {
    auto templates = templatesFuture.get();

    rmscore::modernapi::AppDataHashMap signedData;

    size_t pos = templ.SelectTemplate(templates);

    if (pos < templates->size()) {
      auto policy = UserPolicy::CreateFromTemplateDescriptor(
        (*templates)[pos],
        _userId,
        auth,
        USER_AllowAuditedExtraction,
        signedData,
        cancelState);

      ConvertToPFileUsingPolicy(policy, _inStream, _fileExt, _outStream);
    }
    return true;
  }, userId, inStream, fileExt, outStream);
}

void PFileConverter::ConvertToPFileUsingPolicy(shared_ptr<UserPolicy>policy,
                                               shared_ptr<istream>   inStream,
                                               const string        & fileExt,
                                               shared_ptr<iostream>  outStream)
{
  if (policy.get() != nullptr) {
    auto outIStream = rmscrypto::api::CreateStreamFromStdStream(outStream);
    auto pStream    = ProtectedFileStream::Create(policy, outIStream, fileExt);

    // preparing
    readPosition  = 0;
    writePosition = pStream->Size();

    inStream->seekg(0, ios::end);
    totalSize = inStream->tellg();
    qDebug() << "Total size = " << totalSize;

    // start threads
    for (size_t i = 0; i < THREADS_NUM; ++i) {
      threadPool.push_back(thread(WorkerThread,
                                  static_pointer_cast<iostream>(inStream),
                                  pStream,
                                  true));
    }

    for (thread& t: threadPool) {
      if (t.joinable()) {
        t.join();
      }
    }

    pStream->Flush();
  }
}

shared_ptr<GetProtectedFileStreamResult>PFileConverter::ConvertFromPFile(
  const string                     & userId,
  shared_ptr<istream>                inStream,
  shared_ptr<iostream>               outStream,
  IAuthenticationCallback          & auth,
  IConsentCallback                 & consent,
  std::shared_ptr<std::atomic<bool> >cancelState)
{
  auto inIStream = rmscrypto::api::CreateStreamFromStdStream(inStream);

  auto fsResult = ProtectedFileStream::Acquire(
    inIStream,
    userId,
    auth,
    &consent,
    POL_None,
    static_cast<ResponseCacheFlags>(RESPONSE_CACHE_INMEMORY
                                    | RESPONSE_CACHE_ONDISK),
    cancelState);

  if ((fsResult.get() != nullptr) && (fsResult->m_status == Success) &&
      (fsResult->m_stream != nullptr)) {
    auto pfs = fsResult->m_stream;

    // preparing
    readPosition  = 0;
    writePosition = 0;
    totalSize     = pfs->Size();

    // start threads
    for (size_t i = 0; i < THREADS_NUM; ++i) {
      threadPool.push_back(thread(WorkerThread,
                                  static_pointer_cast<iostream>(outStream), pfs,
                                  false));
    }

    for (thread& t: threadPool) {
      if (t.joinable()) {
        t.join();
      }
    }
  }
  return fsResult;
}
