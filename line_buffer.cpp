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

bool LineBuffer::Append(
	char const* const data,
	const size_t size)
{
	if (!data || !size || !_buffer)
	{
		return false;
	}

	if (_size + size > _capacity)
	{
		const auto size_needed = _size + size;
		const auto size_multiplied = size + (size >> 1);
		const auto size_new = size_needed < size_multiplied
			? size_multiplied
			: size_needed;

		Memory enlarged(size_new);

		if (!enlarged)
		{
			return false;
		}

		memcpy(enlarged, _buffer, _size);

		_buffer = static_cast<Memory&&>(enlarged);
		_capacity = size_new;
	}

	memcpy(static_cast<char*>(_buffer.get()) + _size, data, size);
	_size += size;

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
