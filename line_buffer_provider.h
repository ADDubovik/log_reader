#pragma once

#include "line_buffer.h"

class LineBufferProvider
{
public:
    LineBufferProvider();
    ~LineBufferProvider();

    bool Open(const char* filename);
    void Close();

    bool SetFilter(const char* filter);
    bool GetLineNext(LineBuffer& line_buffer);

private:
    class Impl;

private:
    Impl* _impl;
};
