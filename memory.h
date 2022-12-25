#pragma once

class Memory
{
public:
    explicit Memory(size_t size = 0);

    ~Memory();

    operator void* () const
    {
        return _pointer;
    }

    void* get() const
    {
        return _pointer;
    }

    operator bool() const
    {
        return _pointer != nullptr;
    }

private:
    void* _pointer;
};
