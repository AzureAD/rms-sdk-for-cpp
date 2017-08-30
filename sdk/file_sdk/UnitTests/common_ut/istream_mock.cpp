#include "istream_mock.h"

IStreamMock::IStreamMock()
{
}

std::shared_future<int64_t> IStreamMock::ReadAsync(uint8_t*, int64_t, int64_t, std::launch)
{
  return std::future<int64_t>();
}

std::shared_future<int64_t> IStreamMock::WriteAsync(const uint8_t*, int64_t, int64_t, std::launch)
{
  return std::future<int64_t>();
}

std::future<bool> IStreamMock::FlushAsync(std::launch)
{
  return std::future<bool>();
}

int64_t IStreamMock::Read(uint8_t*, int64_t)
{
  return 1;
}

int64_t IStreamMock::Write(const uint8_t*, int64_t)
{
  return 1;
}

bool IStreamMock::Flush()
{
  return true;
}

std::shared_ptr<IStream> IStreamMock::Clone()
{
  return std::make_shared<IStreamMock>();
}

void IStreamMock::Seek(uint64_t)
{
}

bool IStreamMock::CanRead()  const
{
  return true;
}
bool IStreamMock::CanWrite() const
{
  return true;
}
uint64_t IStreamMock::Position()
{
  return 1;
}

uint64_t IStreamMock::Size()
{
  return 1;
}

void IStreamMock::Size(uint64_t)
{
}

IStreamMock::~IStreamMock()
{}
