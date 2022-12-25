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

    struct Line
    {
        ULONG64 start_from = 0;
        ULONG64 past_the_end = 0;
    };

    bool process(
        char const* const chunk,
        const size_t start_from_index,
        const size_t past_the_end_index,
        const size_t chunk_position_in_file,
        Line& line);

    operator bool() const;

private:
    struct SyntaxElement
    {
        char ch;
        size_t index_roll_if_not_match;
        bool reject_if_not_match;
        bool apply_if_match;
    };

private:
    Memory _syntax;
    size_t _syntax_index;
    ULONG64 _line_start;
    bool _rejected;
};
