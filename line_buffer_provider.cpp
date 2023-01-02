#include <Windows.h>

#include "handle_guarded.h"
#include "searcher.h"
#include "line_buffer.h"

#include "line_buffer_provider.h"

namespace
{
    constexpr size_t c_buf_size = 1024;
} // namespace

class LineBufferProvider::Impl
{
public:
    Impl();
    ~Impl();

    bool Open(const char* filename);
    void Close();

    bool SetFilter(const char* filter);
    bool GetLineNext(LineBuffer& line_buffer);

private:
    bool ReadBuffer();

private:
    HandleGuarded _file;
    Searcher _searcher;

    size_t _buf_index = c_buf_size;
    char _buf[c_buf_size];

};

LineBufferProvider::Impl::Impl() = default;

LineBufferProvider::Impl::~Impl() = default;

bool LineBufferProvider::Impl::Open(const char* filename)
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

void LineBufferProvider::Impl::Close()
{
    _file.Close();
}

bool LineBufferProvider::Impl::SetFilter(const char* filter)
{
    _searcher = Searcher(filter);

    return _searcher;
}

bool LineBufferProvider::Impl::GetLineNext(LineBuffer& line_buffer)
{
    if (!_searcher || !_file || !line_buffer)
    {
        return false;
    }

    do
    {
        if (_buf_index >= c_buf_size)
        {
            if (false == ReadBuffer())
            {
                return false;
            }

            _buf_index = 0;
        }

        const auto ch = _buf[_buf_index];
        const auto search_result = _searcher.ProcessNextChar(ch);

        ++_buf_index;

        switch (search_result)
        {
        case Searcher::SymbolMeaning::Usual:
        {
            if (false == line_buffer.Append(ch))
            {
                return false;
            }

            break;
        }
        case Searcher::SymbolMeaning::Error:
        {
            return false;
        }
        case Searcher::SymbolMeaning::LineEndSuitable:
        {
            if (false == line_buffer.Append(ch))
            {
                return false;
            }

            return true;
        }
        case Searcher::SymbolMeaning::LineEndNonSuitable:
        {
            line_buffer.Clear();
            break;
        }
        }
    } while (true);

    return false;
}

bool LineBufferProvider::Impl::ReadBuffer()
{
    DWORD bytes_read = 0;
    BOOL read_result;

    memset(&_buf, 0, c_buf_size);

    read_result = ReadFile(
        _file,       // [in]                HANDLE       hFile,
        &_buf,       // [out]               LPVOID       lpBuffer,
        c_buf_size,  // [in]                DWORD        nNumberOfBytesToRead,
        &bytes_read, // [out, optional]     LPDWORD      lpNumberOfBytesRead,
        NULL         // [in, out, optional] LPOVERLAPPED lpOverlapped
    );

    if (false == read_result || 0 == bytes_read)
    {
        return false;
    }

    return true;
}

LineBufferProvider::LineBufferProvider()
    : _impl(new Impl)
{
}

LineBufferProvider::~LineBufferProvider()
{
    delete _impl;
    _impl = nullptr;
}

bool LineBufferProvider::Open(const char* filename)
{
    return _impl->Open(filename);
}

void LineBufferProvider::Close()
{
    _impl->Close();
}

bool LineBufferProvider::SetFilter(const char* filter)
{
    return _impl->SetFilter(filter);
}

bool LineBufferProvider::GetLineNext(LineBuffer& line_buffer)
{
    return _impl->GetLineNext(line_buffer);
}
