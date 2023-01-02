#pragma once

#include <Windows.h>

class HandleGuarded
{
public:
    explicit HandleGuarded(HANDLE handle= NULL);

    ~HandleGuarded();

    HandleGuarded(HandleGuarded&& oth)
    {
        auto temp = _handle;
        _handle = oth._handle;
        oth._handle = temp;
    }
    HandleGuarded(const HandleGuarded&) = delete;

    HandleGuarded& operator=(HandleGuarded&& oth)
    {
        auto temp = _handle;
        _handle = oth._handle;
        oth._handle = temp;

        return *this;
    }
    HandleGuarded& operator=(const HandleGuarded&) = delete;

    void Close();

    operator bool() const;
    operator HANDLE() const;

private:
    HANDLE _handle;
};
