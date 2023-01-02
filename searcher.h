#pragma once

#include <Windows.h>

#include "memory.h"

class Searcher
{
public:
    explicit Searcher(char const* pattern = "");

    Searcher(Searcher&&) = default;
    Searcher(const Searcher&) = delete;

    Searcher& operator=(Searcher&&) = default;
    Searcher& operator=(const Searcher&) = delete;

    enum class SymbolMeaning
    {
        Usual,
        Error,
        LineEndSuitable,
        LineEndNonSuitable,
    };

    SymbolMeaning ProcessNextChar(const char ch);

    struct Line
    {
        ULONG64 start_from = 0;
        ULONG64 past_the_end = 0;
    };

    operator bool() const;

private:
    struct SyntaxElement
    {
        char ch;
        size_t index_roll_if_not_match;
        bool reject_if_not_match;
    };

private:
    Memory _syntax;
    size_t _syntax_index;
    ULONG64 _line_start;
    bool _rejected;
};
