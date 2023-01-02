#include <Windows.h>

#include "handle_guarded.h"
#include "memory.h"
#include "searcher.h"
#include "line_buffer.h"

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
    bool ReadBuffer();

    bool MakeLineBufferWithSuitableLine();

private:
    HandleGuarded _file;
    Searcher _searcher;
    LineBuffer _line_buffer;

    size_t _buf_index = buf_size;
    size_t _line_start_index_in_buffer = buf_size;
    size_t _buf_past_the_end_index = 0;
    size_t _buf_position_in_file = 0;
    size_t _read_from_file_total = 0;
    char _buf[buf_size];
};

CLogReader::Impl::Impl() = default;

CLogReader::Impl::~Impl() = default;

bool CLogReader::Impl::Open(const char* filename)
{
    _file = HandleGuarded(
        CreateFileA(
            filename,              // [in]           LPCSTR                lpFileName, 
            GENERIC_READ,          // [in]           DWORD                 dwDesiredAccess,
            FILE_SHARE_READ,       // [in]           DWORD                 dwShareMode,
            NULL,                  // [in, optional] LPSECURITY_ATTRIBUTES lpSecurityAttributes,
            OPEN_EXISTING,         // [in]           DWORD                 dwCreationDisposition,
            FILE_ATTRIBUTE_NORMAL, // [in]           DWORD                 dwFlagsAndAttributes,
            NULL                   // [in, optional] HANDLE                hTemplateFile
        )
    );

    return _file;
}

void CLogReader::Impl::Close()
{
    _file.Close();
}

bool CLogReader::Impl::SetFilter(const char* filter)
{
    _searcher = Searcher(filter);

    return _searcher;
}

bool CLogReader::Impl::GetLineNext(char* buf, const int bufsize)
{
    if (!_searcher || !_file || !_line_buffer  || !buf || !bufsize)
    {
        return false;
    }

    return true;
}

bool CLogReader::Impl::ReadBuffer()
{
    DWORD bytes_read = 0;
    BOOL read_result;

    memset(&_buf, 0, buf_size);

    read_result = ReadFile(
        _file,       // [in]                HANDLE       hFile,
        &_buf,       // [out]               LPVOID       lpBuffer,
        buf_size,    // [in]                DWORD        nNumberOfBytesToRead,
        &bytes_read, // [out, optional]     LPDWORD      lpNumberOfBytesRead,
        NULL         // [in, out, optional] LPOVERLAPPED lpOverlapped
    );

    if (false == read_result || 0 == bytes_read)
    {
        return false;
    }

    _buf_position_in_file = _read_from_file_total;
    _buf_past_the_end_index = bytes_read;
    _read_from_file_total += bytes_read;

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
