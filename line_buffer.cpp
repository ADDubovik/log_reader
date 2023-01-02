#include <Windows.h>

#include "line_buffer.h"

namespace {

	constexpr size_t initial_capacity = 1024;

} // namespace

LineBuffer::LineBuffer()
	: _buffer(initial_capacity)
	, _size(0)
	, _capacity(initial_capacity)
{
}

bool LineBuffer::Append(const char ch)
{
	if (!_buffer)
	{
		return false;
	}

	if (_size >= _capacity)
	{
		const auto capacity_new = _capacity + (_capacity >> 1);

		Memory enlarged(capacity_new);

		if (!enlarged)
		{
			return false;
		}

		memcpy(enlarged, _buffer, _size);

		_buffer = static_cast<Memory&&>(enlarged);
		_capacity = capacity_new;
	}

	static_cast<char*>(_buffer.get())[_size] = ch;
	++_size;

	return true;
}

void LineBuffer::Clear()
{
	_size = 0;
}

size_t LineBuffer::Capacity() const
{
	return _capacity;
}

size_t LineBuffer::Size() const
{
	return _size;
}

char const* const LineBuffer::Data() const
{
	return static_cast<char const*>(_buffer.get());
}

LineBuffer::operator bool() const
{
	return _buffer;
}
