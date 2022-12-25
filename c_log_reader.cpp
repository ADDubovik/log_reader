#include <Windows.h>

#include "handle_guarded.h"

#include "c_log_reader.h"

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
    HandleGuarded _handle;
};

CLogReader::Impl::Impl()
    : _handle(NULL)
{
}

CLogReader::Impl::~Impl() = default;

bool CLogReader::Impl::Open(const char* filename)
{
    _handle = HandleGuarded(
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

    return _handle;
}

void CLogReader::Impl::Close()
{
    _handle.Close();
}

bool CLogReader::Impl::SetFilter(const char* filter)
{
    return false;
}

bool CLogReader::Impl::GetLineNext(char* buf, const int bufsize)
{
    return false;
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
