#include <Windows.h>

#include "memory.h"

Memory::Memory(size_t size)
    : _pointer(
        size
        ? malloc(size)
        : nullptr
    )
{}

Memory::~Memory()
{
    if (_pointer)
    {
        free(_pointer);
    }
}
