#include <algorithm>
#include <QApplication>
#include <QDebug>
#include <fstream>
#include <istream>
#include <iostream>
#include "ProtectedFileStream.h"
#include "FastPFileConverter.h"
#include "RMSExceptions.h"

using namespace std;
using namespace rmscore;
using namespace rmscore::modernapi;
using std::ifstream;
using std::ios_base;
using std::fstream;

static std::unique_ptr<QApplication> _qApp;
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


FastPFileConverter::FastPFileConverter()
{
    threadPool = vector<thread>();

	//initialize dummy QApplication
	if (_qApp == nullptr)
	{
		int argc = 1;
		char name[] = "pfileConverter";
		char ** argv = new char*[argc];
		argv[0] = name;
		_qApp = std::make_unique<QApplication>(argc, argv);
	}
}

bool FastPFileConverter::ConvertFromPFile(
  const string                     & userId,
  const string					   & inFile,
  const string                     & outFile,
  IAuthenticationCallback          & auth,
  IConsentCallback                 & consent)
{
  auto inStream = make_shared<ifstream>(inFile, ios_base::in | ios_base::binary);
  auto outStream = make_shared<fstream>(outFile, ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary);

  if (!inStream->is_open()) {
      throw std::exception();
  }

  if (!outStream->is_open()) {
      throw std::exception();
  }

  auto inIStream = rmscrypto::api::CreateStreamFromStdStream(static_pointer_cast<istream>(inStream));

  auto fsResult = ProtectedFileStream::Acquire(
    inIStream,
    userId,
    auth,
    &consent,
    POL_None,
    static_cast<ResponseCacheFlags>(RESPONSE_CACHE_NOCACHE),
    nullptr);

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
                                  static_pointer_cast<iostream>(outStream), 
								  pfs,
                                  false));
    }

    for (thread& t: threadPool) {
      if (t.joinable()) {
        t.join();
      }
    }
  }
  return fsResult->m_status == Success;
}
