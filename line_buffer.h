#pragma once

#include "memory.h"

class LineBuffer
{
public:
	LineBuffer();

	bool Append(
		char const* const data,
		const size_t size);

	void Clear();

	size_t Capacity() const;
	size_t Size() const;
	char const* const Data() const;

private:
	Memory _buffer;
	size_t _size;
	size_t _capacity;
};
