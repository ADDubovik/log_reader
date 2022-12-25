#include "searcher.h"

Searcher::Searcher(char const* pattern)
    : _length(strlen(pattern))
    , _pattern(_length)
    , _pattern_current_index(0)
    , _has_roll(false)
    , _pattern_roll_index(0)
    , _is_line_suitable(false)
{
    if (_pattern)
    {
        memcpy(_pattern, pattern, _length);
    }
}

bool Searcher::process(
    char const* const chunk,
    const size_t start_from_index,
    const size_t past_the_end_index,
    const size_t chunk_position_in_file,
    Line& line)
{
    char const* const pattern = reinterpret_cast<char const*>(_pattern.get());

    const auto skip_asterisks = [this, pattern]() {
        while ('*' == pattern[_pattern_current_index])
        {
            _has_roll = true;
            ++_pattern_current_index;
            _pattern_roll_index = _pattern_current_index;
        }
    };

    skip_asterisks();

    for (auto i = start_from_index; i < past_the_end_index; ++i)
    {
        const char ch = chunk[i];
        switch (ch)
        {
        case '\x0d':
        {
            break;
        }
        case '\x0a':
        {
            _line.past_the_end = chunk_position_in_file + i + 1;

            line = _line;

            _line.start_from = _line.past_the_end;

            //_pattern_current_index = 0;

            //skip_asterisks();

            return true;
        }
        default:
        {
            //if (pattern[_pattern_current_index])
            //{
            //    if (ch == pattern[_pattern_current_index])
            //    {
            //
            //    }
            //}
        }
        }
    }

    return false;
}

Searcher::operator bool() const
{
    return _pattern;
}
