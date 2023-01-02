#include <Windows.h>

#include "line_buffer_provider.h"

#include "log_reader.h"

namespace
{
    constexpr size_t buf_size = 1024;
}

class CLogReader::Impl
{
public:
    Impl();

    ~Impl();

    bool Open(const char* filename);
    void Close();

    bool SetFilter(const char* filter);
    bool GetLineNext(char* buf, const int bufsize);

private:
    LineBuffer _line_buffer;
    LineBufferProvider _line_buffer_provider;

    size_t _line_buffer_position = 0;
};

CLogReader::Impl::Impl() = default;

CLogReader::Impl::~Impl() = default;

bool CLogReader::Impl::Open(const char* filename)
{
    return _line_buffer_provider.Open(filename);
}

void CLogReader::Impl::Close()
{
    _line_buffer_provider.Close();
}

bool CLogReader::Impl::SetFilter(const char* filter)
{
    return _line_buffer_provider.SetFilter(filter);
}

bool CLogReader::Impl::GetLineNext(char* buf, const int bufsize)
{
    if (!buf || !bufsize || !_line_buffer)
    {
        return false;
    }

    if (_line_buffer.Size() <= _line_buffer_position)
    {
        _line_buffer.Clear();

        if (false == _line_buffer_provider.GetLineNext(_line_buffer) || false == _line_buffer || 0 == _line_buffer.Size())
        {
            return false;
        }        

        _line_buffer_position = 0;
    }

    const auto line_buffer_size = _line_buffer.Size();
    const auto size_to_copy = line_buffer_size < bufsize
        ? line_buffer_size
        : bufsize;

    memcpy(buf, _line_buffer.Data() + _line_buffer_position, size_to_copy);

    _line_buffer_position += size_to_copy;

    return true;
}

CLogReader::CLogReader()
    : _impl(new Impl)
{
}

CLogReader::~CLogReader()
{
    delete _impl;
    _impl = nullptr;
}

bool CLogReader::Open(const char* filename)
{
    return _impl->Open(filename);
}

void CLogReader::Close()
{
    _impl->Close();
}

bool CLogReader::SetFilter(const char* filter)
{
	return _impl->SetFilter(filter);
}

bool CLogReader::GetLineNext(char* buf, const int bufsize)
{
	return _impl->GetLineNext(buf, bufsize);
}
