#pragma once

#include <Windows.h>

class HandleGuarded
{
public:
    explicit HandleGuarded(HANDLE handle= NULL);

    ~HandleGuarded();

    void Close();

    operator bool() const;
    operator HANDLE() const;

private:
    HANDLE _handle;
};
