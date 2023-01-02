#pragma once

#include "memory.h"

class LineBuffer
{
public:
	LineBuffer();

	LineBuffer(LineBuffer&&) noexcept = default;
	LineBuffer(const LineBuffer&) = delete;

	LineBuffer& operator=(LineBuffer&&) noexcept = default;
	LineBuffer& operator=(const LineBuffer&) noexcept = delete;

	bool Append(const char ch);

	void Clear();

	size_t Capacity() const;
	size_t Size() const;
	char const* const Data() const;

	operator bool() const;

private:
	Memory _buffer;
	size_t _size;
	size_t _capacity;
};
