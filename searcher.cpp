#include "searcher.h"

namespace
{
    size_t CountNonAsterisks(char const* pattern)
    {
        size_t result = 0;

        for (char const* pointer = pattern; *pointer; ++pointer)
        {
            if (*pointer != '*')
            {
                ++result;
            }
        }

        return result;
    }
}

Searcher::Searcher(char const* pattern)
    : _syntax(sizeof(SyntaxElement) * (CountNonAsterisks(pattern) + 1))
    , _syntax_index(0)
    , _line_start(0)
    , _rejected(false)
{
    if (_syntax)
    {
        auto syntax_elements = reinterpret_cast<SyntaxElement*>(_syntax.get());

        size_t inserted = 0;
        size_t index_roll_if_not_match = 0;
        bool has_roll = false;

        for (char const* pointer = pattern; *pointer; ++pointer)
        {
            const char ch = *pointer;
            if ('*' == ch)
            {
                has_roll = true;
                index_roll_if_not_match = inserted;
            }
            else
            {
                syntax_elements[inserted].ch = ch;
                syntax_elements[inserted].index_roll_if_not_match = index_roll_if_not_match;
                syntax_elements[inserted].reject_if_not_match = !has_roll;
                syntax_elements[inserted].apply_if_match = false;
                
                ++inserted;
            }
        }

        syntax_elements[inserted].ch = '\x0a';
        syntax_elements[inserted].index_roll_if_not_match = index_roll_if_not_match;
        syntax_elements[inserted].reject_if_not_match = !has_roll;
        syntax_elements[inserted].apply_if_match = true;
    }
}

bool Searcher::process(
    char const* const chunk,
    const size_t start_from_index,
    const size_t past_the_end_index,
    const size_t chunk_position_in_file,
    Line& line)
{
    if (!_syntax)
    {
        return false;
    }

    const auto syntax_elements = reinterpret_cast<SyntaxElement const*>(_syntax.get());

    for (auto i = start_from_index; i < past_the_end_index; ++i)
    {
        const char ch = chunk[i];

        if ('\x0d' == ch)
        {
            continue;
        }

        if (!_rejected)
        {
            const auto& syntax_element = syntax_elements[_syntax_index];

            if (syntax_element.ch == ch || syntax_element.ch == '?')
            {
                if (syntax_element.apply_if_match)
                {
                    line.start_from = _line_start;
                    line.past_the_end = chunk_position_in_file + i + 1;
                    _line_start = line.past_the_end;

                    _syntax_index = 0;
                    _rejected = false;

                    return true;
                }
                else
                {
                    ++_syntax_index;
                }
            }
            else
            {
                if (syntax_element.reject_if_not_match)
                {
                    _rejected = true;
                }
                else
                {
                    _syntax_index = syntax_element.index_roll_if_not_match;
                }
            }
        }

        if ('\x0a' == ch)
        {
            _syntax_index = 0;
            _line_start = chunk_position_in_file + i + 1;
            _rejected = false;
        }
    }

    return false;
}

Searcher::operator bool() const
{
    return _syntax;
}
