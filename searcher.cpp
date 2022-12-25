#include "searcher.h"

Searcher::Searcher(char const* pattern)
    : _length(strlen(pattern))
    , _pattern(_length + 1)
    , _pattern_current_index(0)
    , _has_roll(false)
    , _pattern_roll_index(0)
    , _line_status(LineStatus::unknown)
{
    if (_pattern)
    {
        memcpy(_pattern, pattern, _length + 1);
    }
}

bool Searcher::process(
    char const* const chunk,
    const size_t start_from_index,
    const size_t past_the_end_index,
    const size_t chunk_position_in_file,
    Line& line)
{
    if (!_pattern)
    {
        return false;
    }

    _line_status = LineStatus::unknown;

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

        if (LineStatus::unknown == _line_status && '\0' == pattern[_pattern_current_index])
        {
            _line_status = LineStatus::suitable;
        }

        switch (ch)
        {
            case '\x0d':
            {
                break;
            }
            case '\x0a':
            {
                _line.past_the_end = chunk_position_in_file + i + 1;
                if (LineStatus::suitable == _line_status)
                {
                    line = _line;
                }
                _line.start_from = _line.past_the_end;

                _pattern_current_index = 0;
                _has_roll = false;
                _pattern_roll_index = 0;

                if (LineStatus::suitable == _line_status)
                {
                    return true;
                }

                skip_asterisks();

                _line_status = LineStatus::unknown;
            }
            default:
            {
                if (LineStatus::unknown == _line_status)
                {
                    const char ch_pattern = pattern[_pattern_current_index];

                    if (ch_pattern)
                    {
                        if (ch_pattern == ch || ch_pattern == '?')
                        {
                            ++_pattern_current_index;

                            skip_asterisks();
                        }
                        else
                        {
                            if (_has_roll)
                            {
                                _pattern_current_index = _pattern_roll_index;
                            }
                            else
                            {
                                _line_status = LineStatus::non_suitable;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

Searcher::operator bool() const
{
    return _pattern;
}
