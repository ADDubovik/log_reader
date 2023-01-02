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
                
                ++inserted;
            }
        }

        syntax_elements[inserted].ch = '\x0a';
        syntax_elements[inserted].index_roll_if_not_match = index_roll_if_not_match;
        syntax_elements[inserted].reject_if_not_match = !has_roll;
    }
}

Searcher::SymbolMeaning Searcher::ProcessNextChar(const char ch)
{
    if (!_syntax)
    {
        return SymbolMeaning::Error;
    }

    if ('\x0d' == ch)
    {
        return SymbolMeaning::Usual;
    }

    const auto syntax_elements = reinterpret_cast<SyntaxElement const*>(_syntax.get());

    const auto& syntax_element = syntax_elements[_syntax_index];

    if ('\x0a' == ch)
    {
        const auto result = syntax_element.ch == ch
            ? SymbolMeaning::LineEndSuitable
            : SymbolMeaning::LineEndNonSuitable;

        _syntax_index = 0;
        _rejected = false;

        return result;
    }
    else if (syntax_element.ch == ch || syntax_element.ch == '?')
    {
        if (false == _rejected)
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

    return SymbolMeaning::Usual;
}

Searcher::operator bool() const
{
    return _syntax;
}
