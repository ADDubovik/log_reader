#include "handle_guarded.h"

HandleGuarded::HandleGuarded(HANDLE handle)
    : _handle(handle)
{
}

HandleGuarded::~HandleGuarded()
{
    Close();
}

void HandleGuarded::Close()
{
    if (*this)
    {
        CloseHandle(_handle);
        _handle = NULL;
    }
}

HandleGuarded::operator bool() const
{
    return
        _handle != NULL &&
        _handle != INVALID_HANDLE_VALUE;
}

HandleGuarded::operator HANDLE() const
{
    return _handle;
}
