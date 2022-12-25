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

private:
    void* _pointer;
};
